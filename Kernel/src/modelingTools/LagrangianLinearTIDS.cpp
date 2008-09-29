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
#include "LagrangianLinearTIDS.h"
#include "LagrangianLinearTIDSXML.h"
#include "BlockMatrix.h"

using namespace std;

// --- Default (private) constructor ---
LagrangianLinearTIDS::LagrangianLinearTIDS(): LagrangianDS()
#ifndef WithSmartPtr
  , K(NULL), C(NULL)
#endif

{
#ifndef WithSmartPtr
  isAllocatedIn["C"] = false;
  isAllocatedIn["K"] = false;
#endif
  DSType = LLTIDS;
}

// --- Constructor from an xml file (newNsds is optional) ---
LagrangianLinearTIDS::LagrangianLinearTIDS(DynamicalSystemXMLSPtr dsXML,  SP::NonSmoothDynamicalSystem newNsds):
  LagrangianDS(dsXML, newNsds)
{
  LagrangianLinearTIDSXMLSPtr lltidsxml = boost::static_pointer_cast <LagrangianLinearTIDSXML>(dsxml);

  // If Fint or NNL is given: ignored.
  if (lltidsxml->hasFInt() ||  lltidsxml->hasNNL())
    cout << "!!!!! Warning : LagrangianLinearTIDS: xml constructor, Fint or NNL input will be ignored in xml file." << endl;

  // K and C

  if (lltidsxml->hasK())
  {
    K.reset(new SimpleMatrix(lltidsxml->getK()));
  }
  if (lltidsxml->hasC())
  {
    C.reset(new SimpleMatrix(lltidsxml->getC()));
  }
}

// --- Constructor from a set of data - Mass, K and C ---
LagrangianLinearTIDS::LagrangianLinearTIDS(int newNumber, const SimpleVector& newQ0, const SimpleVector& newVelocity0,
    const SiconosMatrix& newMass, const SiconosMatrix& newK, const SiconosMatrix& newC):
  LagrangianDS(newNumber, newQ0, newVelocity0, newMass)
{
  assert((newK.size(0) == ndof && newK.size(1) == ndof) &&
         "LagrangianLinearTIDS - constructor from data, inconsistent size between K and ndof");

  assert((newC.size(0) == ndof && newC.size(1) == ndof) &&
         "LagrangianLinearTIDS - constructor from data, inconsistent size between C and ndof");

  K.reset(new SimpleMatrix(newK));
  C.reset(new SimpleMatrix(newC));

  DSType = LLTIDS;
}

// --- Constructor from a set of data - Mass, no K and no C ---
LagrangianLinearTIDS::LagrangianLinearTIDS(int newNumber, const SimpleVector& newQ0, const SimpleVector& newVelocity0,
    const SiconosMatrix& newMass):
  LagrangianDS(newNumber, newQ0, newVelocity0, newMass)
{
  DSType = LLTIDS;

}

LagrangianLinearTIDS::~LagrangianLinearTIDS()
{
}


bool LagrangianLinearTIDS::checkDynamicalSystem()
{
  bool output = true;
  // ndof
  if (ndof == 0)
  {
    RuntimeException::selfThrow("LagrangianLinearTIDS::checkDynamicalSystem - number of degrees of freedom is equal to 0.");
    output = false;
  }

  // q0 and velocity0 != NULL
  if (! q0 || ! velocity0)
  {
    RuntimeException::selfThrow("LagrangianLinearTIDS::checkDynamicalSystem - initial conditions are badly set.");
    output = false;
  }

  // Mass
  if (! mass)
  {
    RuntimeException::selfThrow("LagrangianLinearTIDS::checkDynamicalSystem - Mass is not set.");
    output = false;
  }

  // fInt, NNL and their Jacobian
  if (isPlugin["fInt"] || isPlugin["jacobianQFInt"] || isPlugin["jacobianVelocityFInt"])
    // ie if fInt is defined and not constant => its Jacobian must be defined (but not necessarily plugged)
  {
    RuntimeException::selfThrow("LagrangianLinearTIDS::checkDynamicalSystem - Fint and/or their Jacobian are plugged, which should not be.");
    output = false;
  }

  if (isPlugin["NNL"] || isPlugin["jacobianQNNL"] || isPlugin["jacobianVelocityNNL"])
    // ie if fInt is defined and not constant => its Jacobian must be defined (but not necessarily plugged)
  {
    RuntimeException::selfThrow("LagrangianLinearTIDS::checkDynamicalSystem -NNl and/or their Jacobian are plugged, which should not be.");
    output = false;
  }

  if (isPlugin["f"] || isPlugin["jacobianXF"])
  {
    RuntimeException::selfThrow("LagrangianLinearTIDS::checkDynamicalSystem - f and/or its Jacobian can not be plugged for a Lagrangian system.");
    output = false;
  }
  return output;
}

void LagrangianLinearTIDS::initRhs(double time)
{
  // Copy of Mass into workMatrix for LU-factorization.

#ifndef WithSmartPtr
  workMatrix["invMass"] = new SimpleMatrix(*mass);
#else
  workMatrix["invMass"].reset(new SimpleMatrix(*mass));
#endif

  // compute x[1] (and thus fExt if required)
  computeRhs(time);

#ifndef WithSmartPtr
  workMatrix["zero-matrix"] = new SimpleMatrix(ndof, ndof, ZERO);
  workMatrix["Id-matrix"] = new SimpleMatrix(ndof, ndof, IDENTITY);
#else
  workMatrix["zero-matrix"].reset(new SimpleMatrix(ndof, ndof, ZERO));
  workMatrix["Id-matrix"].reset(new SimpleMatrix(ndof, ndof, IDENTITY));
#endif

  // jacobianXRhs
  if (K)
  {
    //  bloc10 of jacobianX is solution of Mass*Bloc10 = K

#ifndef WithSmartPtr
    workMatrix["jacobianXBloc10"] = new SimpleMatrix(-1 * *K);
    isAllocatedIn["jXb10"] = true;
#else
    workMatrix["jacobianXBloc10"].reset(new SimpleMatrix(-1 * *K));
#endif

    workMatrix["invMass"]->PLUForwardBackwardInPlace(*workMatrix["jacobianXBloc10"]);
  }
  else
    workMatrix["jacobianXBloc10"] = workMatrix["zero-matrix"] ;
  if (C)
  {
    //  bloc11 of jacobianX is solution of Mass*Bloc11 = C

#ifndef WithSmartPtr
    workMatrix["jacobianXBloc11"] = new SimpleMatrix(-1 * *C);
    isAllocatedIn["jXb11"] = true;
#else
    workMatrix["jacobianXBloc11"].reset(new SimpleMatrix(-1 * *C));
#endif

    workMatrix["invMass"]->PLUForwardBackwardInPlace(*workMatrix["jacobianXBloc11"]);
  }
  else
    workMatrix["jacobianXBloc11"] = workMatrix["zero-matrix"] ;

#ifndef WithSmartPtr
  jacobianXRhs = new BlockMatrix(workMatrix["zero-matrix"], workMatrix["Id-matrix"], workMatrix["jacobianXBloc10"], workMatrix["jacobianXBloc11"]);
  isAllocatedIn["jacobianXRhs"] = true ;
#else
  jacobianXRhs.reset(new BlockMatrix(workMatrix["zero-matrix"], workMatrix["Id-matrix"], workMatrix["jacobianXBloc10"], workMatrix["jacobianXBloc11"]));
#endif
}

void LagrangianLinearTIDS::initialize(const string& simulationType, double time, unsigned int sizeOfMemory)
{
  // Memory allocation for p[0], p[1], p[2].
  initP(simulationType);

  // set q and q[1] to q0 and velocity0, initialize acceleration.
  *q[0] = *q0;
  *q[1] = *velocity0;

  // If z is NULL (ie has not been set), we initialize it with a null vector of size 1, since z is required in plug-in functions call.
  if (! z)
  {

#ifndef WithSmartPtr
    z = new SimpleVector(1);
    isAllocatedIn["z"] = true;
#else
    z.reset(new SimpleVector(1));
#endif

  }

  // Set variables of top-class DynamicalSystem
  connectToDS(); // note that connection can not be done during constructor call, since user can complete the ds after (add plugin or anything else).

  bool res = checkDynamicalSystem();
  if (!res) cout << "Warning: your dynamical system seems to be uncomplete (check = false)" << endl;

  // Initialize memory vectors
  initMemory(sizeOfMemory);

  // rhs and its jacobian
  initRhs(time);
}

void LagrangianLinearTIDS::setK(const SiconosMatrix& newValue)
{
  if (newValue.size(0) != ndof || newValue.size(1) != ndof)
    RuntimeException::selfThrow("LagrangianLinearTIDS - setK: inconsistent input matrix size ");

  if (!K)
  {

#ifndef WithSmartPtr
    K = new SimpleMatrix(newValue);
    isAllocatedIn["K"] = true;
#else
    K.reset(new SimpleMatrix(newValue));
#endif

  }
  else
    *K = newValue;
}

void LagrangianLinearTIDS::setKPtr(SiconosMatrixSPtr newPtr)
{
  if (newPtr->size(0) != ndof || newPtr->size(1) != ndof)
    RuntimeException::selfThrow("LagrangianLinearTIDS - setKPtr: inconsistent input matrix size ");

#ifndef WithSmartPtr
  if (isAllocatedIn["K"]) delete K;
  K = newPtr;
  isAllocatedIn["K"] = false;
#else
  K = newPtr;
#endif

}

void LagrangianLinearTIDS::setC(const SiconosMatrix& newValue)
{
  if (newValue.size(0) != ndof || newValue.size(1) != ndof)
    RuntimeException::selfThrow("LagrangianLinearTIDS - setC: inconsistent input matrix size ");

  if (!C)
  {

#ifndef WithSmartPtr
    C = new SimpleMatrix(newValue);
    isAllocatedIn["C"] = true;
#else
    C.reset(new SimpleMatrix(newValue));
#endif

  }
  else
    *C = newValue;
}

void LagrangianLinearTIDS::setCPtr(SiconosMatrixSPtr newPtr)
{
  if (newPtr->size(0) != ndof || newPtr->size(1) != ndof)
    RuntimeException::selfThrow("LagrangianLinearTIDS - setCPtr: inconsistent input matrix size ");

#ifndef WithSmartPtr
  if (isAllocatedIn["C"]) delete C;
  C = newPtr;
  isAllocatedIn["C"] = false;
#else
  C = newPtr;
#endif

}

void LagrangianLinearTIDS::display() const
{
  LagrangianDS::display();
  cout << "===== Lagrangian Linear Time Invariant System display ===== " << endl;
  cout << "- Stiffness Matrix K : " << endl;
  if (K) K->display();
  else cout << "-> NULL" << endl;
  cout << "- Viscosity Matrix C : " << endl;
  if (C) C->display();
  else cout << "-> NULL" << endl;
  cout << "=========================================================== " << endl;
}

void LagrangianLinearTIDS::computeRhs(double time, bool)
{
  // second argument is useless but present because of top-class function overloading.

  *q[2] = *p[2]; // Warning: p update is done in Interactions/Relations

  if (fExt)
  {
    computeFExt(time);
    *q[2] += *fExt; // This supposes that fExt is up to date!!
  }

  if (K)
    *q[2] -= prod(*K, *q[0]);

  if (C)
    *q[2] -= prod(*C, *q[1]);

  // Then we search for q[2], such as Mass*q[2] = fExt - Cq[1] - Kq[0] + p.
  workMatrix["invMass"]->PLUForwardBackwardInPlace(*q[2]);
}

void LagrangianLinearTIDS::computeJacobianXRhs(double time, bool)
{
  // Nothing to be done since jacobianXRhs is constant and filled during initialize.
  // But this function is required, since it is called from Lsodar (if not present, the one of LagrangianDS will be called)
}

void LagrangianLinearTIDS::saveSpecificDataToXML()
{
  assert(dsxml &&
         "LagrangianLinearTIDS::saveDSToXML - object DynamicalSystemXML does not exist");

  LagrangianDSXMLSPtr lgptr = boost::static_pointer_cast <LagrangianDSXML>(dsxml);
  lgptr->setMassMatrix(*mass);
  lgptr->setQ(*q[0]);
  lgptr->setQ0(*q0);
  lgptr->setQMemory(*qMemory);
  lgptr->setVelocity(*q[1]);
  lgptr->setVelocity0(*velocity0);
  lgptr->setVelocityMemory(*velocityMemory);

  // FExt
  if (lgptr->hasFExt())
  {
    if (!lgptr->isFExtPlugin())
    {
      lgptr->setFExtVector(*fExt);
    }
  }
  else
  {
    lgptr->setFExtPlugin(pluginNames["fExt"]);
  }
  (boost::static_pointer_cast <LagrangianLinearTIDSXML>(dsxml))->setK(*K);
  (boost::static_pointer_cast <LagrangianLinearTIDSXML>(dsxml))->setC(*C);
}

LagrangianLinearTIDS* LagrangianLinearTIDS::convert(DynamicalSystem* ds)
{
  LagrangianLinearTIDS* ltids = dynamic_cast<LagrangianLinearTIDS*>(ds);
  return ltids;
}

