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
/*! \file LagrangianScleronomousR.h

*/
#ifndef LagrangianScleronomousR_H
#define LagrangianScleronomousR_H

#include "LagrangianR.hpp"

/** Lagrangian (Non Linear) Relations, Scleronomous and Holonomic.
 *
 * \author SICONOS Development Team - copyright INRIA
 *  \version 3.0.0.
 *  \date Apr 27, 2004
 *
 * Relation with:
 *
 * \f[
 * Y[0] = y = h(q,z) \\
 * \f]
 *
 * \f[
 * Y[1] = \dot y = G0(q,z)\dot q \\
 * \f]
 *
 * \f[
 * p = G0^t(q,z)\lambda
 * \f]
 *
 * with
 *
 \f[
 G0(q,z) = \nabla_q h(q,z)
 \f]

 * G0 and h are connected to plug-in functions.\n
 * The plugin function to compute h(q,z) needs the following parameters:\n
 * --> sizeQ: size of q = sum of the sizes of all the DynamicalSystems involved in the interaction\n
 * --> q : pointer to the first element of q \n
 * --> sizeY : size of vector y (ie of the interaction) \n
 * --> [in,out] y : pointer to the first element of y \n
 * --> sizeZ : size of vector z \n
 * --> [in,out] z: pointer to z vector(s) from DS. \n
 * Its signature must be "void plugin(unsigned int, const double*, unsigned int, double*, unsigned int, double*)"\n\n
 * The plugin function to compute G0(q,z), gradient of h according to q, needs the following parameters: \n
 *--> sizeQ: size of q = sum of the sizes of all the DynamicalSystems involved in the interaction  \n
 *--> q : pointer to the first element of q  \n
 *--> sizeY : size of vector y (ie of the intercation) \n
 *--> [in,out] G0 : pointer to the first element of G0 (sizeY X sizeDS matrix)\n
 * --> sizeZ : size of vector z \n
 * -->[in,out] z: pointer to z vector(s) from DS.\n
 * Its signature must be "void plugin(unsigned int, const double*, unsigned int, double*, unsigned int, double*)"\n

*
 */
class LagrangianScleronomousR : public LagrangianR
{

protected:
  /** LagrangianScleronomousR plug-in to compute h(q,z)
    * @param sizeQ: size of q = sum of the sizes of all the DynamicalSystems involved in the interaction
    * @param q : pointer to the first element of q
    * @param sizeY : size of vector y (ie of the interaction)
    * @param[in,out] y : pointer to the first element of y
    * @param sizeZ : size of vector z
    * @param[in,out] z: pointer to z vector(s) from DS.
    */
  //  FPtr3 hPtr;

  /** LagrangianScleronomousR plug-in to compute G0(q,z), gradient of h according to q
   * @param sizeQ: size of q = sum of the sizes of all the DynamicalSystems involved in the interaction
   * @param q : pointer to the first element of q
   * @param sizeY : size of vector y (ie of the intercation)
   * @param[in,out] G0 : pointer to the first element of G0 (sizeY X sizeDS matrix)
   * @param sizeZ : size of vector z
   * @param[in,out] z: pointer to z vector(s) from DS.
   */
  SP::PluggedObject pluginjqh;
  /** basic constructor
      \param the sub-type of the relation
  */
  LagrangianScleronomousR(): LagrangianR(RELATION::ScleronomousR)
  {
    ;
  }

public:

  /** constructor from xml file
   *  \param relationXML
   */
  LagrangianScleronomousR(SP::RelationXML);

  /** constructor from a set of data
   *  \param string : the name of the plugin to compute h(q,z).\n
   * The signature  of the plugged function must be:
   *  "void pluginH(unsigned int, const double*, unsigned int, double*, unsigned int, double*)"
   *  \param string : the name of the plugin to compute jacobian h according to q.\n
   * The signature  of the plugged function must be:
   *  "void pluginG0(unsigned int, const double*, unsigned int, double*, unsigned int, double*)"
   *  \exception RuntimeException
   */
  LagrangianScleronomousR(const std::string&, const std::string&);

  /** destructor
   */
  virtual ~LagrangianScleronomousR() {};

  /** to compute y = h(q,v,t) using plug-in mechanism
   * \param: double, current time
   */
  virtual void computeh(double);

  virtual void computeJacqh(double);


  /** to compute the jacobian of h using plug-in mechanism. Index shows which jacobian is computed
   * \param: double, current time
   * \param: unsigned int
   */
  //  void computeJacqh(double);
  void computeg(double, unsigned int = 0);


  /** to compute output
   *  \param double : current time
   *  \param unsigned int: number of the derivative to compute, optional, default = 0.
   */
  virtual void computeOutput(double, unsigned int = 0);

  /** to compute p
   *  \param double : current time
   *  \param unsigned int: "derivative" order of lambda used to compute input
   */
  void computeInput(double, unsigned int = 0);

  /** encapsulates an operation of dynamic casting. Needed by Python interface.
   *  \param Relation* : the relation which must be converted
   * \return a pointer on the relation if it is of the right type, NULL otherwise
   */
  static LagrangianScleronomousR* convert(Relation *r);
};

TYPEDEF_SPTR(LagrangianScleronomousR);

#endif // LAGRANGIANRELATION_H
