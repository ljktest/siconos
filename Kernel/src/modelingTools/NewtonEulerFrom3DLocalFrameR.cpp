/* Siconos-Kernel, Copyright INRIA 2005-2012.
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


#include "NewtonEulerFrom3DLocalFrameR.hpp"
#include "NewtonEulerDS.hpp"
#include <boost/math/quaternion.hpp>
#include "Interaction.hpp"

//#define NEFC3D_DEBUG
/*
See devNotes.pdf for details. A detailed documentation is available in DevNotes.pdf: chapter 'NewtonEulerR: computation of \nabla q H'. Subsection 'Case FC3D: using the local frame local velocities'
*/
void NewtonEulerFrom3DLocalFrameR::initComponents(Interaction& inter)
{
  NewtonEulerFrom1DLocalFrameR::initComponents(inter);
  unsigned int qSize = 7 * (inter.getSizeOfDS() / 6);
  /*keep only the distance.*/
  _jachq.reset(new SimpleMatrix(3, qSize));
  _Mabs_C.reset(new SimpleMatrix(3, 3));
  _AUX2.reset(new SimpleMatrix(3, 3));
  //  _isContact=1;
}
void NewtonEulerFrom3DLocalFrameR::FC3DcomputeJachqTFromContacts(SP::NewtonEulerDS d1)
{

  double Nx = _Nc->getValue(0);
  double Ny = _Nc->getValue(1);
  double Nz = _Nc->getValue(2);
  double Px = _Pc1->getValue(0);
  double Py = _Pc1->getValue(1);
  double Pz = _Pc1->getValue(2);
  double G1x = d1->q()->getValue(0);
  double G1y = d1->q()->getValue(1);
  double G1z = d1->q()->getValue(2);

  assert(_Nc->norm2() >0.0 && "NewtonEulerFrom3DLocalFrameR::FC3DcomputeJachqTFromContacts. Normal vector not consitent ") ;


#ifdef NEFC3D_DEBUG
  printf("contact normal:\n");
  _Nc->display();
  printf("contact point :\n");
  _Pc1->display();
  printf("center of mass :\n");
  d1->q()->display();
#endif
  double t[6];
  double * pt = t;
  orthoBaseFromVector(&Nx, &Ny, &Nz, pt, pt + 1, pt + 2, pt + 3, pt + 4, pt + 5);
  pt = t;
  _Mabs_C->setValue(0, 0, Nx);
  _Mabs_C->setValue(1, 0, *pt);
  _Mabs_C->setValue(2, 0, *(pt + 3));
  _Mabs_C->setValue(0, 1, Ny);
  _Mabs_C->setValue(1, 1, *(pt + 1));
  _Mabs_C->setValue(2, 1, *(pt + 4));
  _Mabs_C->setValue(0, 2, Nz);
  _Mabs_C->setValue(1, 2, *(pt + 2));
  _Mabs_C->setValue(2, 2, *(pt + 5));
#ifdef NEFC3D_DEBUG
  printf("_Mabs_C:\n");
  _Mabs_C->display();
#endif
  _NPG1->zero();

  (*_NPG1)(0, 0) = 0;
  (*_NPG1)(0, 1) = -(G1z - Pz);
  (*_NPG1)(0, 2) = (G1y - Py);
  (*_NPG1)(1, 0) = (G1z - Pz);
  (*_NPG1)(1, 1) = 0;
  (*_NPG1)(1, 2) = -(G1x - Px);
  (*_NPG1)(2, 0) = -(G1y - Py);
  (*_NPG1)(2, 1) = (G1x - Px);
  (*_NPG1)(2, 2) = 0;

  d1->updateMObjToAbs();
  SimpleMatrix& Mobj1_abs = *d1->MObjToAbs();



#ifdef NEFC3D_DEBUG
  printf("NewtonEulerFrom3DLocalFrameR::FC3DcomputeJachqTFromContacts, Mobj1_abs:");
  Mobj1_abs.display();
#endif


  prod(*_NPG1, Mobj1_abs, *_AUX1, true);
#ifdef NEFC3D_DEBUG
  printf("NewtonEulerFrom3DLocalFrameR::FC3DcomputeJachqTFromContacts, Mobj1_abs:");
  Mobj1_abs.display();
  printf("NewtonEulerFrom3DLocalFrameR::FC3DcomputeJachqTFromContacts, AUX1:");
  _AUX1->display();
  printf("NewtonEulerFrom3DLocalFrameR::FC3DcomputeJachqTFromContacts, AUX2:");
  _AUX2->display();
#endif

  prod(*_Mabs_C, *_AUX1, *_AUX2, true);
#ifdef NEFC3D_DEBUG
  printf("NewtonEulerFrom3DLocalFrameR::FC3DcomputeJachqTFromContacts, Mabs_C:");
  _Mabs_C->display();
  printf("NewtonEulerFrom3DLocalFrameR::FC3DcomputeJachqTFromContacts, AUX1:");
  _AUX1->display();
  printf("NewtonEulerFrom3DLocalFrameR::FC3DcomputeJachqTFromContacts, AUX2:");
  _AUX2->display();
#endif


  for (unsigned int ii = 0; ii < 3; ii++)
    for (unsigned int jj = 0; jj < 3; jj++)
      _jachqT->setValue(ii, jj, _Mabs_C->getValue(ii, jj));

  for (unsigned int ii = 0; ii < 3; ii++)
    for (unsigned int jj = 3; jj < 6; jj++)
      _jachqT->setValue(ii, jj, _AUX2->getValue(ii, jj - 3));
#ifdef NEFC3D_DEBUG
  printf("NewtonEulerFrom3DLocalFrameR::FC3DcomputeJachqTFromContacts, _jahcqT:\n");
  _jachqT->display();
  SP::SimpleMatrix jaux(new SimpleMatrix(*_jachqT));
  jaux->trans();
  SP::SiconosVector v(new SiconosVector(3));
  SP::SiconosVector vRes(new SiconosVector(6));
  v->zero();
  v->setValue(0, 1);
  prod(*jaux, *v, *vRes, true);
  vRes->display();
  v->zero();
  v->setValue(1, 1);
  prod(*jaux, *v, *vRes, true);
  vRes->display();
  v->zero();
  v->setValue(2, 1);
  prod(*jaux, *v, *vRes, true);
  vRes->display();
#endif
}

void NewtonEulerFrom3DLocalFrameR::FC3DcomputeJachqTFromContacts(SP::NewtonEulerDS d1, SP::NewtonEulerDS d2)
{
  double Nx = _Nc->getValue(0);
  double Ny = _Nc->getValue(1);
  double Nz = _Nc->getValue(2);
  double Px = _Pc1->getValue(0);
  double Py = _Pc1->getValue(1);
  double Pz = _Pc1->getValue(2);
  double G1x = d1->q()->getValue(0);
  double G1y = d1->q()->getValue(1);
  double G1z = d1->q()->getValue(2);
  double G2x = d2->q()->getValue(0);
  double G2y = d2->q()->getValue(1);
  double G2z = d2->q()->getValue(2);

  double t[6];
  double * pt = t;
  orthoBaseFromVector(&Nx, &Ny, &Nz, pt, pt + 1, pt + 2, pt + 3, pt + 4, pt + 5);
  pt = t;
  _Mabs_C->setValue(0, 0, Nx);
  _Mabs_C->setValue(1, 0, *pt);
  _Mabs_C->setValue(2, 0, *(pt + 3));
  _Mabs_C->setValue(0, 1, Ny);
  _Mabs_C->setValue(1, 1, *(pt + 1));
  _Mabs_C->setValue(2, 1, *(pt + 4));
  _Mabs_C->setValue(0, 2, Nz);
  _Mabs_C->setValue(1, 2, *(pt + 2));
  _Mabs_C->setValue(2, 2, *(pt + 5));

  _NPG1->zero();

  (*_NPG1)(0, 0) = 0;
  (*_NPG1)(0, 1) = -(G1z - Pz);
  (*_NPG1)(0, 2) = (G1y - Py);
  (*_NPG1)(1, 0) = (G1z - Pz);
  (*_NPG1)(1, 1) = 0;
  (*_NPG1)(1, 2) = -(G1x - Px);
  (*_NPG1)(2, 0) = -(G1y - Py);
  (*_NPG1)(2, 1) = (G1x - Px);
  (*_NPG1)(2, 2) = 0;

  _NPG2->zero();

  (*_NPG2)(0, 0) = 0;
  (*_NPG2)(0, 1) = -(G2z - Pz);
  (*_NPG2)(0, 2) = (G2y - Py);
  (*_NPG2)(1, 0) = (G2z - Pz);
  (*_NPG2)(1, 1) = 0;
  (*_NPG2)(1, 2) = -(G2x - Px);
  (*_NPG2)(2, 0) = -(G2y - Py);
  (*_NPG2)(2, 1) = (G2x - Px);
  (*_NPG2)(2, 2) = 0;


  d1->updateMObjToAbs();
  SimpleMatrix& Mobj1_abs = *d1->MObjToAbs();


  prod(*_NPG1, Mobj1_abs, *_AUX1, true);
  prod(*_Mabs_C, *_AUX1, *_AUX2, true);


  for (unsigned int ii = 0; ii < 3; ii++)
    for (unsigned int jj = 0; jj < 3; jj++)
      _jachqT->setValue(ii, jj, _Mabs_C->getValue(ii, jj));

  for (unsigned int ii = 0; ii < 3; ii++)
    for (unsigned int jj = 3; jj < 6; jj++)
      _jachqT->setValue(ii, jj, _AUX2->getValue(ii, jj - 3));

  d2->updateMObjToAbs();
  SimpleMatrix& Mobj2_abs = *d2->MObjToAbs();

  prod(*_NPG2, Mobj2_abs, *_AUX1, true);
  prod(*_Mabs_C, *_AUX1, *_AUX2, true);

  for (unsigned int ii = 0; ii < 3; ii++)
    for (unsigned int jj = 0; jj < 3; jj++)
      _jachqT->setValue(ii, jj + 6, -_Mabs_C->getValue(ii, jj));

  for (unsigned int ii = 0; ii < 3; ii++)
    for (unsigned int jj = 3; jj < 6; jj++)
      _jachqT->setValue(ii, jj + 6, -_AUX2->getValue(ii, jj - 3));

}

void NewtonEulerFrom3DLocalFrameR::computeJachqT(Interaction& inter, SP::DynamicalSystem ds1, SP::DynamicalSystem ds2)
{
  SP::NewtonEulerDS d1 =  std11::static_pointer_cast<NewtonEulerDS> (ds1);
  SP::NewtonEulerDS d2 =  std11::static_pointer_cast<NewtonEulerDS> (ds2);
  if(d1 != d2)
    FC3DcomputeJachqTFromContacts(d1, d2);
  
  else
  {
    FC3DcomputeJachqTFromContacts(d1);
  }
}
