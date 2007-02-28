/* Siconos-Kernel version 2.0.1, Copyright INRIA 2005-2006.
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
#include "Moreau.h"
#include "MoreauXML.h"
#include "Simulation.h"
#include "Model.h"
#include "NonSmoothDynamicalSystem.h"
#include "TimeDiscretisation.h"

#include "LagrangianLinearTIDS.h"
#include "FirstOrderLinearTIDS.h"

using namespace std;

// --- Default constructor ---
Moreau::Moreau(): OneStepIntegrator()
{
  integratorType = "Moreau";
}

// --- xml constructor ---
Moreau::Moreau(OneStepIntegratorXML * osiXML, Simulation* newS):
  OneStepIntegrator("Moreau", newS)
{
  // Note: we do not call xml constructor of OSI, but default one, since we need to download theta and DS at the same time.

  if (osiXML == NULL)
    RuntimeException::selfThrow("Moreau::xml constructor - OneStepIntegratorXML object == NULL.");

  integratorXml = osiXML;
  MoreauXML * moreauXml = static_cast<MoreauXML*>(osiXML);

  // Required inputs: a list of DS and one theta per DS.
  // No xml entries at the time for sizeMem and W.

  if (!osiXML->hasDSList())
    RuntimeException::selfThrow("Moreau::xml constructor - DS list is missing in xml input file.");
  if (!moreauXml->hasThetaList())
    RuntimeException::selfThrow("Moreau::xml constructor - theta list is missing in xml input file.");

  vector<double> thetaXml;     // list of theta values
  // thetaXml[i] will correspond to the i-eme ds in the xml list. If all attributes is true in ds,
  // then theta values are sorted so to correspond to growing ds numbers order.

  if (moreauXml->hasAllTheta()) // if one single value for all theta
    thetaXml.push_back(moreauXml->getSingleTheta());
  else
    moreauXml->getTheta(thetaXml);

  NonSmoothDynamicalSystem * nsds = simulationLink->getModelPtr()->getNonSmoothDynamicalSystemPtr();

  if (osiXML->hasAllDS()) // if flag all=true is present -> get all ds from the nsds
  {
    // In nsds DS are saved in a vector.
    // Future version: saved them in a set? And then just call:
    // OSIDynamicalSystems = nsds->getDynamicalSystems();
    DynamicalSystemsSet tmpDS = nsds->getDynamicalSystems();
    DSIterator it;
    unsigned int i = 0;
    for (it = tmpDS.begin(); it != tmpDS.end(); ++it)
    {
      OSIDynamicalSystems.insert(*it);
      // get corresponding theta. In xml they must be sorted in an order that corresponds to growing DS-numbers order.
      if (moreauXml->hasAllTheta()) // if one single value for all theta
        thetaMap[*it] = thetaXml[0];
      else
        thetaMap[*it] = thetaXml[i++];
    }
  }
  else
  {
    // get list of ds numbers implicated in the OSI
    vector<int> dsNumbers;
    osiXML->getDSNumbers(dsNumbers);
    unsigned int i = 0;
    // get corresponding ds and insert them into the set.
    vector<int>::iterator it;
    for (it = dsNumbers.begin(); it != dsNumbers.end(); ++it)
    {
      OSIDynamicalSystems.insert(nsds->getDynamicalSystemPtrNumber(*it));
      if (moreauXml->hasAllTheta()) // if one single value for all theta
        thetaMap[nsds->getDynamicalSystemPtrNumber(*it)] = thetaXml[0];
      else
        thetaMap[nsds->getDynamicalSystemPtrNumber(*it)] = thetaXml[i++];
    }
  }

  // W loading: not yet implemented
  if (moreauXml->hasWList())
    RuntimeException::selfThrow("Moreau::xml constructor - W matrix loading not yet implemented.");
}

// --- constructor from a minimum set of data ---
Moreau::Moreau(DynamicalSystem* newDS, const double newTheta, Simulation* newS): OneStepIntegrator("Moreau", newS)
{
  if (simulationLink == NULL)
    RuntimeException::selfThrow("Moreau:: constructor (ds,theta,simulation) - simulation == NULL");

  OSIDynamicalSystems.insert(newDS);
  thetaMap[newDS] = newTheta;
}

// --- constructor from a set of data ---
Moreau::Moreau(DynamicalSystemsSet& newDS, const double newTheta, Simulation* newS): OneStepIntegrator("Moreau", newDS, newS)
{
  if (simulationLink == NULL)
    RuntimeException::selfThrow("Moreau:: constructor (setOfDS,theta,simulation) - simulation == NULL");

  DSIterator itDS;
  for (itDS = OSIDynamicalSystems.begin(); itDS != OSIDynamicalSystems.end(); ++itDS)
    thetaMap[*itDS] = newTheta;
}

Moreau::Moreau(DynamicalSystemsSet& newDS, const MapOfDouble& newTheta, Simulation* newS): OneStepIntegrator("Moreau", newDS, newS)
{
  if (simulationLink == NULL)
    RuntimeException::selfThrow("Moreau:: constructor (setOfDS,theta,simulation) - simulation == NULL");

  thetaMap = newTheta;
}

Moreau::~Moreau()
{
  matIterator it;
  for (it = WMap.begin(); it != WMap.end(); ++it)
    if (isWAllocatedInMap[it->first]) delete it->second;
  WMap.clear();
  thetaMap.clear();

  DSIterator itDS;
  for (itDS = OSIDynamicalSystems.begin(); itDS != OSIDynamicalSystems.end(); ++itDS)
    if (*itDS != NULL && (*itDS)->getType() == LNLDS)(*itDS)->freeWorkVector("LagNLDSMoreau");
}

void Moreau::setWMap(const MapOfMatrices& newMap)
{
  // check sizes.
  if (newMap.size() != OSIDynamicalSystems.size())
    RuntimeException::selfThrow("Moreau::setWMap(newMap): number of W matrices is different from number of DS.");

  // pointer links! No reallocation
  matIterator it;
  for (it = WMap.begin(); it != WMap.end(); ++it)
    if (isWAllocatedInMap[it->first]) delete it->second;

  WMap.clear();
  WMap = newMap;
  isWAllocatedInMap.clear();
  for (it = WMap.begin(); it != WMap.end(); ++it)
    isWAllocatedInMap[it->first] = false;
}

const SimpleMatrix Moreau::getW(DynamicalSystem* ds)
{
  if (ds == NULL)
    return *(WMap[0]);
  if (WMap[ds] == NULL)
    RuntimeException::selfThrow("Moreau::getW(ds): W[ds] == NULL.");
  return *(WMap[ds]);
}

SiconosMatrix* Moreau::getWPtr(DynamicalSystem* ds)
{
  if (ds == NULL)
    return WMap[0];
  if (WMap[ds] == NULL)
    RuntimeException::selfThrow("Moreau::getWPtr(ds): W[ds] == NULL.");
  return WMap[ds];
}

void Moreau::setW(const SiconosMatrix& newValue, DynamicalSystem* ds)
{
  unsigned int line = newValue.size(0);
  unsigned int col  = newValue.size(1);

  if (line != col) // Check that newValue is square
    RuntimeException::selfThrow("Moreau::setW(newVal,ds) - newVal is not square! ");

  if (ds == NULL)
    RuntimeException::selfThrow("Moreau::setW(newVal,ds) - ds == NULL.");

  unsigned int sizeW = ds->getDim(); // n for first order systems, ndof for lagrangian.
  if (line != sizeW) // check consistency between newValue and dynamical system size
    RuntimeException::selfThrow("Moreau::setW(newVal) - unconsistent dimension between newVal and dynamical system to be integrated ");

  if (WMap[ds] == NULL) // allocate a new W if required
  {
    WMap[ds] = new SimpleMatrix(newValue);
    isWAllocatedInMap[ds] = true;
  }
  else  // or fill-in an existing one if dimensions are consistent.
  {
    if (line == WMap[ds]->size(0) && col == WMap[ds]->size(1))
      *(WMap[ds]) = newValue;
    else
      RuntimeException::selfThrow("Moreau - setW: inconsistent dimensions with problem size for given input matrix W");
  }
}

void Moreau::setWPtr(SiconosMatrix *newPtr, DynamicalSystem* ds)
{
  unsigned int line = newPtr->size(0);
  unsigned int col  = newPtr->size(1);
  if (line != col) // Check that newPtr is square
    RuntimeException::selfThrow("Moreau::setWPtr(newVal) - newVal is not square! ");

  if (ds == NULL)
    RuntimeException::selfThrow("Moreau::setWPtr(newVal,ds) - ds == NULL.");

  unsigned int sizeW = ds->getDim(); // n for first order systems, ndof for lagrangian.
  if (line != sizeW) // check consistency between newValue and dynamical system size
    RuntimeException::selfThrow("Moreau::setW(newVal) - unconsistent dimension between newVal and dynamical system to be integrated ");

  if (isWAllocatedInMap[ds]) delete WMap[ds]; // free memory for previous W
  WMap[ds] = newPtr;                  // link with new pointer
  isWAllocatedInMap[ds] = false;
}

void Moreau::setThetaMap(const MapOfDouble& newMap) // useless function ?
{
  thetaMap = newMap;
}

const double Moreau::getTheta(DynamicalSystem* ds)
{
  if (ds == NULL)
    RuntimeException::selfThrow("Moreau::getTheta(ds) - ds == NULL");
  return thetaMap[ds];
}

void Moreau::setTheta(const double newTheta, DynamicalSystem* ds)
{
  if (ds == NULL)
    RuntimeException::selfThrow("Moreau::setTheta(val,ds) - ds == NULL");
  thetaMap[ds] = newTheta;
}

void Moreau::initialize()
{
  OneStepIntegrator::initialize();
  // Get initial time
  double t0 = simulationLink->getModelPtr()->getT0();
  // Compute W(t0) for all ds
  DSIterator it;
  for (it = OSIDynamicalSystems.begin(); it != OSIDynamicalSystems.end(); ++it)
  {
    computeW(t0, *it);
    if ((*it)->getType() == LNLDS)
      (*it)->allocateWorkVector("LagNLDSMoreau", WMap[*it]->size(0));
  }
}

void Moreau::computeW(const double t, DynamicalSystem* ds)
{
  if (ds == NULL)
    RuntimeException::selfThrow("Moreau::computeW(t,ds) - ds == NULL");

  double h = simulationLink->getTimeStep();

  double theta = thetaMap[ds];
  // Check if W is allocated; if not, do allocation.
  if (WMap[ds] == NULL)
  {
    unsigned int sizeW = ds->getDim(); // n for first order systems, ndof for lagrangian.
    WMap[ds] = new SimpleMatrix(sizeW, sizeW);
    isWAllocatedInMap[ds] = true;
  }

  SiconosMatrix * W = WMap[ds];

  // === Lagrangian systems
  string dsType = ds->getType();
  if (dsType == LNLDS || dsType == LLTIDS)
  {
    LagrangianDS* d = static_cast<LagrangianDS*>(ds);
    SiconosMatrix *K, *C, *Mass;

    // Get Mass matrix
    Mass = d->getMassPtr();
    // Compute W
    *W = *Mass ;

    if (dsType == LNLDS)
    {
      // === Lagrangian non-linear systems ===
      // Compute Mass matrix (if loaded from plugin)
      d->computeMass();
      // Compute and get Jacobian (if loaded from plugin)
      d->computeJacobianFL(0, t);
      d->computeJacobianFL(1, t);
      K = d->getJacobianFLPtr(0);
      C = d->getJacobianFLPtr(1);
      if (C != NULL)
        *W -= h * theta**C;
      if (K != NULL)
        *W -= h * h * theta * theta**K;
    }
    else // if dsType = LLTIDS, LagrangianLinearTIDS
    {
      // === Lagrangian linear time invariant system ===
      K = ((static_cast<LagrangianLinearTIDS*>(d))->getKPtr());
      C = ((static_cast<LagrangianLinearTIDS*>(d))->getCPtr());
      if (C != NULL)
        *W += h * theta**C;
      if (K != NULL)
        *W +=  h * h * theta * theta**K;
    }
  }

  // === Linear dynamical system ===
  else if (dsType == FOLDS || dsType == FOLTIDS)
  {
    FirstOrderLinearDS* d = static_cast<FirstOrderLinearDS*>(ds);
    SiconosMatrix *I;
    unsigned int size = d->getN();
    // Deals with M
    if (d->getMPtr() == NULL)
    {
      I = new SimpleMatrix(size, size);
      I->eye();
      *W = *I - (h * theta * (d->getA()));
      delete I;
    }
    else
    {
      I = d->getMPtr();
      *W = *I - (h * theta * (d->getA()));
    }
  }
  // === ===
  else RuntimeException::selfThrow("Moreau::computeW - not yet implemented for Dynamical system type :" + dsType);

  // Remark: W is not LU-factorized here.
  // Function PLUForwardBackward will do that if required.
}


void Moreau::computeFreeState()
{
  // get current and next times, theta and time step
  double t = simulationLink->getNextTime();
  double told = simulationLink->getCurrentTime();
  double h = t - told;

  DSIterator it;
  double theta;
  SiconosMatrix * W;
  for (it = OSIDynamicalSystems.begin(); it != OSIDynamicalSystems.end(); ++it)
  {
    DynamicalSystem* ds = *it;
    theta = thetaMap[ds];

    string dsType = ds->getType();

    W = WMap[ds];

    // === Lagrangian Systems ===
    if ((dsType == LNLDS) || (dsType == LLTIDS))
    {
      // -- Get the DS --
      LagrangianDS* d = static_cast<LagrangianDS*>(ds);

      // --- RESfree computation ---
      //
      // Get state i (previous time step) from Memories -> var. indexed with "Old"
      SiconosVector* qold, *vold;
      qold = static_cast<SimpleVector*>(d->getQMemoryPtr()->getSiconosVector(0));
      vold = static_cast<SimpleVector*>(d->getVelocityMemoryPtr()->getSiconosVector(0));

      // Velocity free and residu.
      SiconosVector *vfree = d->getVelocityFreePtr();
      SiconosVector *RESfree = vfree;
      RESfree->zero(); // Note that RESfree and vFree are located at the same place in Memory.
      // --- Compute Velocity Free ---
      // For non-linear Lagrangian systems (LNLDS)
      if (dsType == LNLDS)
      {
        // Get Mass (remark: M is computed for present state during computeW(t) )
        computeW(t, d);
        SiconosMatrix *M = d->getMassPtr();
        SiconosVector *v = d->getVelocityPtr();

        // === Compute ResFree and vfree solution of Wk(v-vfree)= RESfree ===
        *RESfree = prod(*M, (*v - *vold));

        // Compute fL
        // warning: get values and not pointers for "old" state.

        if (d->getFLPtr() != NULL)
        {
          SiconosVector *fLOld = NULL, *fLCurrent = NULL;
          d->computeFL(told, qold, vold);
          fLOld = new SimpleVector(*(d->getFLPtr()));
          d->computeFL(t);
          fLCurrent = d->getFLPtr();
          *RESfree -= h * ((1.0 - theta)**fLOld + theta**fLCurrent);

          delete fLOld;
          fLCurrent = NULL;
        }
        // W(vFree-vOld) = RFree. Remind that vfree and RESfree are just two names for the same thing.
        W->PLUForwardBackwardInPlace(*RESfree);
        *vfree =  *v - *RESfree;
      }
      // --- For linear Lagrangian LLTIDS:
      else
      {
        // get K and C pointers
        SiconosMatrix * K = static_cast<LagrangianLinearTIDS*>(d)->getKPtr();
        SiconosMatrix * C = static_cast<LagrangianLinearTIDS*>(d)->getCPtr();
        // Compute ResFree and vfree
        if (K != NULL)
          *RESfree -= h * (prod(*K, *qold) + h * theta * prod(*K, *vold));
        if (C != NULL)
          *RESfree -= h * prod(*C, *vold);
        // Computation of the external forces
        SiconosVector * FExtOld = NULL, *FExtCurrent = NULL;
        if (d->getFExtPtr() != NULL)
        {
          d->computeFExt(told);
          FExtOld = new SimpleVector(*(d->getFExtPtr()));
          d->computeFExt(t);
          FExtCurrent = d->getFExtPtr();
          *RESfree += h * (theta**FExtCurrent + (1.0 - theta)**FExtOld);
          delete FExtOld;
          FExtCurrent = NULL;
        }

        // W(vFree-vOld) = RFree.
        W->PLUForwardBackwardInPlace(*RESfree);
        *vfree =  *vold + *RESfree;
      }
      // calculate qfree (whereas it is useless for future computation)
      SiconosVector *qfree = d->getQFreePtr();
      *qfree = (*qold) + h * (theta * (*vfree) + (1.0 - theta) * (*vold));
    }
    else if (dsType == FOLDS || dsType == FOLTIDS)
    {
      FirstOrderLinearDS *d = static_cast<FirstOrderLinearDS*>(ds);
      SimpleVector *xfree = static_cast<SimpleVector*>(d->getXFreePtr());
      SimpleVector *xold = static_cast<SimpleVector*>(d->getXMemoryPtr()->getSiconosVector(0));
      SimpleVector *rold = static_cast<SimpleVector*>(d->getRMemoryPtr()->getSiconosVector(0));
      unsigned int sizeX = xfree->size();

      SiconosMatrix *A = d->getAPtr();
      SiconosMatrix *I;
      // Deals with M
      if (d->getMPtr() == NULL)
      {
        I = new SimpleMatrix(sizeX, sizeX);
        I->eye();
        *xfree =  prod((*I + h * (1.0 - theta) * *A), *xold) + (h * (1.0 - theta) * *rold);
        delete I;
      }
      else
      {
        I = d->getMPtr();
        *xfree = prod((*I + h * (1.0 - theta) * *A), *xold) + (h * (1.0 - theta) * *rold);
      }

      // Warning: b is supposed to be constant, not time dependent.
      SimpleVector *b = d->getBPtr();
      if (b != NULL)
        *xfree += h * *b;

      //        *xfree += h * prod(*T, (theta * uCurrent + (1.0-theta) * uOld));
      //      }

      W->PLUForwardBackwardInPlace(*xfree);
    }
    else RuntimeException::selfThrow("Moreau::computeFreeState - not yet implemented for Dynamical system type: " + dsType);
  }
}


void Moreau::integrate(double& tinit, double& tend, double& tout, int&)
{
  double h = tend - tinit;
  tout = tend;

  DSIterator it;
  SiconosMatrix * W;
  double theta;
  for (it = OSIDynamicalSystems.begin(); it != OSIDynamicalSystems.end(); ++it)
  {
    DynamicalSystem* ds = *it;
    W = WMap[ds];
    theta = thetaMap[ds];
    string dsType = ds->getType();

    if (dsType == LNLDS)
    {
      RuntimeException::selfThrow("Moreau::integrate - not yet implemented for Dynamical system type: " + dsType);
      // We do not use integrate() for LNDS
    }
    else if (dsType == LLTIDS)
    {
      // get the ds
      LagrangianLinearTIDS* d = static_cast<LagrangianLinearTIDS*>(ds);
      // get q and velocity pointers for current time step
      SiconosVector *v, *q, *vold, *qold;
      q = d->getQPtr();
      v = d->getVelocityPtr();
      // get q and velocity pointers for previous time step
      qold = static_cast<SimpleVector*>(d->getQMemoryPtr()->getSiconosVector(0));
      vold = static_cast<SimpleVector*>(d->getVelocityMemoryPtr()->getSiconosVector(0));
      // get mass, K and C pointers
      SiconosMatrix *M, *K, *C;
      M = d->getMassPtr();
      K = d->getKPtr();
      C = d->getCPtr();
      // get p pointer
      SiconosVector  *p;
      p = d->getPPtr(2);
      // Inline Version
      // The method computeFExt does not allow to compute directly
      // as a function.  To do that, you have to call directly the function of the plugin
      // or call the F77 function  MoreauLTIDS
      // Computation of the external forces
      d->computeFExt(tinit);
      SimpleVector FExt0 = d->getFExt();
      d->computeFExt(tend);
      SimpleVector FExt1 = d->getFExt();
      // velocity computation

      *v = (h * (theta * FExt1 + (1.0 - theta) * FExt0 - prod(*C, *vold) - prod(*K, *qold) - h * theta * prod(*K, *vold)) + *p);
      W->PLUForwardBackwardInPlace(*v);
      *v += *vold;
      // q computation
      *q = (*qold) + h * ((theta * (*v)) + (1.0 - theta) * (*vold));
      // Right Way  : Fortran 77 version with BLAS call
      // F77NAME(MoreauLTIDS)(tinit,tend,theta
      //                      ndof, &qold(0),&vold(0),
      //                      &W(0,0),&K(0,0),&C(0,0),fext,
      //                      &v(0),&q(0))
    }
    else RuntimeException::selfThrow("Moreau::integrate - not yet implemented for Dynamical system type :" + dsType);
  }
}

void Moreau::updateState(const unsigned int level)
{
  double h = simulationLink->getTimeStep();

  DSIterator it;
  SiconosMatrix * W;
  double theta;
  for (it = OSIDynamicalSystems.begin(); it != OSIDynamicalSystems.end(); ++it)
  {
    DynamicalSystem* ds = *it;
    W = WMap[ds];
    theta = thetaMap[ds];
    // Get the DS type

    std::string dsType = ds->getType();

    if ((dsType == LNLDS) || (dsType == LLTIDS))
    {
      // get dynamical system
      LagrangianDS* d = static_cast<LagrangianDS*>(ds);
      // get velocity free, p, velocity and q pointers
      SiconosVector *vfree = d->getVelocityFreePtr();
      SiconosVector *p = d->getPPtr(level);
      SiconosVector *v = d->getVelocityPtr();
      SiconosVector *q = d->getQPtr();
      // Save value of q and v in stateTmp for future convergence computation
      if (dsType == LNLDS)
        ds->addWorkVector(q, "LagNLDSMoreau");
      // Compute velocity
      *v = *p;
      W->PLUForwardBackwardInPlace(*v);
      *v += *vfree;
      // Compute q
      //  -> get previous time step state
      SiconosVector *vold = d->getVelocityMemoryPtr()->getSiconosVector(0);
      SiconosVector *qold = d->getQMemoryPtr()->getSiconosVector(0);
      *q = *qold + h * (theta * *v + (1.0 - theta)* *vold);
      // set reaction to zero
      p->zero();
      // --- Update W for general Lagrangian system
      if (dsType == LNLDS)
      {
        double t = simulationLink->getNextTime();
        computeW(t, ds);
      }
      // Remark: for Linear system, W is already saved in object member w
    }
    else if (dsType == FOLDS || dsType == FOLTIDS)
    {
      FirstOrderNonLinearDS * fonlds = static_cast<FirstOrderNonLinearDS*>(ds);
      SiconosVector* x = ds->getXPtr();
      SiconosVector* xFree = fonlds->getXFreePtr();

      *x = h * theta * *fonlds->getRPtr();
      W->PLUForwardBackwardInPlace(*x);
      *x += *xFree;
    }
    else RuntimeException::selfThrow("Moreau::updateState - not yet implemented for Dynamical system type: " + dsType);
    // Remark: for Linear system, W is already saved in object member w
  }
}


void Moreau::display()
{
  OneStepIntegrator::display();

  cout << "====== Moreau OSI display ======" << endl;
  DSIterator it;
  for (it = OSIDynamicalSystems.begin(); it != OSIDynamicalSystems.end(); ++it)
  {
    cout << "--------------------------------" << endl;
    cout << "--> W of dynamical system number " << (*it)->getNumber() << ": " << endl;
    if (WMap[*it] != NULL) WMap[*it]->display();
    else cout << "-> NULL" << endl;
    cout << "--> and corresponding theta is: " << thetaMap[*it] << endl;
  }
  cout << "================================" << endl;
}

void Moreau::saveWToXML()
{
  //   if(integratorXml != NULL)
  //     {
  //       (static_cast<MoreauXML*>(integratorXml))->setW(W);
  //     }
  //   else RuntimeException::selfThrow("Moreau::saveIntegratorToXML - IntegratorXML object not exists");
  RuntimeException::selfThrow("Moreau::saveWToXML -  not yet implemented.");
}

Moreau* Moreau::convert(OneStepIntegrator* osi)
{
  Moreau* moreau = dynamic_cast<Moreau*>(osi);
  return moreau;
}
