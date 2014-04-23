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
#include "FirstOrderType1R.hpp"
#include "Interaction.hpp"
#include "FirstOrderNonLinearDS.hpp"

#include "BlockVector.hpp"


FirstOrderType1R::FirstOrderType1R(const std::string& pluginh, const std::string& pluging):
  FirstOrderR(RELATION::Type1R)
{
  // Size vector of pointers to functions.
  // Connect input and output to plug-in
  setComputehFunction(SSLH::getPluginName(pluginh), SSLH::getPluginFunctionName(pluginh));
  setComputegFunction(SSLH::getPluginName(pluging), SSLH::getPluginFunctionName(pluging));
  // The jacobians are not set, and thus considered as null matrices at this point.
}

FirstOrderType1R::FirstOrderType1R(const std::string& pluginh, const std::string& pluging, const std::string& pluginJachx, const std::string& pluginJacglambda):
  FirstOrderR(RELATION::Type1R)
{
  // Size vector of pointers to functions.
  // Connect input and output to plug-in
  setComputehFunction(SSLH::getPluginName(pluginh), SSLH::getPluginFunctionName(pluginh));
  setComputegFunction(SSLH::getPluginName(pluging), SSLH::getPluginFunctionName(pluging));
  setComputeJachxFunction(SSLH::getPluginName(pluginJachx), SSLH::getPluginFunctionName(pluginJachx));
  setComputeJacglambdaFunction(SSLH::getPluginName(pluginJacglambda), SSLH::getPluginFunctionName(pluginJacglambda));
}

void FirstOrderType1R::initComponents(Interaction& inter, VectorOfBlockVectors& DSlink, VectorOfVectors& workV, VectorOfSMatrices& workM)
{

  // Check if an Interaction is connected to the Relation.
  unsigned int sizeY = inter.getSizeOfY();
  unsigned int sizeDS = inter.getSizeOfDS();
  unsigned int sizeZ = DSlink[FirstOrderR::z]->size();


  workV.resize(FirstOrderR::workVecSize);
  workV[FirstOrderR::vec_z].reset(new SiconosVector(sizeZ));
  workV[FirstOrderR::vec_x].reset(new SiconosVector(sizeDS));
  workV[FirstOrderR::vec_r].reset(new SiconosVector(sizeDS));

  workM.resize(FirstOrderR::mat_workMatSize);

  if (!_C)
    workM[FirstOrderR::mat_C].reset(new SimpleMatrix(sizeY, sizeDS));
  if (!_D)
    workM[FirstOrderR::mat_D].reset(new SimpleMatrix(sizeY, sizeY));
  if (!_F)
    workM[FirstOrderR::mat_F].reset(new SimpleMatrix(sizeY, sizeZ));
  if (!_B)
    workM[FirstOrderR::mat_B].reset(new SimpleMatrix(sizeDS, sizeY));
}

void FirstOrderType1R::computeh(double time, SiconosVector& x, SiconosVector& z, SiconosVector& y)
{
  assert(_pluginh && "FirstOrderType1R::computeOutput() is not linked to a plugin function");

  ((Type1Ptr)(_pluginh->fPtr))(x.size(), &(x)(0), y.size(), &(y)(0), z.size(), &(z)(0));

}

void FirstOrderType1R::computeg(double time, SiconosVector& lambda, SiconosVector& z, SiconosVector& r)
{
  assert(_pluging && "FirstOrderType1R::computeInput() is not linked to a plugin function");

  ((Type1Ptr)(_pluging->fPtr))(lambda.size(), &(lambda)(0), r.size(), &(r)(0), z.size(), &(z)(0));

}

void FirstOrderType1R::computeOutput(double time, Interaction& inter, VectorOfBlockVectors& DSlink, VectorOfVectors& workV, VectorOfSMatrices& workM, SiconosMatrix& osnsM, unsigned int level)
{
  SiconosVector& y = *inter.y(0);
  // Warning: temporary method to have contiguous values in memory, copy of block to simple.

  SiconosVector& workX = *workV[FirstOrderR::vec_x];
  workX = *DSlink[FirstOrderR::x];
  SiconosVector& workZ = *workV[FirstOrderR::vec_z];
  workZ = *DSlink[FirstOrderR::z];

  computeh(time, workX, workZ, y);

  *DSlink[FirstOrderR::z] = workZ;
}

void FirstOrderType1R::computeInput(double time, Interaction& inter, VectorOfBlockVectors& DSlink, VectorOfVectors& workV, VectorOfSMatrices& workM, SiconosMatrix& osnsM, unsigned int level)
{
  assert(_pluging && "FirstOrderType1R::computeInput() is not linked to a plugin function");

  SiconosVector& lambda = *inter.lambda(level);
  // Warning: temporary method to have contiguous values in memory, copy of block to simple.

  SiconosVector& workR = *workV[FirstOrderR::vec_r];
  workR = *DSlink[FirstOrderR::r];
  SiconosVector& workZ = *workV[FirstOrderR::vec_z];
  workZ = *DSlink[FirstOrderR::z];

  computeg(time, lambda, workZ, workR);

  *DSlink[FirstOrderR::r] = workR;
  *DSlink[FirstOrderR::z] = workZ;
}

void FirstOrderType1R::computeJachx(double time, SiconosVector& x, SiconosVector& z, SimpleMatrix& C)
{
  //
  assert(_pluginJachx && "FirstOrderType1R::computeJacobianH() failed; not linked to a plug-in function.");

  ((Type1Ptr)(_pluginJachx->fPtr))(x.size(), &(x)(0), C.size(0), C.getArray(), z.size(), &(z)(0));

}

void FirstOrderType1R::computeJachz(double time, SiconosVector& x, SiconosVector& z, SimpleMatrix& D)
{
  if (_pluginJachz && _pluginJachz->fPtr)
    ((Type1Ptr)(_pluginJachz->fPtr))(x.size(), &(x)(0), D.size(0), D.getArray(), z.size(), &(z)(0));

}

void FirstOrderType1R::computeJacglambda(double time, SiconosVector& lambda, SiconosVector& z, SimpleMatrix& B)
{
  assert(_pluginJacLg && "FirstOrderType1R::computeJacobiang() failed; not linked to a plug-in function.");

  ((Type1Ptr)(_pluginJacLg->fPtr))(lambda.size(), &(lambda)(0), B.size(0), B.getArray(), z.size(), &(z)(0));
}

void FirstOrderType1R::computeJach(double time, Interaction& inter,VectorOfBlockVectors& DSlink, VectorOfVectors& workV, VectorOfSMatrices& workM)
{
  SiconosVector& x = *workV[FirstOrderR::vec_x];
  x = *DSlink[FirstOrderR::x];
  SiconosVector& z = *workV[FirstOrderR::vec_z];
  z = *DSlink[FirstOrderR::z];
  if (!_C)
  {
    computeJachx(time, x, z, *workM[FirstOrderR::mat_C]);
  }
  if (!_F)
  {
    computeJachz(time, x, z, *workM[FirstOrderR::mat_F]);
  }
  *DSlink[FirstOrderR::z] = z;
}

void FirstOrderType1R::computeJacg(double time, Interaction& inter,VectorOfBlockVectors& DSlink, VectorOfVectors& workV, VectorOfSMatrices& workM)
{
  SiconosVector& z = *workV[FirstOrderR::vec_z];
  z = *DSlink[FirstOrderR::z];
  if (!_B)
  {
    computeJacglambda(time, *inter.lambda(0), z, *workM[FirstOrderR::mat_B]);
  }
  *DSlink[FirstOrderR::z] = z;
}
