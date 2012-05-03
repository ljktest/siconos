/* Siconos-Kernel, Copyright INRIA 2005-2011.
 * Siconos is a program dedicated to modeling, simulation and control
 * of non smooth dynamical systems.
 * Siconos is a free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * Siconos is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Siconos; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Contact: Vincent ACARY, siconos-team@lists.gforge.inria.fr
 */
#include "MLCPProjectOnConstraints.hpp"
#include "MixedComplementarityConditionNSL.hpp"
#include "EqualityConditionNSL.hpp"
#include "Simulation.hpp"
#include "LagrangianR.hpp"
#include "NewtonEulerDS.hpp"
#include "NewtonEulerR.hpp"
#include "NewtonEulerFrom1DLocalFrameR.hpp"
#include "OSNSMatrixProjectOnConstraints.hpp"
#include "LagrangianLinearTIDS.hpp"

using namespace std;
using namespace RELATION;
using namespace Siconos;
//#define MLCPPROJ_DEBUG

void MLCPProjectOnConstraints::initOSNSMatrix()
{
  _M.reset(new OSNSMatrixProjectOnConstraints(0, 0, _MStorageType));
  _n = 0;
  _m = 0;
  _curBlock = 0;
}


// Constructor from a set of data
MLCPProjectOnConstraints::MLCPProjectOnConstraints(const int newNumericsSolverId, double alphaval):
  MLCP(newNumericsSolverId), _alpha(alphaval)
{
  _levelMin = 1;
  _levelMax = 1;
}


void MLCPProjectOnConstraints::display() const
{
  cout << "======= MLCPProjectOnConstraints of size " << _sizeOutput << " with: " << endl;
  cout << "======= m " << _m << " _n " << _n << endl;
  LinearOSNS::display();
}
void MLCPProjectOnConstraints::updateInteractionBlocks()
{
  // The present functions checks various conditions and possibly
  // compute interactionBlocks matrices.
  //
  // Let interi and interj be two Interactions.
  //
  // Things to be checked are:
  //  1 - is the topology time invariant?
  //  2 - does interactionBlocks[interi][interj] already exists (ie has been
  //  computed in a previous time step)?
  //  3 - do we need to compute this interactionBlock? A interactionBlock is
  //  to be computed if interi and interj are in IndexSet1 AND if interi and
  //  interj have common DynamicalSystems.
  //
  // The possible cases are:
  //
  //  - If 1 and 2 are true then it does nothing. 3 is not checked.
  //  - If 1 == true, 2 == false, 3 == false, it does nothing.
  //  - If 1 == true, 2 == false, 3 == true, it computes the
  //    interactionBlock.
  //  - If 1==false, 2 is not checked, and the interactionBlock is
  //    computed if 3==true.
  //

  // Get index set from Simulation
  SP::InteractionsGraph indexSet = simulation()->indexSet(_levelMin);


  // It seems that index() in not update in Index(0)
  // see comment in void Simulation::updateIndexSets()
  //if (_levelMin==0)
  {
    indexSet->update_vertices_indices();
    indexSet->update_edges_indices();
  }
  bool isLinear = simulation()->model()->nonSmoothDynamicalSystem()->isLinear();

  // we put diagonal informations on vertices
  // self loops with bgl are a *nightmare* at the moment
  // (patch 65198 on standard boost install)

  if (indexSet->properties()->symmetric)
  {
    RuntimeException::selfThrow
    (" MLCPProjectOnConstraints::updateInteractionBlocks() - not yet implemented for symmetric case");
  }
  else // not symmetric => follow out_edges for each vertices
  {
    if (!_hasBeUpdated)
    {
      //      printf("MLCPProjectOnConstraints::updateInteractionBlocks must be updated.\n");
      _n = 0;
      _m = 0;
      _curBlock = 0;
    }
    InteractionsGraph::VIterator vi, viend;
    for (boost::tie(vi, viend) = indexSet->vertices();
         vi != viend; ++vi)
    {

      SP::Interaction inter = indexSet->bundle(*vi);
      unsigned int nslawSize = boost::static_pointer_cast<OSNSMatrixProjectOnConstraints>
                               (_M)->computeSizeForProjection(inter);

      if (! indexSet->blockProj[*vi])
      {
        indexSet->blockProj[*vi].reset(new SimpleMatrix(nslawSize, nslawSize));
      }

      if (!isLinear || !_hasBeUpdated)
      {
        computeDiagonalInteractionBlock(*vi);
      }

      /* interactionBlock must be zeroed at init */
      std::vector<bool> initialized;
      initialized.resize(indexSet->edges_number());
      std::fill(initialized.begin(), initialized.end(), false);

      /* on a undirected graph, out_edges gives all incident edges */
      InteractionsGraph::OEIterator oei, oeiend;
      for (boost::tie(oei, oeiend) = indexSet->out_edges(*vi);
           oei != oeiend; ++oei)
      {

        /* on adjoint graph there is at most 2 edges between source and target */
        InteractionsGraph::EDescriptor ed1, ed2;
        boost::tie(ed1, ed2) = indexSet->edges(indexSet->source(*oei), indexSet->target(*oei));

        assert(*oei == ed1 || *oei == ed2);

        /* the first edge as the lower index */
        assert(indexSet->index(ed1) <= indexSet->index(ed2));

        SP::Interaction inter1 = indexSet->bundle(indexSet->source(*oei));
        SP::Interaction inter2 = indexSet->bundle(indexSet->target(*oei));

        // Memory allocation if needed
        unsigned int nslawSize1 = boost::static_pointer_cast<OSNSMatrixProjectOnConstraints>
                                  (_M)->computeSizeForProjection(inter);
        unsigned int nslawSize2 = boost::static_pointer_cast<OSNSMatrixProjectOnConstraints>
                                  (_M)->computeSizeForProjection(inter);
        unsigned int isrc = indexSet->index(indexSet->source(*oei));
        unsigned int itar = indexSet->index(indexSet->target(*oei));

        SP::SiconosMatrix currentInteractionBlock;

        if (itar > isrc) // upper block
        {
          if (! indexSet->upper_blockProj[ed1])
          {
            indexSet->upper_blockProj[ed1].reset(new SimpleMatrix(nslawSize1, nslawSize2));
            if (ed2 != ed1)
              indexSet->upper_blockProj[ed2] = indexSet->upper_blockProj[ed1];
          }
          currentInteractionBlock = indexSet->upper_blockProj[ed1];

        }
        else  // lower block
        {
          if (! indexSet->lower_blockProj[ed1])
          {
            indexSet->lower_blockProj[ed1].reset(new SimpleMatrix(nslawSize1, nslawSize2));
            if (ed2 != ed1)
              indexSet->lower_blockProj[ed2] = indexSet->lower_blockProj[ed1];
          }
          currentInteractionBlock = indexSet->lower_blockProj[ed1];
        }


        if (!initialized[indexSet->index(ed1)])
        {
          initialized[indexSet->index(ed1)] = true;
          currentInteractionBlock->zero();
        }


        if (!isLinear || !_hasBeUpdated)
        {
          if (isrc != itar)
            computeInteractionBlock(*oei);
        }

      }
    }
  }
#ifdef MLCPPROJ_DEBUG
  displayBlocks(indexSet);
#endif

}
void MLCPProjectOnConstraints::displayBlocks(SP::InteractionsGraph indexSet)
{

  std::cout <<  "MLCPProjectOnConstraints::displayBlocks(SP::InteractionsGraph indexSet) " << std::endl;
  std::cout << "                          indexSet :" << indexSet << std::endl;


  InteractionsGraph::VIterator vi, viend;
  for (boost::tie(vi, viend) = indexSet->vertices();
       vi != viend; ++vi)
  {
    SP::Interaction inter = indexSet->bundle(*vi);
    std::cout << "                          vertex :" << *vi << std::endl;
    std::cout << "                          bundle :" << indexSet->bundle(*vi) << std::endl;

    if (indexSet->blockProj[*vi])
    {
      std::cout << "                          blockProj ";
      indexSet->blockProj[*vi]->display();
    }

    InteractionsGraph::OEIterator oei, oeiend;



    for (boost::tie(oei, oeiend) = indexSet->out_edges(*vi);
         oei != oeiend; ++oei)
    {
      unsigned int isrc = indexSet->index(indexSet->source(*oei));
      unsigned int itar = indexSet->index(indexSet->target(*oei));
      std::cout << "                          isrc :" << isrc << std::endl;
      std::cout << "                          itar :" << itar << std::endl;


      InteractionsGraph::EDescriptor ed1, ed2;
      std::cout << "                          outedges :" << *oei << std::endl;
      boost::tie(ed1, ed2) = indexSet->edges(indexSet->source(*oei), indexSet->target(*oei));
      std::cout << "                          edges(ed1,ed2) :" << ed1 << " " << ed2  << std::endl;
      std::cout << "                          (ed1)->upper_blockProj : ";
      if (indexSet->upper_blockProj[ed1])
      {
        std::cout << indexSet->upper_blockProj[ed1] << "   :" ;
        indexSet->upper_blockProj[ed1]->display();
      }
      else
        std::cout << "NULL " << std::endl;

      std::cout << "                          (ed1)->lower_blockProj : ";
      if (indexSet->lower_blockProj[ed1])
      {
        std::cout << indexSet->lower_blockProj[ed1] << "   :" ;
        indexSet->lower_blockProj[ed1]->display();
      }
      else
        std::cout << "NULL " << std::endl;

      std::cout << "                          (ed2)->upper_blockProj : ";
      if (indexSet->upper_blockProj[ed2])
      {
        std::cout << indexSet->upper_blockProj[ed2] << "   :" ;
        indexSet->upper_blockProj[ed2]->display();
      }
      else
        std::cout << "NULL" << std::endl;

      std::cout << "                          (ed2)->lower_blockProj : ";
      if (indexSet->lower_blockProj[ed2])
      {
        std::cout << indexSet->lower_blockProj[ed2] << "   :" ;
        indexSet->lower_blockProj[ed2]->display();
      }
      else
        std::cout << "NULL" << std::endl;
    }

  }
}
void MLCPProjectOnConstraints::updateInteractionBlocksOLD()
{

  SP::InteractionsGraph indexSet = simulation()->indexSet(levelMin());

  bool isLinear = simulation()->model()->nonSmoothDynamicalSystem()->isLinear();
  //  cout<<"isLinear: "<<isLinear<<" hasTopologyChanged: "<<hasTopologyChanged<<"hasBeUpdated: "<<_hasBeUpdated<<endl;


  if (indexSet->properties()->symmetric)
  {
    RuntimeException::selfThrow
    ("MLCPProjectOnConstraints::updateInteractionBlocks() - symmetric case for the indexSet is not yet implemented");
  }
  else // not symmetric => follow out_edges for each vertices
  {
    if (!_hasBeUpdated || !isLinear)
    {
      if (!_hasBeUpdated)
      {
        //      printf("MLCPProjectOnConstraints::updateInteractionBlocks must be updated.\n");
        _n = 0;
        _m = 0;
        _curBlock = 0;
      }
      InteractionsGraph::VIterator vi, viend;
      for (boost::tie(vi, viend) = indexSet->vertices();
           vi != viend; ++vi)
      {
        SP::Interaction inter = indexSet->bundle(*vi);
        unsigned int sizeY = 0;
        sizeY = boost::static_pointer_cast<OSNSMatrixProjectOnConstraints>
                (_M)->computeSizeForProjection(inter);

        // #ifdef MLCPPROJ_DEBUG
        //       cout<<"\nMLCPProjectOnConstraints::updateInteractionBlocks()"<<endl;
        //       std::cout << "indexSet :"<< indexSet << std::endl;
        //       indexSet->display();
        //       std::cout << "vi :"<< *vi << std::endl;
        //       std::cout << "indexSet->blockProj[*vi]: before"<< indexSet->blockProj[*vi] << std::endl;
        // #endif

        if (! indexSet->blockProj[*vi])
        {
          indexSet->blockProj[*vi].reset(new SimpleMatrix(sizeY, sizeY));
        }
        // #ifdef MLCPPROJ_DEBUG
        //       std::cout << "indexSet->blockProj[*vi]: after"<< indexSet->blockProj[*vi] << std::endl;
        // #endif

        computeDiagonalInteractionBlock(*vi);
      }





      InteractionsGraph::EIterator ei, eiend;
      for (boost::tie(ei, eiend) = indexSet->edges();
           ei != eiend; ++ei)
      {
        SP::Interaction inter1 = indexSet->bundle(indexSet->source(*ei));
        SP::Interaction inter2 = indexSet->bundle(indexSet->target(*ei));
        unsigned int sizeY1 = 0;
        unsigned int sizeY2 = 0;
        sizeY1 = boost::static_pointer_cast<OSNSMatrixProjectOnConstraints>
                 (_M)->computeSizeForProjection(inter1);
        sizeY2 = boost::static_pointer_cast<OSNSMatrixProjectOnConstraints>
                 (_M)->computeSizeForProjection(inter2);

        // Memory allocation if needed
        unsigned int isrc = indexSet->index(indexSet->source(*ei));
        unsigned int itar = indexSet->index(indexSet->target(*ei));
        if (itar > isrc) // upper block
        {
          if (! indexSet->upper_blockProj[*ei])
          {
            indexSet->upper_blockProj[*ei].reset(new SimpleMatrix(sizeY1, sizeY2));
          }
        }
        else  // lower block
        {
          if (! indexSet->lower_blockProj[*ei])
          {
            indexSet->lower_blockProj[*ei].reset(new SimpleMatrix(sizeY1, sizeY2));
          }
        }

        // Computation of the diagonal block
        computeInteractionBlock(*ei);

        // allocation for transposed block
        // should be avoided
        if (itar > isrc) // upper block has been computed
        {
          // if (!indexSet->lower_blockProj[*ei])
          //   {
          //     indexSet->lower_blockProj[*ei].
          //  reset(new SimpleMatrix(indexSet->upper_blockProj[*ei]->size(1),
          //             indexSet->upper_blockProj[*ei]->size(0)));
          //   }
          indexSet->lower_blockProj[*ei].reset(new SimpleMatrix(*(indexSet->upper_blockProj[*ei])));
          indexSet->lower_blockProj[*ei]->trans();
          //          indexSet->lower_blockProj[*ei]->trans(*indexSet->upper_blockProj[*ei]);
        }
        else
        {
          assert(itar < isrc);    // lower block has been computed
          // if (!indexSet->upper_blockProj[*ei])
          //   {
          //     indexSet->upper_blockProj[*ei].
          //  reset(new SimpleMatrix(indexSet->lower_blockProj[*ei]->size(1),
          //             indexSet->lower_blockProj[*ei]->size(0)));
          //   }
          indexSet->upper_blockProj[*ei].
          reset(new SimpleMatrix(*(indexSet->lower_blockProj[*ei])));
          indexSet->upper_blockProj[*ei]->trans();
        }
        // #ifdef MLCPPROJ_DEBUG
        //             printf("MLCPP upper: %i %i\n",indexSet->upper_blockProj[*ei]->size(0),indexSet->upper_blockProj[*ei]->size(1));
        //             printf("MLCPP lower: %i %i\n",indexSet->lower_blockProj[*ei]->size(0),indexSet->lower_blockProj[*ei]->size(1));
        // #endif

      }

    }

  }

}

void MLCPProjectOnConstraints::computeDiagonalInteractionBlock(const InteractionsGraph::VDescriptor& vd)
{
  SP::InteractionsGraph indexSet = simulation()->indexSet(levelMin());

  SP::DynamicalSystem DS1 = indexSet->properties(vd).source;
  SP::DynamicalSystem DS2 = indexSet->properties(vd).target;
  SP::Interaction inter = indexSet->bundle(vd);

  unsigned int sizeY = 0;
  sizeY = boost::static_pointer_cast<OSNSMatrixProjectOnConstraints>
          (_M)->computeSizeForProjection(inter);


#ifdef MLCPPROJ_DEBUG
  cout << "\nMLCPProjectOnConstraints::computeDiagonalInteractionBlock" << endl;
  std::cout << "levelMin()" << levelMin() << std::endl;
  std::cout << "indexSet :" << indexSet << std::endl;
  std::cout << "vd :" << vd << std::endl;
  indexSet->display();
  // std::cout << "DS1 :" << std::endl;
  // DS1->display();
  // std::cout << "DS2 :" << std::endl;
  // DS2->display();
#endif
  assert(indexSet->blockProj[vd]);
  SP::SiconosMatrix currentInteractionBlock = indexSet->blockProj[vd];


  assert(currentInteractionBlock->size(0) == sizeY);
  assert(currentInteractionBlock->size(1) == sizeY);

  if (!_hasBeUpdated)
    computeOptions(inter, inter);
  // Computes matrix _interactionBlocks[inter1][inter2] (and allocates memory if
  // necessary) if inter1 and inter2 have commond DynamicalSystem.  How
  // _interactionBlocks are computed depends explicitely on the type of
  // Relation of each Interaction.

  // Warning: we suppose that at this point, all non linear
  // operators (G for lagrangian relation for example) have been
  // computed through plug-in mechanism.

  // Get the W and Theta maps of one of the Interaction -
  // Warning: in the current version, if OSI!=Moreau, this fails.
  // If OSI = MOREAU, centralInteractionBlocks = W if OSI = LSODAR,
  // centralInteractionBlocks = M (mass matrices)
  MapOfDSMatrices centralInteractionBlocks;
  getOSIMaps(inter , centralInteractionBlocks);

  SP::SiconosMatrix leftInteractionBlock, rightInteractionBlock, leftInteractionBlock1;


  // General form of the interactionBlock is : interactionBlock =
  // a*extraInteractionBlock + b * leftInteractionBlock * centralInteractionBlocks
  // * rightInteractionBlock a and b are scalars, centralInteractionBlocks a
  // matrix depending on the integrator (and on the DS), the
  // simulation type ...  left, right and extra depend on the relation
  // type and the non smooth law.




  currentInteractionBlock->zero();

  // loop over the common DS
  bool endl = false;
  for (SP::DynamicalSystem ds = DS1; !endl; ds = DS2)
  {
    assert(ds == DS1 || ds == DS2);
    endl = (ds == DS2);

    if (Type::value(*ds) == Type::LagrangianLinearTIDS ||
        Type::value(*ds) == Type::LagrangianDS)
    {
      if (inter->getRelationType() != Lagrangian)
      {
        RuntimeException::selfThrow(
          "MLCPProjectOnConstraints::computeDiagonalInteractionBlock - relation is not of type Lagrangian with a LagrangianDS.");
      }


      SP::LagrangianDS lds = (boost::static_pointer_cast<LagrangianDS>(ds));
      unsigned int sizeDS = lds->getDim();
      leftInteractionBlock.reset(new SimpleMatrix(sizeY, sizeDS));
      inter->getLeftInteractionBlockForDS(ds, leftInteractionBlock);

      if (lds->boundaryConditions()) // V.A. Should we do that ?
      {
        for (vector<unsigned int>::iterator itindex =
               lds->boundaryConditions()->velocityIndices()->begin() ;
             itindex != lds->boundaryConditions()->velocityIndices()->end();
             ++itindex)
        {
          // (sizeY,sizeDS));
          SP::SiconosVector coltmp(new SimpleVector(sizeY));
          coltmp->zero();
          leftInteractionBlock->setCol(*itindex, *coltmp);
        }
      }
      // (inter1 == inter2)
      SP::SiconosMatrix work(new SimpleMatrix(*leftInteractionBlock));
      //
      //        cout<<"LinearOSNS : leftUBlock\n";
      //        work->display();
      work->trans();
      //        cout<<"LinearOSNS::computeInteractionBlock leftInteractionBlock"<<endl;
      //        leftInteractionBlock->display();
      centralInteractionBlocks[ds->number()]->PLUForwardBackwardInPlace(*work);
      //*currentInteractionBlock +=  *leftInteractionBlock ** work;


      prod(*leftInteractionBlock, *work, *currentInteractionBlock, false);
      //      gemm(CblasNoTrans,CblasNoTrans,1.0,*leftInteractionBlock,*work,1.0,*currentInteractionBlock);
      //*currentInteractionBlock *=h;
    }
    else if (Type::value(*ds) == Type::NewtonEulerDS)
    {

      if (inter->getRelationType() != NewtonEuler)
      {
        RuntimeException::selfThrow("MLCPProjectOnConstraints::computeDiagonalInteractionBlock - relation is not from NewtonEulerR.");
      }
      SP::NewtonEulerDS neds = (boost::static_pointer_cast<NewtonEulerDS>(ds));
#ifdef MLCPPROJ_WITH_CT

      unsigned int sizeDS = neds->getDim();
      SP::SimpleMatrix T = neds->T();
      SP::SimpleMatrix workT(new SimpleMatrix(*T));
      workT->trans();
      SP::SimpleMatrix workT2(new SimpleMatrix(6, 6));
      prod(*workT, *T, *workT2, true);
      leftInteractionBlock.reset(new SimpleMatrix(sizeY, sizeDS));
      inter->getLeftInteractionBlockForDS(ds, leftInteractionBlock);
      SP::SiconosMatrix work(new SimpleMatrix(*leftInteractionBlock));
      cout << "LinearOSNS : leftUBlock\n";
      work->display();
      work->trans();
      cout << "LinearOSNS::computeInteractionBlock workT2" << endl;
      workT2->display();
      workT2->PLUForwardBackwardInPlace(*work);
      prod(*leftInteractionBlock, *work, *currentInteractionBlock, false);
#else
      if (0) //(boost::static_pointer_cast<NewtonEulerR> inter->relation())->_isConstact){
      {
        unsigned int sizeDS = neds->getDim();
        SP::SimpleMatrix T = neds->T();
        SP::SimpleMatrix workT(new SimpleMatrix(*T));
        workT->trans();
        SP::SimpleMatrix workT2(new SimpleMatrix(6, 6));
        prod(*workT, *T, *workT2, true);
        leftInteractionBlock1.reset(new SimpleMatrix(sizeY, sizeDS));
        inter->getLeftInteractionBlockForDS(ds, leftInteractionBlock);
        leftInteractionBlock.reset(new SimpleMatrix(1, sizeDS));
        for (unsigned int ii = 0; ii < sizeDS; ii++)
          leftInteractionBlock->setValue(1, ii, leftInteractionBlock1->getValue(1, ii));

        SP::SiconosMatrix work(new SimpleMatrix(*leftInteractionBlock));
        //cout<<"LinearOSNS : leftUBlock\n";
        //work->display();
        work->trans();
        //cout<<"LinearOSNS::computeInteractionBlock workT2"<<endl;
        //workT2->display();
        workT2->PLUForwardBackwardInPlace(*work);
        prod(*leftInteractionBlock, *work, *currentInteractionBlock, false);
      }
      else
      {
        unsigned int sizeDS = (boost::static_pointer_cast<NewtonEulerDS>(ds))->getqDim();
        leftInteractionBlock.reset(new SimpleMatrix(sizeY, sizeDS));
        inter->getLeftInteractionBlockForDSProjectOnConstraints(ds, leftInteractionBlock);
        SP::SiconosMatrix work(new SimpleMatrix(*leftInteractionBlock));
        //cout<<"LinearOSNS sizeY="<<sizeY<<": leftUBlock\n";
        //work->display();
        work->trans();
        prod(*leftInteractionBlock, *work, *currentInteractionBlock, false);
      }

    }
    else
      RuntimeException::selfThrow("MLCPProjectOnConstraints::computeDiagonalInteractionBlock - ds is not from NewtonEulerDS neither a LagrangianDS.");



#endif
#ifdef MLCPPROJ_DEBUG
      std::cout << "MLCPProjectOnConstraints::computeDiagonalInteractionBlock DiaginteractionBlock " << std::endl;
      currentInteractionBlock->display();
#endif
    }
  }

  void MLCPProjectOnConstraints::computeInteractionBlock(const InteractionsGraph::EDescriptor& ed)
  {

    // Computes matrix _interactionBlocks[inter1][inter2] (and allocates memory if
    // necessary) if inter1 and inter2 have commond DynamicalSystem.  How
    // _interactionBlocks are computed depends explicitely on the type of
    // Relation of each Interaction.

    // Warning: we suppose that at this point, all non linear
    // operators (G for lagrangian relation for example) have been
    // computed through plug-in mechanism.

    // Get dimension of the NonSmoothLaw (ie dim of the interactionBlock)
    SP::InteractionsGraph indexSet = simulation()->indexSet(_levelMin);

    SP::DynamicalSystem ds = indexSet->bundle(ed);
    SP::Interaction inter1 = indexSet->bundle(indexSet->source(ed));
    SP::Interaction inter2 = indexSet->bundle(indexSet->target(ed));

    unsigned int index1 = indexSet->index(indexSet->source(ed));
    unsigned int index2 = indexSet->index(indexSet->target(ed));

    unsigned int sizeY1 = 0;
    sizeY1 = boost::static_pointer_cast<OSNSMatrixProjectOnConstraints>
             (_M)->computeSizeForProjection(inter1);
    unsigned int sizeY2 = 0;
    sizeY2 = boost::static_pointer_cast<OSNSMatrixProjectOnConstraints>
             (_M)->computeSizeForProjection(inter2);



    /*
      DynamicalSystemsSet commonDS;
      intersection(*inter1->dynamicalSystems(),*inter2->dynamicalSystems(), commonDS);
      assert (!commonDS.isEmpty()) ;
      for (DSIterator itDS = commonDS.begin(); itDS!=commonDS.end(); itDS++)
      {
      assert (*itDS == ds);
      }
    */
    MapOfDSMatrices centralInteractionBlocks;
    getOSIMaps(inter1, centralInteractionBlocks);

    SP::SiconosMatrix currentInteractionBlock;

    assert(index1 != index2);

    if (index2 > index1) // upper block
    {
      //     if (! indexSet->properties(ed).upper_block)
      //     {
      //       indexSet->properties(ed).upper_block.reset(new SimpleMatrix(sizeY1, sizeY2));
      //     }

      assert(indexSet->upper_blockProj[ed]->size(0) == sizeY1);
      assert(indexSet->upper_blockProj[ed]->size(1) == sizeY2);

      currentInteractionBlock = indexSet->upper_blockProj[ed];
    }
    else  // lower block
    {
      //     if (! indexSet->properties(ed).lower_block)
      //     {
      //       indexSet->properties(ed).lower_block.reset(new SimpleMatrix(sizeY1, sizeY2));
      //     }

      assert(indexSet->lower_blockProj[ed]->size(0) == sizeY1);
      assert(indexSet->lower_blockProj[ed]->size(1) == sizeY2);

      currentInteractionBlock = indexSet->lower_blockProj[ed];
    }


    SP::SiconosMatrix leftInteractionBlock, rightInteractionBlock;

    RELATION::TYPES relationType1, relationType2;

    // General form of the interactionBlock is : interactionBlock =
    // a*extraInteractionBlock + b * leftInteractionBlock * centralInteractionBlocks
    // * rightInteractionBlock a and b are scalars, centralInteractionBlocks a
    // matrix depending on the integrator (and on the DS), the
    // simulation type ...  left, right and extra depend on the relation
    // type and the non smooth law.
    relationType1 = inter1->getRelationType();
    relationType2 = inter2->getRelationType();
    if (relationType1 == NewtonEuler &&
        relationType2 == NewtonEuler)
    {
      assert(inter1 != inter2);
      currentInteractionBlock->zero();
#ifdef MLCPPROJ_WITH_CT
      unsigned int sizeDS = (boost::static_pointer_cast<NewtonEulerDS>(ds))->getDim();
      leftInteractionBlock.reset(new SimpleMatrix(sizeY1, sizeDS));
      inter1->getLeftInteractionBlockForDS(ds, leftInteractionBlock);
      SP::NewtonEulerDS neds = (boost::static_pointer_cast<NewtonEulerDS>(ds));
      SP::SimpleMatrix T = neds->T();
      SP::SimpleMatrix workT(new SimpleMatrix(*T));
      workT->trans();
      SP::SimpleMatrix workT2(new SimpleMatrix(6, 6));
      prod(*workT, *T, *workT2, true);
      rightInteractionBlock.reset(new SimpleMatrix(sizeY2, sizeDS));
      inter2->getLeftInteractionBlockForDS(ds, rightInteractionBlock);
      rightInteractionBlock->trans();
      workT2->PLUForwardBackwardInPlace(*rightInteractionBlock);
      prod(*leftInteractionBlock, *rightInteractionBlock, *currentInteractionBlock, false);

#else

      unsigned int sizeDS = (boost::static_pointer_cast<NewtonEulerDS>(ds))->getqDim();
      leftInteractionBlock.reset(new SimpleMatrix(sizeY1, sizeDS));
      inter1->getLeftInteractionBlockForDSProjectOnConstraints(ds, leftInteractionBlock);
      SP::NewtonEulerDS neds = (boost::static_pointer_cast<NewtonEulerDS>(ds));
      rightInteractionBlock.reset(new SimpleMatrix(sizeY2, sizeDS));
      inter2->getLeftInteractionBlockForDSProjectOnConstraints(ds, rightInteractionBlock);
      rightInteractionBlock->trans();
      prod(*leftInteractionBlock, *rightInteractionBlock, *currentInteractionBlock, false);
    }
#endif
      else if (relationType1 == Lagrangian &&
               relationType2 == Lagrangian)
      {
        unsigned int sizeDS =  ds->getDim();
        leftInteractionBlock.reset(new SimpleMatrix(sizeY1, sizeDS));
        inter1->getLeftInteractionBlockForDS(ds, leftInteractionBlock);

        Type::Siconos dsType = Type::value(*ds);
        if (dsType == Type::LagrangianLinearTIDS || dsType == Type::LagrangianDS)
        {
          SP::LagrangianDS d = boost::static_pointer_cast<LagrangianDS> (ds);

          if (d->boundaryConditions()) // V.A. Should we do that ?
          {
            for (vector<unsigned int>::iterator itindex =
                   d->boundaryConditions()->velocityIndices()->begin() ;
                 itindex != d->boundaryConditions()->velocityIndices()->end();
                 ++itindex)
            {
              // (sizeY1,sizeDS));
              SP::SiconosVector coltmp(new SimpleVector(sizeY1));
              coltmp->zero();
              leftInteractionBlock->setCol(*itindex, *coltmp);
            }
          }
        }

        // inter1 != inter2
        rightInteractionBlock.reset(new SimpleMatrix(sizeY2, sizeDS));
        inter2->getLeftInteractionBlockForDS(ds, rightInteractionBlock);

        // Warning: we use getLeft for Right interactionBlock
        // because right = transpose(left) and because of
        // size checking inside the getBlock function, a
        // getRight call will fail.
        rightInteractionBlock->trans();
        centralInteractionBlocks[ds->number()]->PLUForwardBackwardInPlace(*rightInteractionBlock);
        //*currentInteractionBlock +=  *leftInteractionBlock ** work;
        prod(*leftInteractionBlock, *rightInteractionBlock, *currentInteractionBlock, false);
#ifdef MLCPPROJ_DEBUG
        std::cout << "MLCPProjectOnConstraints::computeInteractionBlock : currentInteractionBlock" << std::endl;
        currentInteractionBlock->display();
#endif
      }

      else
        RuntimeException::selfThrow("MLCPProjectOnConstraints::computeInteractionBlock not yet implemented for relation of type " + relationType1);

    }

    void MLCPProjectOnConstraints::computeqBlock(SP::Interaction inter, unsigned int pos)
    {

      assert(inter);
      unsigned int sizeY = boost::static_pointer_cast<OSNSMatrixProjectOnConstraints>
                           (_M)->computeSizeForProjection(inter);
      for (unsigned int i = 0; i < sizeY; i++)
        _q->setValue(pos + i, inter->y(0)->getValue(0 + i));
#ifdef MLCPPROJ_DEBUG
      printf("MLCPProjectOnConstraints::computeqBlock, _q from y(0)\n");
      _q->display();
#endif
    }

    void MLCPProjectOnConstraints::postCompute()
    {
      _hasBeUpdated = true;
      // This function is used to set y/lambda values using output from
      // lcp_driver (w,z).  Only Interactions (ie Interactions) of
      // indexSet(leveMin) are concerned.

      // === Get index set from Topology ===
      SP::InteractionsGraph indexSet = simulation()->indexSet(levelMin());

      // y and lambda vectors
      SP::SiconosVector lambda;
      SP::SiconosVector y;

      // === Loop through "active" Interactions (ie present in
      // indexSets[1]) ===
      /** We chose to do a small step _alpha in view of stabilized the algorithm.*/
#ifdef MLCPPROJ_DEBUG
      printf("MLCPProjectOnConstraints::postCompute damping value = %f\n", _alpha);
#endif
      (*_z) *= _alpha;
      unsigned int pos = 0;
#ifdef MLCPPROJ_DEBUG
      printf("MLCPProjectOnConstraints::postCompute _z\n");
      _z->display();
      display();
#endif



      InteractionsGraph::VIterator ui, uiend;
#ifdef MLCPPROJ_DEBUG
      printf("MLCPProjectOnConstraints::postCompute BEFORE UPDATE:\n");
      for (boost::tie(ui, uiend) = indexSet->vertices(); ui != uiend; ++ui)
      {
        SP::Interaction inter = indexSet->bundle(*ui);
        RELATION::TYPES relationType = inter->getRelationType();
        if (relationType == NewtonEuler)
        {
          SP::Relation R = inter->relation();
          SP::NewtonEulerR ner = (boost::static_pointer_cast<NewtonEulerR>(R));
          ner->computeh(0);
          ner->getq()->display();
        }
        else if (relationType == Lagrangian)
        {
          inter->relation()->computeOutput(0.0, 0);
          for (DSIterator it = inter->dynamicalSystemsBegin();
               it != inter->dynamicalSystemsEnd();
               ++it)
          {
            SP::LagrangianDS lds =  boost::static_pointer_cast<LagrangianDS>(*it);
            lds->q()->display();
          }
        }
        else
        {
          RuntimeException::selfThrow("MLCPProjectOnConstraints::postCompute - relation type is not from Lagrangian type neither NewtonEuler.");
        }
      }
#endif


      for (boost::tie(ui, uiend) = indexSet->vertices(); ui != uiend; ++ui)
      {

        SP::Interaction inter = indexSet->bundle(*ui);
        // Get the relative position of inter-interactionBlock in the vector w
        // or z
        pos = _M->getPositionOfInteractionBlock(inter);
        RELATION::TYPES relationType = inter->getRelationType();
        if (relationType == NewtonEuler)
        {
          postComputeNewtonEulerR(inter, pos);
        }
        else if (relationType == Lagrangian)
        {
          postComputeLagrangianR(inter, pos);

        }
        else
        {
          RuntimeException::selfThrow("MLCPProjectOnConstraints::computeInteractionBlock - relation type is not from Lagrangian type neither NewtonEuler.");
        }

      }
#ifdef MLCPPROJ_DEBUG
      printf("MLCPProjectOnConstraints::postCompute AFTER UPDATE:\n");
      for (boost::tie(ui, uiend) = indexSet->vertices(); ui != uiend; ++ui)
      {
        SP::Interaction inter = indexSet->bundle(*ui);
        RELATION::TYPES relationType = inter->getRelationType();
        if (relationType == NewtonEuler)
        {
          SP::Relation R = inter->relation();
          SP::NewtonEulerR ner = (boost::static_pointer_cast<NewtonEulerR>(R));
          ner->computeh(0);
          ner->getq()->display();
        }
        else if (relationType == Lagrangian)
        {
          inter->relation()->computeOutput(0.0, 0);
          for (DSIterator it = inter->dynamicalSystemsBegin();
               it != inter->dynamicalSystemsEnd();
               ++it)
          {
            SP::LagrangianDS lds =  boost::static_pointer_cast<LagrangianDS>(*it);
            lds->q()->display();
          }
        }
        else
        {
          RuntimeException::selfThrow("MLCPProjectOnConstraints::postCompute - relation type is not from Lagrangian type neither NewtonEuler.");
        }
      }
#endif


    }

    void MLCPProjectOnConstraints::postComputeLagrangianR(SP::Interaction inter, unsigned int pos)
    {

      SP::LagrangianR  lr = boost::static_pointer_cast<LagrangianR>(inter->relation());
#ifdef MLCPPROJ_DEBUG
      printf("MLCPProjectOnConstraints::postComputeLagrangian lr->jachq \n");
      lr->jachq()->display();
#endif

#ifdef MLCPPROJ_DEBUG
      printf("MLCPProjectOnConstraints::postComputeLagrangianR q before update\n");
      for (DSIterator it = inter->dynamicalSystemsBegin();
           it != inter->dynamicalSystemsEnd();
           ++it)
      {
        SP::LagrangianDS lds =  boost::static_pointer_cast<LagrangianDS>(*it);
        lds->q()->display();
      }
#endif
      unsigned int sizeY = inter->nonSmoothLaw()->size();
      SP::SimpleVector aBuff(new SimpleVector(sizeY));
      setBlock(*_z, aBuff, sizeY, pos, 0);
#ifdef MLCPPROJ_DEBUG
      printf("MLCPP lambda of Interaction is pos =%i :", pos);
      aBuff->display();
#endif



      // aBuff should nornally be in lambda[corectlevel]
      // The update of the position in DS should be made in Moreau::upateState or ProjectedMoreau::updateState
      SP::SiconosMatrix J = lr->jachq();
      SP::SimpleMatrix aux(new SimpleMatrix(*J));
      aux->trans();
      prod(*aux, *aBuff, *(lr->q()), false);
#ifdef MLCPPROJ_DEBUG
      printf("MLCPProjectOnConstraints::postComputeLagrangianR _z\n");
      _z->display();
      printf("MLCPProjectOnConstraints::postComputeLagrangianR updated\n");
      (lr->q())->display();
#endif



      //RuntimeException::selfThrow("MLCPProjectOnConstraints::postComputeLagrangianR() - not yet implemented");
    }
    void MLCPProjectOnConstraints::postComputeNewtonEulerR(SP::Interaction inter, unsigned int pos)
    {
      // Get Y and Lambda for the current Interaction
      //y = inter->y(levelMin());
      //lambda = inter->lambda(levelMin());
      // Copy _w/_z values, starting from index pos into y/lambda.

      //      setBlock(*_w, y, y->size(), pos, 0);// Warning: yEquivalent is
      // saved in y !!
      //setBlock(*_z, lambda, lambda->size(), pos, 0);

      unsigned int sizeY = boost::static_pointer_cast<OSNSMatrixProjectOnConstraints>
                           (_M)->computeSizeForProjection(inter);

      SP::NewtonEulerR ner = (boost::static_pointer_cast<NewtonEulerR>(inter->relation()));
#ifdef MLCPPROJ_DEBUG
      printf("MLCPProjectOnConstraints::postCompute ner->jachq \n");
      ner->jachq()->display();
      ner->jachqT()->display();
#endif

#ifdef MLCPPROJ_DEBUG
      printf("MLCPProjectOnConstraints::postCompute q before update\n");
      (ner->getq())->display();
#endif


      SP::SimpleVector aBuff(new SimpleVector(sizeY));
      setBlock(*_z, aBuff, sizeY, pos, 0);
      /*Now, update the ds's dof throw the relation*/
      //proj_with_q SP::SiconosMatrix J=ner->jachqProj();
#ifdef MLCPPROJ_DEBUG
      printf("MLCPP lambda of Interaction is pos =%i :", pos);
      aBuff->display();
#endif

      DSIterator itDS;
      Index coord(8);
#ifdef MLCPPROJ_WITH_CT
      unsigned int k = 0;
      unsigned int NumDS = 0;
      SP::SiconosMatrix J = ner->jachqT();
      //printf("J\n");
      //J->display();

      SP::SimpleMatrix aux(new SimpleMatrix(*J));
      aux->trans();

      itDS = inter->dynamicalSystemsBegin();
      while (itDS != inter->dynamicalSystemsEnd())
      {
        SP::NewtonEulerDS neds = (boost::static_pointer_cast<NewtonEulerDS>(*itDS));
        k += neds->getqDim();
        itDS++;
      }
      SP::SimpleVector deltaqG(new SimpleVector(k));
      // prod(*aux,*aBuff,*vel,true);
      k = 0;
      deltaqG->zero();

      itDS = inter->dynamicalSystemsBegin();
      while (itDS != inter->dynamicalSystemsEnd())
      {
        Type::Siconos dsType = Type::value(**itDS);
        if (dsType != Type::NewtonEulerDS)
          RuntimeException::selfThrow("MLCPProjectOnConstraint::postCompute- ds is not from NewtonEulerDS.");
        SP::NewtonEulerDS neds = (boost::static_pointer_cast<NewtonEulerDS>(*itDS));

        SP::SimpleVector vel(new SimpleVector(neds->getDim()));
        SP::SimpleVector deltaQ(new SimpleVector(neds->getqDim()));
        coord[0] = k;
        coord[1] = k + neds->getDim();
        coord[2] = 0;
        coord[3] = aux->size(1);
        coord[4] = 0;
        coord[5] = aux->size(1);
        coord[6] = 0;
        coord[7] = neds->getDim();
        subprod(*aux   , *aBuff, *vel, coord, true);
        SP::SimpleMatrix T = neds->T();
        SP::SimpleMatrix workT(new SimpleMatrix(*T));
        workT->trans();
        SP::SimpleMatrix workT2(new SimpleMatrix(6, 6));
        prod(*workT, *T, *workT2, true);

        workT2->PLUForwardBackwardInPlace(*vel);
        prod(*T, *vel, *deltaQ);

        for (unsigned int ii = 0; ii < neds->getqDim(); ii++)
        {
          ner->getq()->setValue(k + ii,
                                deltaQ->getValue(ii) +
                                ner->getq()->getValue(k + ii));
          deltaqG->setValue(k + ii, deltaQ->getValue(ii) + deltaqG->getValue(k + ii));
        }

        k += neds->getDim();
        itDS++;
#ifdef MLCPPROJ_DEBUG
        printf("MLCPProjectOnConstraints::postCompute ds %d, q updated\n", NumDS);
        (ner->getq())->display();
#endif
        NumDS++;

      }
      printf("MLCPProjectOnConstraints deltaqG:\n");
      deltaqG->display();
#else

      SP::SiconosMatrix J = ner->jachq();
      SP::SimpleMatrix aux(new SimpleMatrix(*J));
      aux->trans();
      prod(*aux, *aBuff, *(ner->getq()), false);
#endif


#ifdef MLCPPROJ_DEBUG
      printf("MLCPProjectOnConstraints::postComputeNewtonR _z\n");
      _z->display();
      printf("MLCPProjectOnConstraints::postComputeNewtonR q updated\n");
      (ner->getq())->display();
#endif
    }

    void MLCPProjectOnConstraints::computeOptions(SP::Interaction inter1, SP::Interaction inter2)
    {
      //  printf("MLCPProjectOnConstraints::computeOptions\n");
      // Get dimension of the NonSmoothLaw (ie dim of the interactionBlock)
      RELATION::TYPES relationType1;
      relationType1 = inter1->getRelationType();
      // Retrieve size of Y (projected variable)
      unsigned int sizeY1;
      sizeY1 = boost::static_pointer_cast<OSNSMatrixProjectOnConstraints>
               (_M)->computeSizeForProjection(inter1);

      // Compute the number of equalities
      unsigned int equalitySize1 =  sizeY1; //default behavior

      if (Type::value(*(inter1->nonSmoothLaw())) == Type::NewtonImpactFrictionNSL ||
          Type::value(*(inter1->nonSmoothLaw())) == Type::NewtonImpactNSL)
      {
        equalitySize1 = 0;
      }
      else if (Type::value(*(inter1->nonSmoothLaw()))
               == Type::MixedComplementarityConditionNSL)
      {
        equalitySize1 =
          MixedComplementarityConditionNSL::convert(inter1->nonSmoothLaw())->getEqualitySize();
      }

      // Compute the number of inequalities
      unsigned int inequalitySize1 =  sizeY1 - equalitySize1;



      if (inter1 == inter2)
      {
        //inter1->getExtraInteractionBlock(currentInteractionBlock);
        _m += inequalitySize1;
        _n += equalitySize1;
        //    _m=0;
        //_n=6;
        if (_curBlock > MLCP_NB_BLOCKS - 2)
          printf("MLCP.cpp : number of block to small, memory crach below!!!\n");
        /*add an equality block.*/

        // #ifdef MLCPPROJ_DEBUG
        //   printf("MLCPProjectOnConstraints::computeOptions()\n");
        // #endif

        if (equalitySize1 > 0)
        {
          _numerics_problem.blocksLine[_curBlock + 1] = _numerics_problem.blocksLine[_curBlock] + equalitySize1;
          _numerics_problem.blocksIsComp[_curBlock] = 0;
          // #ifdef MLCPPROJ_DEBUG
          //       std::cout << "_curBlock : " << _curBlock <<std::endl;
          //       std::cout << "_numerics_problem.blocksLine["<<_curBlock+1 <<" ] : " << _numerics_problem.blocksLine[_curBlock+1] <<std::endl;
          //       std::cout << "_numerics_problem.blocksIsComp["<<_curBlock <<" ] : " << _numerics_problem.blocksIsComp[_curBlock] <<std::endl;
          // #endif

          _curBlock++;
        }
        /*add a complementarity block.*/
        if (inequalitySize1 > 0)
        {
          _numerics_problem.blocksLine[_curBlock + 1] = _numerics_problem.blocksLine[_curBlock] + inequalitySize1;
          _numerics_problem.blocksIsComp[_curBlock] = 1;
          // #ifdef MLCPPROJ_DEBUG
          //       std::cout << "_curBlock : " << _curBlock <<std::endl;
          //       std::cout << "_numerics_problem.blocksLine["<<_curBlock+1<< "] : " << _numerics_problem.blocksLine[_curBlock+1] <<std::endl;
          //       std::cout << "_numerics_problem.blocksIsComp["<<_curBlock<< "] : " << _numerics_problem.blocksIsComp[_curBlock] <<std::endl;
          // #endif

          _curBlock++;

        }
      }
      // #ifdef MLCPPROJ_DEBUG
      //   std::cout << "_m : " << _m <<std::endl;
      //   std::cout << "_n : " << _n <<std::endl;
      // #endif
    }
