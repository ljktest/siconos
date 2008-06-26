/* Siconos-Kernel version 3.0.0, Copyright INRIA 2005-2008.
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
 * Contact: Vincent ACARY vincent.acary@inrialpes.fr
 */
#include "OneStepNSProblem.h"
#include "OneStepNSProblemXML.h"
#include "NonSmoothDynamicalSystem.h"
#include "UnitaryRelation.h"
#include "Interaction.h"
#include "Topology.h"
#include "Simulation.h"
#include "Model.h"
#include "Moreau.h"
#include "LagrangianDS.h"

using namespace std;

// --- CONSTRUCTORS/DESTRUCTOR ---
// xml constructor
OneStepNSProblem::OneStepNSProblem(const string& pbType, OneStepNSProblemXML* osnspbxml, Simulation* newSimu):
  nspbType(pbType), id(DEFAULT_OSNS_NAME), sizeOutput(0), solver(NULL), isSolverAllocatedIn(false),  simulation(newSimu), onestepnspbxml(osnspbxml),
  OSNSInteractions(NULL), levelMin(0), levelMax(0), maxSize(0), CPUtime(0), nbIter(0), numerics_options(NULL)
{
  if (onestepnspbxml == NULL)
    RuntimeException::selfThrow("OneStepNSProblem::xml constructor, xml file == NULL");

  // === Checks simulation ===
  if (newSimu == NULL)
    RuntimeException::selfThrow("OneStepNSProblem::xml constructor(..., simulation), simulation == NULL");

  // === get dimension of the problem ===
  if (onestepnspbxml->hasDim()) sizeOutput = onestepnspbxml->getDimNSProblem();

  // === get Id ===

  if (onestepnspbxml->hasId()) id = onestepnspbxml->getId();
  //else if( !(simulation->getOneStepNSProblems()).empty()) // An id is required if there is more than one OneStepNSProblem in the simulation
  //RuntimeException::selfThrow("OneStepNSProblem::xml constructor, an id is required for the one step non smooth problem.");

  // === read solver related data ===
  if (onestepnspbxml->hasNonSmoothSolver())
    solver = new NonSmoothSolver(onestepnspbxml->getNonSmoothSolverXMLPtr());
  else // solver = default one
    solver = new NonSmoothSolver();

  isSolverAllocatedIn = true;

  // === Link to the Interactions of the Non Smooth Dynamical System (through the Simulation) ===
  // Warning: this means that all Interactions of the NSProblem are included in the OSNS !!
  OSNSInteractions = simulation->getModelPtr()->getNonSmoothDynamicalSystemPtr()->getInteractions();

  // Numerics general options
  numerics_options = new Numerics_Options();
  numerics_options->verboseMode = 0; // turn verbose mode to off by default
}

// Constructor with given simulation and a pointer on Solver (Warning, solver is an optional argument)
OneStepNSProblem::OneStepNSProblem(const string& pbType, Simulation * newSimu, const string& newId, NonSmoothSolver* newSolver):
  nspbType(pbType), id(newId), sizeOutput(0), solver(newSolver), isSolverAllocatedIn(false),  simulation(newSimu), onestepnspbxml(NULL),
  OSNSInteractions(NULL), levelMin(0), levelMax(0), maxSize(0), CPUtime(0), nbIter(0), numerics_options(NULL)
{
  // === Checks simulation ===
  if (newSimu == NULL)
    RuntimeException::selfThrow("OneStepNSProblem:: constructor(..., newSolver, ...), newSolver == NULL");
  if (newSolver == NULL)
  {
    // If the user does not provide any Solver, an empty one is built.
    // Data will be read from XXX.opt file in Numerics.
    solver = new NonSmoothSolver();
    isSolverAllocatedIn = true;
  }

  // === Link to the Interactions of the Non Smooth Dynamical System (through the Simulation) ===
  // Warning: this means that all Interactions of the NSProblem are included in the OSNS !!
  OSNSInteractions = simulation->getModelPtr()->getNonSmoothDynamicalSystemPtr()->getInteractions();

  // === Adds this in the simulation set of OneStepNSProblem ===
  // First checks the id if required.
  if (!(simulation->getOneStepNSProblems())->empty() && id == DEFAULT_OSNS_NAME) // An id is required if there is more than one OneStepNSProblem in the simulation
    RuntimeException::selfThrow("OneStepNSProblem::constructor(...). Since the simulation has several one step non smooth problem, an id is required for each of them.");
  simulation->addOneStepNSProblemPtr(this);

  // Numerics general options
  numerics_options = new Numerics_Options();
  numerics_options->verboseMode = 0; // turn verbose mode to off by default
}

OneStepNSProblem::~OneStepNSProblem()
{
  if (isSolverAllocatedIn) delete solver;
  solver = NULL;
  simulation = NULL;
  onestepnspbxml = NULL;
  clearUnitaryBlocks();
  clearDSBlocks();
  clearUnitaryDSBlocks();
  clearDSUnitaryBlocks();
  OSNSInteractions->clear();
  OSNSInteractions = NULL;
  delete numerics_options;
  numerics_options = NULL;
}

SiconosMatrix* OneStepNSProblem::getUnitaryBlockPtr(UnitaryRelation* UR1, UnitaryRelation* UR2) const
{
  // if UR2 is not given or NULL, UR2=UR1, ie we get the diagonal unitaryBlock.
  if (UR2 == NULL) UR2 = UR1;

  ConstUnitaryMatrixRowIterator itRow = unitaryBlocks.find(UR1);
  // itRow: we get the map of unitaryBlocks that corresponds to UR1.
  // Then, thanks to itCol, we iterate through this map to find UR2 and the unitaryBlock that corresonds to UR1 and UR2
  ConstUnitaryMatrixColumnIterator itCol = (itRow->second).find(UR2);

  if (itCol == (itRow->second).end()) // if UR1 and UR2 are not connected
    RuntimeException::selfThrow("OneStepNSProblem - getUnitaryBlockPtr(UR1,UR2) : no unitaryBlock corresonds to UR1 and UR2, ie the Unitary Relations are not connected.");

  return itCol->second;

}

void OneStepNSProblem::setUnitaryBlocks(const MapOfMapOfUnitaryMatrices& newMap)
{
  //   clearUnitaryBlocks();
  //   unitaryBlocks = newMap;
  //   UnitaryMatrixRowIterator itRow;
  //   UnitaryMatrixColumnIterator itCol;
  //   for(itRow = unitaryBlocks.begin(); itRow!= unitaryBlocks.end() ; ++itRow)
  //     {
  //       for(itCol = (itRow->second).begin(); itCol!=(itRow->second).end(); ++itCol)
  //  isUnitaryBlockAllocatedIn[itRow->first][itCol->first] = false;
  //     }
  RuntimeException::selfThrow("OneStepNSProblem::setUnitaryBlocks - Not implemented: forbidden operation.");
}

void OneStepNSProblem::clearUnitaryBlocks()
{
  UnitaryMatrixRowIterator itRow;
  UnitaryMatrixColumnIterator itCol;
  for (itRow = unitaryBlocks.begin(); itRow != unitaryBlocks.end() ; ++itRow)
  {
    for (itCol = (itRow->second).begin(); itCol != (itRow->second).end(); ++itCol)
    {
      if (isUnitaryBlockAllocatedIn[itRow->first][itCol->first])
        delete unitaryBlocks[itRow->first][itCol->first];
    }
  }
  unitaryBlocks.clear();
  isUnitaryBlockAllocatedIn.clear();
}

SiconosMatrix* OneStepNSProblem::getDSBlockPtr(DynamicalSystem* DS1) const
{

  ConstMatIterator itDS = DSBlocks.find(DS1);
  return itDS->second;

}


void OneStepNSProblem::setDSBlocks(const MapOfDSMatrices& newMap)
{
  RuntimeException::selfThrow("OneStepNSProblem::setDSBlocks - Not implemented: forbidden operation.");
}

void OneStepNSProblem::clearDSBlocks()
{
  MatIterator itDS;
  for (itDS = DSBlocks.begin(); itDS != DSBlocks.end() ; ++itDS)
  {
    if (isDSBlockAllocatedIn[itDS->first])
      delete DSBlocks[itDS->first];
  }
  DSBlocks.clear();
  isDSBlockAllocatedIn.clear();
}



SiconosMatrix* OneStepNSProblem::getUnitaryDSBlockPtr(UnitaryRelation* UR1, DynamicalSystem* DS2) const
{
  ConstUnitaryDSMatrixRowIterator itRow = unitaryDSBlocks.find(UR1);

  // itRow: we get the map of DSBlocks that corresponds to UR1.
  // Then, thanks to itCol, we iterate through this map to find DS2 and the UnitaryDSBlock that corresonds to UR1 and DS2
  ConstMatIterator itCol = (itRow->second).find(DS2);

  if (itCol == (itRow->second).end()) // if UR1 and DS2 are not connected
    RuntimeException::selfThrow("OneStepNSProblem - getUnitaryDSBlockPtr(UR1,DS2) : no unitaryDSBlock corresonds to UR1 and DS2, ie the Unitary Relation and the DynamicalSystem are not connected.");

  return itCol->second;

}

void OneStepNSProblem::setUnitaryDSBlocks(const MapOfUnitaryMapOfDSMatrices& newMap)
{
  RuntimeException::selfThrow("OneStepNSProblem::setUnitaryDSBlocks - Not implemented: forbidden operation.");
}

void OneStepNSProblem::clearUnitaryDSBlocks()
{
  UnitaryDSMatrixRowIterator itRow;
  MatIterator itCol;
  for (itRow = unitaryDSBlocks.begin(); itRow != unitaryDSBlocks.end() ; ++itRow)
  {
    for (itCol = (itRow->second).begin(); itCol != (itRow->second).end(); ++itCol)
    {
      if (isUnitaryDSBlockAllocatedIn[itRow->first][itCol->first])
        delete unitaryDSBlocks[itRow->first][itCol->first];
    }
  }
  unitaryDSBlocks.clear();
  isUnitaryDSBlockAllocatedIn.clear();
}



SiconosMatrix* OneStepNSProblem::getDSUnitaryBlockPtr(DynamicalSystem* DS1, UnitaryRelation* UR2) const
{
  ConstDSUnitaryMatrixRowIterator itRow = DSUnitaryBlocks.find(DS1);
  ConstUnitaryMatrixColumnIterator itCol = (itRow->second).find(UR2);
  if (itCol == (itRow->second).end()) // if DS1 and UR2 are not connected
    RuntimeException::selfThrow("OneStepNSProblem - getDSUnitaryBlockPtr(DS1,UR2) : no DSUnitaryBlock corresonds to DS1 and UR2, ie the Unitary Relation and the DynamicalSystem are not connected.");

  return itCol->second;

}

void OneStepNSProblem::setDSUnitaryBlocks(const MapOfDSMapOfUnitaryMatrices& newMap)
{
  RuntimeException::selfThrow("OneStepNSProblem::setDSUnitaryBlocks - Not implemented: forbidden operation.");
}

void OneStepNSProblem::clearDSUnitaryBlocks()
{
  DSUnitaryMatrixRowIterator itRow;
  UnitaryMatrixColumnIterator itCol;

  for (itRow = DSUnitaryBlocks.begin(); itRow != DSUnitaryBlocks.end() ; ++itRow)
  {
    for (itCol = (itRow->second).begin(); itCol != (itRow->second).end(); ++itCol)
    {
      if (isDSUnitaryBlockAllocatedIn[itRow->first][itCol->first])
        delete DSUnitaryBlocks[itRow->first][itCol->first];
    }
  }
  DSUnitaryBlocks.clear();
  isDSUnitaryBlockAllocatedIn.clear();
}







void OneStepNSProblem::setNonSmoothSolverPtr(NonSmoothSolver * newSolv)
{
  if (isSolverAllocatedIn) delete solver;
  solver = newSolv;
  isSolverAllocatedIn = false;
}

void OneStepNSProblem::updateUnitaryBlocks()
{
  // The present functions checks various conditions and possibly compute unitaryBlocks matrices.
  //
  // Let URi and URj be two Unitary Relations.
  //
  // Things to be checked are:
  //  1 - is the topology time invariant?
  //  2 - does unitaryBlocks[URi][URj] already exists (ie has been computed in a previous time step)?
  //  3 - do we need to compute this unitaryBlock? A unitaryBlock is to be computed if URi and URj are in IndexSet1 AND if URi and URj have common DynamicalSystems.
  //
  // The possible cases are:
  //
  //  - If 1 and 2 are true then it does nothing. 3 is not checked.
  //  - If 1 == true, 2 == false, 3 == false, it does nothing.
  //  - If 1 == true, 2 == false, 3 == true, it computes the unitaryBlock.
  //  - If 1==false, 2 is not checked, and the unitaryBlock is computed if 3==true.
  //
  UnitaryRelationsSet * indexSet;
  bool isTimeInvariant;
  UnitaryRelationsIterator itUR1, itUR2;
  DynamicalSystemsSet commonDS;
  // Get index set from Simulation

  indexSet = simulation->getIndexSetPtr(levelMin);


  isTimeInvariant = simulation->getModelPtr()->getNonSmoothDynamicalSystemPtr()->getTopologyPtr()->isTimeInvariant();
  for (itUR1 = indexSet->begin(); itUR1 != indexSet->end(); ++itUR1)
  {

    for (itUR2 = indexSet->begin(); itUR2 != indexSet->end(); ++itUR2)
    {
      if (!isTimeInvariant)
        computeUnitaryBlock(*itUR1, *itUR2);
      else // if(isTimeInvariant)
      {
        if ((unitaryBlocks.find(*itUR1)) != unitaryBlocks.end())  // if unitaryBlocks[UR1] exists
        {
          if ((unitaryBlocks[*itUR1].find(*itUR2)) == (unitaryBlocks[*itUR1].end()))   // if unitaryBlocks[UR1][UR2] does not exist
            computeUnitaryBlock(*itUR1, *itUR2);
        }
        else computeUnitaryBlock(*itUR1, *itUR2);
      }
    }
  }
}

void OneStepNSProblem::computeAllUnitaryBlocks()
{
  UnitaryRelationsSet * indexSet = simulation->getIndexSetPtr(0);

  UnitaryRelationsIterator itUR1, itUR2;
  DynamicalSystemsSet commonDS;

  for (itUR1 = indexSet->begin(); itUR1 != indexSet->end(); ++itUR1)
  {
    for (itUR2 = indexSet->begin(); itUR2 != indexSet->end(); ++itUR2)
      computeUnitaryBlock(*itUR1, *itUR2);
  }
}

void OneStepNSProblem::computeUnitaryBlock(UnitaryRelation*, UnitaryRelation*)
{
  RuntimeException::selfThrow("OneStepNSProblem::computeUnitaryBlock - not yet implemented for problem type =" + nspbType);
}

void OneStepNSProblem::updateDSBlocks()
{
  // The present functions checks various conditions and possibly compute DSBlocks matrices.
  //
  // Let URi and URj be two Unitary Relations.
  //
  // Things to be checked are:
  //  1 - is the topology time invariant?
  //  2 - does DSBlocks[DSi] already exists (ie has been computed in a previous time step)?
  //  3 - do we need to compute this DSBlock? A DSBlock is to be computed if DSi is concerned by a UR in IndexSet1
  //
  // The possible cases are:
  //
  //  - If 1 and 2 are true then it does nothing. 3 is not checked.
  //  - If 1 == true, 2 == false, 3 == false, it does nothing.
  //  - If 1 == true, 2 == false, 3 == true, it computes the unitaryBlock.
  //  - If 1==false, 2 is not checked, and the unitaryBlock is computed if 3==true.
  //

  // \warning We decided to include all dynamical systems test 3 is not satisfied


  bool isTimeInvariant = simulation->getModelPtr()->getNonSmoothDynamicalSystemPtr()->getTopologyPtr()->isTimeInvariant();
  DynamicalSystemsSet *allDS = simulation->getModelPtr()->getNonSmoothDynamicalSystemPtr()->getDynamicalSystems();;

  DSIterator itDS;
  for (itDS = allDS->begin(); itDS != allDS->end(); ++itDS)
  {
    if (!isTimeInvariant)
      computeDSBlock(*itDS);
    else // if(isTimeInvariant)
    {
      if ((DSBlocks.find(*itDS)) != DSBlocks.end())  // if unitaryBlocks[UR1] exists
      {
        ; // do nothing
      }
      else computeDSBlock(*itDS);
    }
  }

}

void OneStepNSProblem::computeAllDSBlocks()
{
  DynamicalSystemsSet* allDS;
  DSIterator itDS;
  allDS = simulation->getModelPtr()->getNonSmoothDynamicalSystemPtr()->getDynamicalSystems();

  for (itDS = allDS->begin(); itDS != allDS->end(); ++itDS)
  {
    computeDSBlock(*itDS);
  }
}

void OneStepNSProblem::computeDSBlock(DynamicalSystem*)
{
  RuntimeException::selfThrow("OneStepNSProblem::computeDSBlock - not yet implemented for problem type =" + nspbType);
}



void OneStepNSProblem::updateUnitaryDSBlocks()
{
  // The present functions checks various conditions and possibly compute unitaryBlocks matrices.
  //
  // Let URi and DSj be an Unitary Relation and a DynamicalSystem.
  //
  // Things to be checked are:
  //  1 - is the topology time invariant?
  //  2 - does unitaryDSBlocks[URi][DSj] already exists (ie has been computed in a previous time step)?
  //  3 - do we need to compute this unitaryDSBlock? A unitaryDSBlock is to be computed if URi is in IndexSet1 AND if DSj is a Dynamical systems concerned by URi
  //
  // The possible cases are:
  //
  //  - If 1 and 2 are true then it does nothing. 3 is not checked.
  //  - If 1 == true, 2 == false, 3 == false, it does nothing.
  //  - If 1 == true, 2 == false, 3 == true, it computes the unitaryBlock.
  //  - If 1==false, 2 is not checked, and the unitaryBlock is computed if 3==true.
  //
  UnitaryRelationsSet * indexSet;
  bool isTimeInvariant;
  UnitaryRelationsIterator itUR;
  DSIterator itDS;
  DynamicalSystemsSet* concernedDS;


  // Get index set from Simulation

  indexSet = simulation->getIndexSetPtr(levelMin);


  isTimeInvariant = simulation->getModelPtr()->getNonSmoothDynamicalSystemPtr()->getTopologyPtr()->isTimeInvariant();

  itUR = indexSet->begin();

  for (itUR = indexSet->begin(); itUR != indexSet->end(); ++itUR)
  {
    concernedDS = (*itUR)->getDynamicalSystemsPtr();

    for (itDS = concernedDS->begin(); itDS != concernedDS->end(); itDS++)
    {
      if (!isTimeInvariant)
      {

        computeUnitaryDSBlock(*itUR, *itDS);
      }
      else // if(isTimeInvariant)
      {
        if ((unitaryDSBlocks.find(*itUR)) != unitaryDSBlocks.end())  // if unitaryBlocks[UR] exists
        {
          if ((unitaryDSBlocks[*itUR].find(*itDS)) == (unitaryDSBlocks[*itUR].end()))   // if unitaryBlocks[UR][DS2] does not exist
          {

            computeUnitaryDSBlock(*itUR, *itDS);
          }
        }
        else
        {
          computeUnitaryDSBlock(*itUR, *itDS);

        }
      }
    }
  }

}

void OneStepNSProblem::computeAllUnitaryDSBlocks()
{
  UnitaryRelationsSet * indexSet = simulation->getIndexSetPtr(0);
  DSIterator itDS;
  UnitaryRelationsIterator itUR;
  DynamicalSystemsSet concernedDS;

  for (itUR = indexSet->begin(); itUR != indexSet->end(); ++itUR)
  {
    concernedDS =  *((*itUR)->getDynamicalSystemsPtr());
    for (itDS = concernedDS.begin(); itDS != concernedDS.end(); itDS++)
      computeUnitaryDSBlock(*itUR, *itDS);
  }
}

void OneStepNSProblem::computeUnitaryDSBlock(UnitaryRelation*, DynamicalSystem*)
{
  RuntimeException::selfThrow("OneStepNSProblem::computeUnitaryDSBlock - not yet implemented for problem type =" + nspbType);
}


void OneStepNSProblem::updateDSUnitaryBlocks()
{
  // The present functions checks various conditions and possibly compute unitaryBlocks matrices.
  //
  // Let DSi and URj be a DynamicalSystem and an Unitary Relation
  //
  // Things to be checked are:
  //  1 - is the topology time invariant?
  //  2 - does DUunitaryBlocks[DSi][URj] already exists (ie has been computed in a previous time step)?
  //  3 - do we need to compute this DSunitaryBlock? A DSUnitaryBlock is to be computed if DSi is a Dynamical systems concerned by URj AND  if URj is in IndexSet1
  //
  // The possible cases are:
  //
  //  - If 1 and 2 are true then it does nothing. 3 is not checked.
  //  - If 1 == true, 2 == false, 3 == false, it does nothing.
  //  - If 1 == true, 2 == false, 3 == true, it computes the unitaryBlock.
  //  - If 1==false, 2 is not checked, and the unitaryBlock is computed if 3==true.
  //
  UnitaryRelationsSet * indexSet;
  bool isTimeInvariant;
  UnitaryRelationsIterator itUR;
  DSIterator itDS;
  DynamicalSystemsSet* concernedDS;


  // Get index set from Simulation

  indexSet = simulation->getIndexSetPtr(levelMin);
  isTimeInvariant = simulation->getModelPtr()->getNonSmoothDynamicalSystemPtr()->getTopologyPtr()->isTimeInvariant();

  for (itUR = indexSet->begin(); itUR != indexSet->end(); ++itUR)
  {
    concernedDS = (*itUR)->getDynamicalSystemsPtr();
    for (itDS = concernedDS->begin(); itDS != concernedDS->end(); itDS++)
    {
      if (!isTimeInvariant)
        computeDSUnitaryBlock(*itDS, *itUR);
      else // if(isTimeInvariant)
      {
        if ((DSUnitaryBlocks.find(*itDS)) != DSUnitaryBlocks.end())  // if unitaryBlocks[UR] exists
        {
          if ((DSUnitaryBlocks[*itDS].find(*itUR)) == (DSUnitaryBlocks[*itDS].end()))   // if unitaryBlocks[UR][DS2] does not exist
            computeDSUnitaryBlock(*itDS, *itUR);
        }
        else computeDSUnitaryBlock(*itDS, *itUR);
      }
    }
  }
}

void OneStepNSProblem::computeAllDSUnitaryBlocks()
{
  UnitaryRelationsSet * indexSet = simulation->getIndexSetPtr(0);
  DSIterator itDS;
  UnitaryRelationsIterator itUR;
  DynamicalSystemsSet* concernedDS;

  for (itUR = indexSet->begin(); itUR != indexSet->end(); ++itUR)
  {
    concernedDS = (*itUR)->getDynamicalSystemsPtr();
    for (itDS = concernedDS->begin(); itDS != concernedDS->end(); itDS++)
      computeDSUnitaryBlock(*itDS, *itUR);
  }
}

void OneStepNSProblem::computeDSUnitaryBlock(DynamicalSystem*, UnitaryRelation*)
{
  RuntimeException::selfThrow("OneStepNSProblem::computeDSUnitaryBlock - not yet implemented for problem type =" + nspbType);
}
void OneStepNSProblem::initialize()
{
  // Checks that the set of Interactions is not empty -
  // Empty set is not forbidden, then we just display a warning message.
  if (OSNSInteractions->isEmpty())
    //RuntimeException::selfThrow("OneStepNSProblem::initialize - The set of Interactions of this problem is empty.");
    cout << "Warning, OneStepNSProblem::initialize, the set of Interactions of this problem is empty." << endl;
  else
    updateUnitaryBlocks();



  Topology * topology = simulation->getModelPtr()->getNonSmoothDynamicalSystemPtr()->getTopologyPtr();
  // The maximum size of the problem (for example, the dim. of M in LCP or Friction problems).
  // Set to the number of possible scalar constraints declared in the topology.
  if (maxSize == 0) // if maxSize not set explicitely by user before initialize
    maxSize = topology->getNumberOfConstraints();
}

void OneStepNSProblem::saveInMemory()
{
  InteractionsIterator it;
  for (it = OSNSInteractions->begin(); it != OSNSInteractions->end(); it++)
    (*it)->swapInMemory();
}

void OneStepNSProblem::saveNSProblemToXML()
{
  // OUT OF DATE - TO BE REVIEWED

  //   if(onestepnspbxml != NULL)
  //     {
  //       onestepnspbxml->setDimNSProblem(sizeOutput);
  //       vector<int> v;
  //       InteractionsIterator it;
  //       for(it=OSNSInteractions->begin(); it!=OSNSInteractions->end();++it)
  //  v.push_back((*it)->getNumber());
  //       //onestepnspbxml->setInteractionConcerned( v, allInteractionConcerned() );

  //       //onestepnspbxml->setSolver(solvingFormalisation, methodName, normType, tolerance, maxIter, searchDirection );
  //     }
  //   else RuntimeException::selfThrow("OneStepNSProblem::saveNSProblemToXML - OneStepNSProblemXML object not exists");
  RuntimeException::selfThrow("OneStepNSProblem::saveNSProblemToXML - Not yet implemented");
}

void OneStepNSProblem::getOSIMaps(UnitaryRelation* UR, MapOfDSMatrices& centralUnitaryBlocks, MapOfDouble& Theta)
{
  // === OSI = MOREAU : gets W matrices and scalar Theta of each DS concerned by the UnitaryRelation ===
  // === OSI = LSODAR : gets M matrices of each DS concerned by the UnitaryRelation, Theta remains empty ===

  OneStepIntegrator * Osi;
  string osiType; // type of the current one step integrator
  DSTYPES dsType; // type of the current Dynamical System
  DSIterator itDS = UR->dynamicalSystemsBegin();
  while (itDS != (UR->dynamicalSystemsEnd()))
  {
    Osi = simulation->getIntegratorOfDSPtr(*itDS); // get OneStepIntegrator of current dynamical system
    osiType = Osi->getType();
    if (osiType == "Moreau")
    {
      centralUnitaryBlocks[*itDS] = (static_cast<Moreau*>(Osi))->getWPtr(*itDS);  // get its W matrix ( pointer link!)
      Theta[*itDS] = (static_cast<Moreau*>(Osi))->getTheta(*itDS);
    }
    else if (osiType == "Lsodar") // Warning: LagrangianDS only at the time !!!
    {
      dsType = (*itDS)->getType();
      if (dsType != LNLDS && dsType != LLTIDS)
        RuntimeException::selfThrow("OneStepNSProblem::getOSIMaps not yet implemented for Lsodar Integrator with dynamical system of type " + dsType);

      // get lu-factorized mass
      centralUnitaryBlocks[*itDS] = (static_cast<LagrangianDS*>(*itDS))->getMassLUPtr();
    }
    else
      RuntimeException::selfThrow("OneStepNSProblem::getOSIMaps not yet implemented for Integrator of type " + osiType);
    ++itDS;
  }
}

void OneStepNSProblem::printStat()
{
  cout << " CPU time for solving : " << CPUtime / (double)CLOCKS_PER_SEC << endl;
  cout << " Number of iterations done: " << nbIter << endl;
}

