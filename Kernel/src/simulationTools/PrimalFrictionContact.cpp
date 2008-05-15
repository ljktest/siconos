/* Siconos-Kernel version 3.0.0, Copyright INRIA 2005-2008.
 * Siconos is a program dedicated to modeling, simulation and control
 * of non smooth dynamical systems.
 * Siconos is a free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * Siconos is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY ory FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Siconos; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Contact: Vincent ACARY vincent.acary@inrialpes.fr
 */
#include "PrimalFrictionContact.h"
#include "PrimalFrictionContactXML.h"
#include "OneStepIntegrator.h"
#include "Topology.h"
#include "Interaction.h"
#include "Simulation.h"
#include "UnitaryRelation.h"
#include "Model.h"
#include "NonSmoothDynamicalSystem.h"
#include "DynamicalSystem.h"
#include "Relation.h"
#include "NewtonImpactFrictionNSL.h"
#include "NewtonImpactFrictionNSL.h"
#include "Moreau.h" // Numerics Header
#include "LagrangianDS.h"

using namespace std;

// xml constructor
PrimalFrictionContact::PrimalFrictionContact(OneStepNSProblemXML* osNsPbXml, Simulation* newSimu):
  OneStepNSProblem("PrimalFrictionContact", osNsPbXml, newSimu), contactProblemDim(3), velocity(NULL), reaction(NULL), localVelocity(NULL), localReaction(NULL), tildeLocalVelocity(NULL), M(NULL), H(NULL), q(NULL), mu(NULL),
  isVelocityAllocatedIn(false), isReactionAllocatedIn(false), isLocalVelocityAllocatedIn(false), isLocalReactionAllocatedIn(false), isTildeLocalVelocityAllocatedIn(false), isMAllocatedIn(false), isHAllocatedIn(false), isQAllocatedIn(false), MStorageType(0)
{
  PrimalFrictionContactXML * xmlFC = (static_cast<PrimalFrictionContactXML*>(osNsPbXml));

  // Read dimension of the problem (required parameter)
  if (!xmlFC->hasProblemDim())
    RuntimeException::selfThrow("PrimalFrictionContact: xml constructor failed, attribute for dimension of the problem (2D or 3D) is missing.");

  contactProblemDim = xmlFC->getProblemDim();

  // Read storage type if given (optional , default = dense)
  if (onestepnspbxml->hasStorageType())
    MStorageType = onestepnspbxml->getStorageType();

}

// Constructor from a set of data
// Required input: simulation
// Optional: NonSmoothSolver and id
PrimalFrictionContact::PrimalFrictionContact(Simulation* newSimu, int dimPb, NonSmoothSolver*  newSolver, const string& newId):
  OneStepNSProblem("PrimalFrictionContact", newSimu, newId, newSolver), contactProblemDim(dimPb), velocity(NULL), reaction(NULL), localVelocity(NULL), localReaction(NULL), tildeLocalVelocity(NULL), M(NULL), H(NULL), q(NULL), mu(NULL),
  isVelocityAllocatedIn(false), isReactionAllocatedIn(false), isLocalVelocityAllocatedIn(false), isLocalReactionAllocatedIn(false), isTildeLocalVelocityAllocatedIn(false), isMAllocatedIn(false), isHAllocatedIn(false), isQAllocatedIn(false), MStorageType(0)
{}

// destructor
PrimalFrictionContact::~PrimalFrictionContact()
{
  if (isVelocityAllocatedIn) delete velocity;
  velocity = NULL;
  if (isReactionAllocatedIn) delete reaction;
  reaction = NULL;
  if (isLocalVelocityAllocatedIn) delete localVelocity;
  localVelocity = NULL;
  if (isLocalReactionAllocatedIn) delete localReaction;
  localReaction = NULL;
  if (isTildeLocalVelocityAllocatedIn) delete tildeLocalVelocity;
  tildeLocalVelocity = NULL;
  if (isMAllocatedIn)
    delete M;
  M = NULL;
  if (isHAllocatedIn)
    delete H;
  H = NULL;
  if (isQAllocatedIn) delete q;
  q = NULL;
  if (mu != NULL) delete mu;
  mu = NULL;
}

// Setters

void PrimalFrictionContact::setVelocity(const SimpleVector& newValue)
{
  if (sizeOutput != newValue.size())
    RuntimeException::selfThrow("PrimalFrictionContact: setVelocity, inconsistent size between given velocity size and problem size. You should set sizeOutput before");

  if (velocity == NULL)
  {
    velocity = new SimpleVector(sizeOutput);
    isVelocityAllocatedIn = true;
  }
  else if (velocity->size() != sizeOutput)
    RuntimeException::selfThrow("PrimalFrictionContact: setVelocity, velocity size differs from sizeOutput");

  *velocity = newValue;
}

void PrimalFrictionContact::setVelocityPtr(SimpleVector* newPtr)
{
  if (sizeOutput != newPtr->size())
    RuntimeException::selfThrow("PrimalFrictionContact: setVelocityPtr, inconsistent size between given velocity size and problem size. You should set sizeOutput before");

  if (isVelocityAllocatedIn) delete velocity;
  velocity = newPtr;
  isVelocityAllocatedIn = false;
}


void PrimalFrictionContact::setReaction(const SimpleVector& newValue)
{
  if (sizeOutput != newValue.size())
    RuntimeException::selfThrow("PrimalFrictionContact: setReaction, inconsistent size between given reaction size and problem size. You should set sizeOutput before");

  if (reaction == NULL)
  {
    reaction = new SimpleVector(sizeOutput);
    isReactionAllocatedIn = true;
  }

  *reaction = newValue;
}

void PrimalFrictionContact::setReactionPtr(SimpleVector* newPtr)
{
  if (sizeOutput != newPtr->size())
    RuntimeException::selfThrow("PrimalFrictionContact: setReactionPtr, inconsistent size between given reaction size and problem size. You should set sizeOutput before");

  if (isReactionAllocatedIn) delete reaction;
  reaction = newPtr;
  isReactionAllocatedIn = false;
}

void PrimalFrictionContact::setLocalVelocity(const SimpleVector& newValue)
{
  if (sizeLocalOutput != newValue.size())
    RuntimeException::selfThrow("PrimalFrictionContact: setLocalVelocity, inconsistent size between given velocity size and problem size. You should set sizeOutput before");

  if (localVelocity == NULL)
  {
    localVelocity = new SimpleVector(sizeLocalOutput);
    isLocalVelocityAllocatedIn = true;
  }
  else if (localVelocity->size() != sizeLocalOutput)
    RuntimeException::selfThrow("PrimalFrictionContact: setLocalVelocity, velocity size differs from sizeLocalOutput");

  *localVelocity = newValue;
}

void PrimalFrictionContact::setLocalVelocityPtr(SimpleVector* newPtr)
{
  if (sizeLocalOutput != newPtr->size())
    RuntimeException::selfThrow("PrimalFrictionContact: setLocalVelocityPtr, inconsistent size between given velocity size and problem size. You should set sizeLocalOutput before");

  if (isVelocityAllocatedIn) delete localVelocity;
  localVelocity = newPtr;
  isLocalVelocityAllocatedIn = false;
}


void PrimalFrictionContact::setLocalReaction(const SimpleVector& newValue)
{
  if (sizeLocalOutput != newValue.size())
    RuntimeException::selfThrow("PrimalFrictionContact: setLocalReaction, inconsistent size between given reaction size and problem size. You should set sizeLocalOutput before");

  if (localReaction == NULL)
  {
    localReaction = new SimpleVector(sizeLocalOutput);
    isLocalReactionAllocatedIn = true;
  }

  *localReaction = newValue;
}

void PrimalFrictionContact::setLocalReactionPtr(SimpleVector* newPtr)
{
  if (sizeLocalOutput != newPtr->size())
    RuntimeException::selfThrow("PrimalFrictionContact: setLocalReactionPtr, inconsistent size between given reaction size and problem size. You should set sizeLocalOutput before");

  if (isLocalReactionAllocatedIn) delete localReaction;
  localReaction = newPtr;
  isLocalReactionAllocatedIn = false;
}
void PrimalFrictionContact::setTildeLocalVelocity(const SimpleVector& newValue)
{
  if (sizeLocalOutput != newValue.size())
    RuntimeException::selfThrow("PrimalFrictionContact: setTildeLocalVelocity, inconsistent size between given velocity size and problem size. You should set sizeLocalOutput before");

  if (tildeLocalVelocity == NULL)
  {
    tildeLocalVelocity = new SimpleVector(sizeLocalOutput);
    isTildeLocalVelocityAllocatedIn = true;
  }
  else if (tildeLocalVelocity->size() != sizeLocalOutput)
    RuntimeException::selfThrow("PrimalFrictionContact: setTildeLocalVelocity, velocity size differs from sizeLocalOutput");

  *tildeLocalVelocity = newValue;
}

void PrimalFrictionContact::setTildeLocalVelocityPtr(SimpleVector* newPtr)
{
  if (sizeLocalOutput != newPtr->size())
    RuntimeException::selfThrow("PrimalFrictionContact: setTildeLocalVelocityPtr, inconsistent size between given velocity size and problem size. You should set sizeLocalOutput before");

  if (isTildeLocalVelocityAllocatedIn) delete tildeLocalVelocity;
  tildeLocalVelocity = newPtr;
  isTildeLocalVelocityAllocatedIn = false;
}


void PrimalFrictionContact::setM(const OSNSMatrix& newValue)
{
  // Useless ?
  RuntimeException::selfThrow("PrimalFrictionContact::setM, forbidden operation. Try setMPtr().");
}

void PrimalFrictionContact::setMPtr(OSNSMatrix* newPtr)
{
  // Note we do not test if newPtr and M sizes are equal. Not necessary?
  if (isMAllocatedIn)
    delete M;
  M = newPtr;
  isMAllocatedIn = false;
}

void PrimalFrictionContact::setH(const OSNSMatrix& newValue)
{
  // Useless ?
  RuntimeException::selfThrow("PrimalFrictionContact::setH, forbidden operation. Try setMPtr().");
}

void PrimalFrictionContact::setHPtr(OSNSMatrix* newPtr)
{
  // Note we do not test if newPtr and M sizes are equal. Not necessary?
  if (isHAllocatedIn)
    delete H;
  H = newPtr;
  isHAllocatedIn = false;
}

void PrimalFrictionContact::setQ(const SimpleVector& newValue)
{
  if (sizeOutput != newValue.size())
    RuntimeException::selfThrow("PrimalFrictionContact: setQ, inconsistent size between given q size and problem size. You should set sizeOutput before");

  if (q == NULL)
  {
    q = new SimpleVector(sizeOutput);
    isQAllocatedIn = true;
  }

  *q = newValue;
}

void PrimalFrictionContact::setQPtr(SimpleVector* newPtr)
{
  if (sizeOutput != newPtr->size())
    RuntimeException::selfThrow("PrimalFrictionContact: setQPtr, inconsistent size between given q size and problem size. You should set sizeOutput before");

  if (isQAllocatedIn) delete q;
  q = newPtr;
  isQAllocatedIn = false;
}

void PrimalFrictionContact::initialize()
{
  // - Checks memory allocation for main variables (M,q,w,z)
  // - Formalizes the problem if the topology is time-invariant

  // This function performs all steps that are time-invariant

  // General initialize for OneStepNSProblem
  OneStepNSProblem::initialize();

  updateDSBlocks(); //blocks of M
  // updateUnitaryDSBlocks(); This computation is not made because, we that UnitaryDSBlocks =H^T
  updateUnitaryDSBlocks(); // blocks of H




  // Connect to the right function according to dim. of the problem
  if (contactProblemDim == 2)
    ;
  //    primalFrictionContact_driver = &pfc_2D_driver;
  else // if(contactProblemDim == 3)
    //primalFrictionContact_driver = &frictionContact3D_driver;
    ;

  // Memory allocation for reaction, and velocity
  // If one of them has already been allocated, nothing is done.
  // We suppose that user has chosen a correct size.
  if (velocity == NULL)
  {
    velocity = new SimpleVector(maxSize);
    isVelocityAllocatedIn = true;
  }
  else
  {
    if (velocity->size() != maxSize)
      velocity->resize(maxSize);
  }

  if (reaction == NULL)
  {
    reaction = new SimpleVector(maxSize);
    isReactionAllocatedIn = true;
  }
  else
  {
    if (reaction->size() != maxSize)
      reaction->resize(maxSize);
  }

  if (localVelocity == NULL)
  {
    localVelocity = new SimpleVector(maxSize);
    isLocalVelocityAllocatedIn = true;
  }
  else
  {
    if (localVelocity->size() != maxSize)
      localVelocity->resize(maxSize);
  }

  if (localReaction == NULL)
  {
    localReaction = new SimpleVector(maxSize);
    isLocalReactionAllocatedIn = true;
  }
  else
  {
    if (reaction->size() != maxSize)
      reaction->resize(maxSize);
  }
  if (tildeLocalVelocity == NULL)
  {
    tildeLocalVelocity = new SimpleVector(maxSize);
    isTildeLocalVelocityAllocatedIn = true;
  }
  else
  {
    if (tildeLocalVelocity->size() != maxSize)
      tildeLocalVelocity->resize(maxSize);
  }
  if (q == NULL)
  {
    q = new SimpleVector(maxSize);
    isQAllocatedIn = true;
  }
  else
  {
    if (q->size() != maxSize)
      q->resize(maxSize);
  }

  // get topology
  Topology * topology = simulation->getModelPtr()->getNonSmoothDynamicalSystemPtr()->getTopologyPtr();

  // Note that unitaryBlocks is up to date since updateUnitaryBlocks has been called during OneStepNSProblem::initialize()

  // Fill vector of friction coefficients
  UnitaryRelationsSet* I0 = topology->getIndexSet0Ptr();
  mu = new std::vector<double>();
  mu->reserve(I0->size());


  DynamicalSystemsSet *allDS = simulation->getModelPtr()->getNonSmoothDynamicalSystemPtr()->getDynamicalSystems();;


  // If the topology is TimeInvariant ie if M structure does not change during simulation:
  if (topology->isTimeInvariant() &&   !OSNSInteractions->isEmpty())
  {

    if (M == NULL)
    {
      // Creates and fills M using UR of indexSet
      M = new OSNSMatrix(allDS, DSBlocks, MStorageType);
      isMAllocatedIn = true;
    }
    else
    {
      M->setStorageType(MStorageType);
      M->fill(allDS, DSBlocks);
    }
    // Get index set from Simulation
    UnitaryRelationsSet * indexSet = simulation->getIndexSetPtr(levelMin);
    if (H == NULL)
    {
      // Creates and fills M using UR of indexSet
      H = new OSNSMatrix(indexSet, allDS, unitaryDSBlocks, MStorageType);
      isHAllocatedIn = true;
    }
    else
    {
      H->setStorageType(MStorageType);
      H->fill(indexSet, allDS, unitaryDSBlocks);
    }




    for (ConstUnitaryRelationsIterator itUR = indexSet->begin(); itUR != indexSet->end(); ++itUR)
    {

#ifndef WithSmartPtr
      mu->push_back(static_cast<NewtonImpactFrictionNSL*>((*itUR)->getInteractionPtr()->getNonSmoothLawPtr())->getMu());
#else
      mu->push_back(boost::static_pointer_cast<NewtonImpactFrictionNSL> ((*itUR)->getInteractionPtr()->getNonSmoothLawPtr())->getMu());
#endif

    }
  }
  else // in that case, M and mu will be updated during preCompute
  {
    // Default size for M = maxSize
    if (M == NULL)
    {
      if (MStorageType == 0)
        M = new OSNSMatrix(maxSize, 0);
      else // if(MStorageType == 1) size = number of DSBlocks = number of DS in the largest considered DynamicalSystemsSet
        M = new OSNSMatrix(simulation->getModelPtr()->getNonSmoothDynamicalSystemPtr()->getDynamicalSystems()->size(), 1);
      isMAllocatedIn = true;
    }
    if (H == NULL)
    {
      if (MStorageType == 0)
        H = new OSNSMatrix(maxSize, 0);
      else // if(MStorageType == 1) size = number of DSBlocks = number of DS in the largest considered DynamicalSystemsSet


        H = new OSNSMatrix(simulation->getModelPtr()->getNonSmoothDynamicalSystemPtr()->getDynamicalSystems()->size(), simulation->getIndexSetPtr(levelMin)->size()   , 1);
      isMAllocatedIn = true;
    }
  }
}

void PrimalFrictionContact::computeUnitaryBlock(UnitaryRelation* UR1, UnitaryRelation* UR2)
{
  // Computes matrix unitaryBlocks[UR1][UR2] (and allocates memory if necessary) if UR1 and UR2 have commond DynamicalSystem.
  // How unitaryBlocks are computed depends explicitely on the type of Relation of each UR.

}

void PrimalFrictionContact::computeDSBlock(DynamicalSystem* DS)
{
  // Computes matrix DSBlocks[DS1](and allocates memory if necessary)

  DSIterator itDS;
  OneStepIntegrator * Osi;
  string osiType; // type of the current one step integrator
  string dsType; // type of the current Dynamical System;

  Osi = simulation->getIntegratorOfDSPtr(DS); // get OneStepIntegrator of current dynamical system
  osiType = Osi->getType();
  if (osiType == "Moreau" || osiType == "Moreau2")
  {
    DSBlocks[DS] = (static_cast<Moreau*>(Osi))->getWPtr(DS);  // get its W matrix ( pointer link!)
    //       cout << "PrimalFrictionContact::computeDSBlock(DynamicalSystem* DS) " << endl;
    //       DSBlocks[DS]->display();
  }
  else if (osiType == "Lsodar") // Warning: LagrangianDS only at the time !!!
  {
    RuntimeException::selfThrow("PrimalFrictionContact::computeDSBlocks. Not yet implemented for Lsodar Integrator");
  }
  else
    RuntimeException::selfThrow("PrimalFrictionContact::computeDSBlocks. nNot yet implemented for Integrator of type " + osiType);
}

/** computes  DSUnitaryBlock-matrix that corresponds to UR1 and DS2
 *  Move this to Unitary Relation class?
 *  \param a pointer to UnitaryRelation UR1
 *  \param a pointer to DynamicalSystems DS2
 */
void PrimalFrictionContact::computeUnitaryDSBlock(UnitaryRelation* UR, DynamicalSystem* DS)
{
  unsigned int sizeDS = (DS)->getDim();
  unsigned int nslawSize = UR->getNonSmoothLawSize();
  string relationType = UR->getRelationType();

  if (relationType == "Lagrangian")
  {
    unitaryDSBlocks[UR][DS] = new SimpleMatrix(nslawSize, sizeDS);
    UR->getLeftUnitaryBlockForDS(DS, unitaryDSBlocks[UR][DS]);
  }
  else RuntimeException::selfThrow("PrimalFrictionContact::computeUnitaryDSBlock not yet implemented for relation of type " + relationType);
}

void PrimalFrictionContact::computeQ(const double time)
{
  if (q->size() != sizeOutput)
    q->resize(sizeOutput);
  q->zero();

  DynamicalSystemsSet *allDS = simulation->getModelPtr()->getNonSmoothDynamicalSystemPtr()->getDynamicalSystems();;
  DSIterator itDS;
  unsigned int sizeDS;
  OneStepIntegrator * Osi;
  string osiType; // type of the current one step integrator
  string dsType; // type of the current Dynamical System
  unsigned int pos = 0;
  for (itDS = allDS->begin(); itDS !=  allDS->end(); ++itDS)
  {
    pos = M->getPositionOfDSBlock(*itDS);
    Osi = simulation->getIntegratorOfDSPtr(*itDS);
    osiType = Osi->getType();
    if (osiType == "Moreau2")
    {
      sizeDS = (*itDS)->getDim();
      setBlock(Osi->getWorkX(*itDS)  , q, sizeDS, 0, pos);
    }
    else
    {
      RuntimeException::selfThrow("PrimalFrictionContact::computeQ. Not yet implemented for Integrator type : " + osiType);
    }
  }
}

void PrimalFrictionContact::computeTildeLocalVelocity(const double time)
{
  if (tildeLocalVelocity->size() != sizeLocalOutput)
    tildeLocalVelocity->resize(sizeLocalOutput);
  tildeLocalVelocity->zero();
  // === Get index set from Simulation ===
  UnitaryRelationsSet * indexSet = simulation->getIndexSetPtr(levelMin);
  // === Loop through "active" Unitary Relations (ie present in indexSets[level]) ===

  unsigned int pos = 0;
  UnitaryRelationsIterator itCurrent, itLiked;
  string simulationType = simulation->getType();
  for (itCurrent = indexSet->begin(); itCurrent !=  indexSet->end(); ++itCurrent)
  {
    // *itCurrent is a UnitaryRelation*.
    // Compute q, this depends on the type of non smooth problem, on the relation type and on the non smooth law
    pos = H->getPositionOfUnitaryBlock(*itCurrent);

    (*itCurrent)->computeEquivalentY(time, levelMin, simulationType, tildeLocalVelocity, pos); // free output is saved in y
  }

}

void PrimalFrictionContact::preCompute(const double time)
{
  // This function is used to prepare data for the PrimalFrictionContact problem
  // - computation of M, H tildeLocalVelocity and q
  // - set sizeOutput, sizeLocalOutput

  // If the topology is time-invariant, only q needs to be computed at each time step.
  // M, sizeOutput have been computed in initialize and are uptodate.

  // Get topology
  Topology * topology = simulation->getModelPtr()->getNonSmoothDynamicalSystemPtr()->getTopologyPtr();
  DynamicalSystemsSet *allDS = simulation->getModelPtr()->getNonSmoothDynamicalSystemPtr()->getDynamicalSystems();;
  UnitaryRelationsSet * indexSet = simulation->getIndexSetPtr(levelMin);


  if (!topology->isTimeInvariant())
  {
    // Computes new unitaryBlocks if required
    updateUnitaryBlocks();
    updateDSBlocks(); //blocks of M
    // updateUnitaryDSBlocks(); This computation is not made because, we that UnitaryDSBlocks =H^T
    updateUnitaryDSBlocks(); // blocks of H



    // Updates matrix M
    UnitaryRelationsSet * indexSet = simulation->getIndexSetPtr(levelMin);
    M->fill(allDS, DSBlocks);
    H->fill(indexSet, allDS, unitaryDSBlocks);
    sizeOutput = M->size();
    sizeLocalOutput = H->size();

    // Checks z and w sizes and reset if necessary
    if (reaction->size() != sizeOutput)
    {
      reaction->resize(sizeOutput, false);
      reaction->zero();
    }

    if (velocity->size() != sizeOutput)
    {
      velocity->resize(sizeOutput);
      velocity->zero();
    }
    // Checks z and w sizes and reset if necessary
    if (localReaction->size() != sizeLocalOutput)
    {
      localReaction->resize(sizeLocalOutput, false);
      localReaction->zero();
    }

    if (localVelocity->size() != sizeLocalOutput)
    {
      localVelocity->resize(sizeLocalOutput);
      localVelocity->zero();
    }

    // Update mu
    mu->clear();
    for (ConstUnitaryRelationsIterator itUR = indexSet->begin(); itUR != indexSet->end(); ++itUR)
    {

#ifndef WithSmartPtr
      mu->push_back(static_cast<NewtonImpactFrictionNSL*>((*itUR)->getInteractionPtr()->getNonSmoothLawPtr())->getMu());
#else
      mu->push_back(boost::static_pointer_cast<NewtonImpactFrictionNSL>((*itUR)->getInteractionPtr()->getNonSmoothLawPtr())->getMu());
#endif
    }

  }

  // Computes q
  computeQ(time);
  computeTildeLocalVelocity(time);

}

int PrimalFrictionContact::compute(double time)
{
  int info = 0;
  // --- Prepare data for PrimalFrictionContact computing ---
  preCompute(time);

  // --- Call Numerics solver ---
  if (sizeOutput != 0)
  {
    // The PrimalFrictionContact Problem in Numerics format
    PrimalFrictionContact_Problem numerics_problem;
    numerics_problem.M = M->getNumericsMatrix();
    numerics_problem.q = q->getArray();
    numerics_problem.numberOfContacts = sizeOutput / contactProblemDim;
    numerics_problem.isComplete = 1;
    numerics_problem.mu = &((*mu)[0]);
    // Call Numerics Driver for PrimalFrictionContact
    //  {
    //    int info2 = -1;
    //    int iparam2[7];
    //    iparam2[0] = 100000;
    //    iparam2[1] = 1;
    //    iparam2[3] = iparam2[0];
    //    iparam2[5] = 0;
    //    iparam2[6] = 0;// 0: proj 1: AC/FB

    //    double dparam2[5];
    //    dparam2[0] = 1e-6;
    //    dparam2[2] = 1e-6;
    //    SimpleVector * z= new SimpleVector(sizeOutput);
    //    SimpleVector * w= new SimpleVector(sizeOutput);
    //    M->display();
    //    q->display();

    //    pfc_3D_nsgs(numerics_problem.numberOfContacts, numerics_problem.M->matrix0, numerics_problem.q , z->getArray() , w->getArray(), mu->data(), &info2, iparam2 , dparam2 );
    //    cout << " ... " << info2 << " " << dparam2[1] << endl;
    //    z->display();
    //    w->display();


    //  }
    //       M->display();
    //       q->display();
    //      info = (*primalFrictionContact_driver)(&numerics_problem, reaction->getArray() , velocity->getArray() , solver->getNumericsSolverOptionsPtr(), numerics_options);
    //  cout << " step 2 "<< info << endl;
    //  reaction->display();
    //  velocity->display();
    // --- Recovering of the desired variables from LCP output ---
    postCompute();

  }

  return info;
}

void PrimalFrictionContact::postCompute()
{
  // This function is used to set y/lambda values using output from primalfrictioncontact_driver
  // Only UnitaryRelations (ie Interactions) of indexSet(leveMin) are concerned.

  // === Get index set from Topology ===
  UnitaryRelationsSet * indexSet = simulation->getIndexSetPtr(levelMin);

  // y and lambda vectors
  SiconosVector * y, *lambda;

  //   // === Loop through "active" Unitary Relations (ie present in indexSets[1]) ===

  unsigned int pos = 0;
  unsigned int nsLawSize;

  //   for(UnitaryRelationsIterator itCurrent = indexSet->begin(); itCurrent!=  indexSet->end(); ++itCurrent)
  //     {
  //       // size of the unitaryBlock that corresponds to the current UnitaryRelation
  //       nsLawSize = (*itCurrent)->getNonSmoothLawSize();
  //       // Get the relative position of UR-unitaryBlock in the vector velocity or reaction
  //       pos = H->getPositionOfUnitaryBlock(*itCurrent);

  //       // Get Y and Lambda for the current Unitary Relation
  //       y = (*itCurrent)->getYPtr(levelMin);
  //       lambda = (*itCurrent)->getLambdaPtr(levelMin);

  //       // Copy velocity/reaction values, starting from index pos into y/lambda.

  //       setBlock(localVelocity, y, y->size(), pos, 0);// Warning: yEquivalent is saved in y !!
  //       setBlock(localReaction, lambda, lambda->size(), pos, 0);

  //     }

  DynamicalSystemsSet *allDS = simulation->getModelPtr()->getNonSmoothDynamicalSystemPtr()->getDynamicalSystems();;
  DSIterator itDS;
  unsigned int sizeDS;
  OneStepIntegrator * Osi;
  string osiType; // type of the current one step integrator
  string dsType; // type of the current Dynamical System
  //   for(itDS = allDS->begin(); itDS!=  allDS->end(); ++itDS)
  //     {
  //       dsType = (*itDS) -> getType();
  //       if(dsType!=LNLDS && dsType!=LLTIDS)
  //      RuntimeException::selfThrow("PrimalFrictionContact::postCompute not yet implemented for dynamical system of types "+dsType);

  //       pos = M->getPositionOfDSBlock(*itDS);
  //       sizeDS = (*itDS)->getDim();
  //       setBlock((static_cast<LagrangianDS*>(*itDS))->getVelocityPtr(),velocity,sizeDS, pos, 0 );

  //     }



}

void PrimalFrictionContact::display() const
{
  cout << "===== " << contactProblemDim << "D Primal Friction Contact Problem " << endl;
  cout << "size (sizeOutput) " << sizeOutput << endl;
  cout << "and  size (sizeLocalOutput) " << sizeLocalOutput << "(ie " << sizeLocalOutput / contactProblemDim << " contacts)." << endl;
  cout << " - Matrix M  : " << endl;
  if (M != NULL) M->display();
  else cout << "-> NULL" << endl;
  cout << " - Matrix H : " << endl;
  if (H != NULL) H->display();
  else cout << "-> NULL" << endl;
  cout << " - Vector q : " << endl;
  if (q != NULL) q->display();
  else cout << "-> NULL" << endl;
  cout << " - Vector tildeLocalVelocity : " << endl;
  if (tildeLocalVelocity != NULL) tildeLocalVelocity->display();
  else cout << "-> NULL" << endl;
  cout << " Friction coefficients: " << endl;
  if (mu != NULL) print(mu->begin(), mu->end());
  else cout << "-> NULL" << endl;
  cout << "============================================================" << endl;
}

void PrimalFrictionContact::saveNSProblemToXML()
{
  //   OneStepNSProblem::saveNSProblemToXML();
  //   if(onestepnspbxml != NULL)
  //     {
  //       (static_cast<PrimalFrictionContactXML*>(onestepnspbxml))->setM(*M);
  //       (static_cast<PrimalFrictionContactXML*>(onestepnspbxml))->setQ(*q);
  //     }
  //   else RuntimeException::selfThrow("PrimalFrictionContact::saveNSProblemToXML - OneStepNSProblemXML object not exists");
  RuntimeException::selfThrow("PrimalFrictionContact::saveNSProblemToXML - Not yet implemented.");
}

PrimalFrictionContact* PrimalFrictionContact::convert(OneStepNSProblem* osnsp)
{
  PrimalFrictionContact* fc2d = dynamic_cast<PrimalFrictionContact*>(osnsp);
  return fc2d;
}


