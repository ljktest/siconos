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
/*! \file LagrangianRheonomousR.hpp

 */
#ifndef LagrangianRheonomousR_H
#define LagrangianRheonomousR_H

#include "LagrangianR.hpp"

/** \class LagrangianRheonomousR
 *  \brief  Lagrangian (Non Linear) Relation, Rheonomous and Holonomic.
 *
 * \author SICONOS Development Team - copyright INRIA
 * \version 3.0.0.
 * \date February 28, 2007
 *
 *  This class provides tools to describe non linear relation of the type:
 *
 * \f[
 * y = h(q,t,z)
 * \f]
 *
 * \f[
 *  \dot y =  \nabla^\top_q(q,t,z)\dot q + \frac{\partial }{\partial t}h(q,t,z)
 * \f]
 * or more generally
 * \f[
 *  \dot y =  H(q,t,z)\dot q + \frac{\partial }{\partial t}h(q,t,z)
 * \f]
 * and by duality
 *
 * \f[
 * p = H^\top(q,t,z)\lambda
 * \f]
 *
 * with
 * \f[
 * G0(q,t,z) = \nabla_q h(q,t,z)
 * \f]
 *
 *  y (or its discrete approximation) is usually stored in y[0]
 *  \f$\dot y \f$(or its discrete approximation) is usually stored in y[1]
 *  higher level can used for storing higher levels of derivatives.
 *
 *
 *
 * h, H and hdot=\f$ \frac{\partial h}{\partial t}(q,t,z) \f$ are
 * connected to user-defined functions.\n G0 and h are connected to
 * plug-in functions.\n
 *
 * The plugin function to compute h(q,t,z) needs the following
 * parameters:\n
 *
 * --> sizeDS : sum of the sizes of all the DynamicalSystems involved in the interaction\n
 * --> q : pointer to the first element of q \n
 * --> time : current time \n
 * --> sizeY : size of vector y (ie of the interaction) \n
 * --> [in,out] y : pointer to the first element of y \n
 * --> sizeZ : size of vector z \n
 * --> [in,out] z: pointer to z vector(s) from DS. \n
 * Its signature must be "void userPluginH(unsigned int, double*, double, unsigned int, double*, unsigned int, double*)"\n\n
 * The plugin function to compute G0(q,t,z), gradient of h according to q, needs the following parameters: \n
 *--> sizeDS : sum of the sizes of all the DynamicalSystems involved in the interaction  \n
 *--> q : pointer to the first element of q  \n
 *--> time : current time \n
 *--> sizeY : size of vector y (ie of the intercation) \n
 *--> [in,out] H : pointer to the first element of G0 (sizeY X sizeDS matrix)\n
 * --> sizeZ : size of vector z \n
 * -->[in,out] z: pointer to z vector(s) from DS.\n
 * Its signature must be "void userPluginG0(unsigned int, double*, double, unsigned int, double*, unsigned int, double*)"\n\n
 * The plugin function to compute hdot(q,t,z), needs the following parameters: \n
 *--> sizeDS : sum of the sizes of all the DynamicalSystems involved in the interaction  \n
 *--> q : pointer to the first element of q  \n
 *--> time : current time \n
 *--> sizeY : size of vector y (ie of the intercation) \n
 *--> [in,out] hDot : pointer to the first element of hDot.\n
 * --> sizeZ : size of vector z \n
 * -->[in,out] z: pointer to z vector(s) from DS.\n
 * Its signature must be "void userPluginG0(unsigned int, double*, double, unsigned int, double*, unsigned int, double*)"\n\n
 *
 */
class LagrangianRheonomousR : public LagrangianR
{

protected:
  /** serialization hooks
  */
  ACCEPT_SERIALIZATION(LagrangianRheonomousR);

  /** plugged vector used to compute hDot */
  SP::SiconosVector _hDot;

  /** LagrangianRheonomousR plug-in to compute h(q,t,z)
  * @param sizeDS : sum of the sizes of all the DynamicalSystems involved in the interaction
  * @param q : pointer to the first element of q
  * @param time : current time
  * @param sizeY : size of vector y (ie of the intercation)
  * @param[in,out] y : pointer to the first element of y
  * @param sizeZ : size of vector z
  * @param[in,out] z : a vector of user-defined parameters
  */
  //  SP::PluggedObject _pluginjqh
  //  FPtr4 hPtr;

  /** LagrangianRheonomousR plug-in to compute hDot(q,t,z)
  * @param sizeDS : sum of the sizes of all the DynamicalSystems involved in the interaction
  * @param q : pointer to the first element of q
  * @param time : current time
  * @param sizeY : size of vector hDot (ie of the intercation)
  * @param[in,out] pointer to the first element of hDot
  * @param sizeZ : size of vector z
  * @param[in,out] z : a vector of user-defined parameters
  */
  SP::PluggedObject _pluginhDot;
  //  FPtr4 hDotPtr;

  /** LagrangianRheonomousR plug-in to compute G0(q,t,z), gradient of h accoring to q
  * @param sizeDS : sum of the sizes of all the DynamicalSystems involved in the interaction
  * @param q : pointer to the first element of q
  * @param time : current time
  * @param sizeY : size of vector y (ie of the intercation)
  * @param[in,out] G0 : pointer to the first element of G0
  * @param sizeZ : size of vector z
  * @param[in,out] z : a vector of user-defined parameters
  */
  //  FPtr4 computeJachqPtr;




  /** initialize G matrices or components specific to derived classes.
  */
  void initComponents(Interaction& inter);

  /** default constructor
  */
  LagrangianRheonomousR(): LagrangianR(RELATION::RheonomousR)
  {
    zeroPlugin();
  };
  void zeroPlugin();
public:

  /** constructor from a set of data
  *  \param pluginh name of the plugin to compute h.\n
  * Its signature must be "void userPluginH(unsigned int, double*, double, unsigned int, double*, unsigned int, double*)"
  *  \param pluginJacobianhq name of the plugin  to compute jacobian h according to q.\n
  * Its signature must be "void userPluginG0(unsigned int, double*, double, unsigned int, double*, unsigned int, double*)"
  *  \param pluginDoth name of the plugin to compute hDot. \n
  * Its signature must be "void userPluginHDot(unsigned int, double*, double, unsigned int, double*, unsigned int, double*)
  */
  LagrangianRheonomousR(const std::string& pluginh, const std::string& pluginJacobianhq, const std::string& pluginDoth);

  /** destructor
  */
  virtual ~LagrangianRheonomousR() {};

  // -- hDot --

  /** get vector hDot
  *  \return a SiconosVector

  inline const SiconosVector gethDot() const { return *hDot; }
  */

  /** get a pointer on vector hDot
  *  \return a smart pointer on a SiconosVector
  */
  inline SP::SiconosVector hDot() const
  {
    return _hDot;
  }

  /** To get the name of hDot plugin
  *  \return a std::string
  */
  const std::string gethDotName() const ;

  const std::string getJachqName() const ;

  /** true if hDot is plugged
  *  \return a bool
  */

  /** to set a specified function to compute function hDot
  *  \param std::string : the complete path to the plugin
  *  \param std::string : the name of the function to use in this plugin
  */
  void setComputehDotFunction(const std::string& , const std::string&);
  //  virtual void setComputehFunction(const std::string& pluginPath, const std::string& functionName);


  /** Compute y = h(t,q,z) using plug-in mechanism with the data vector of the interaction
   * should be used as less as possible to avoid side--effects
   * prefer computeh(double time, Interaction& inter, SP::BlockVector q, SP::BlockVector z)
   * \param time  current time
   * \param inter interaction that owns the relation
   */
  virtual void computeh(double time, Interaction& inter);

  /** to compute y = h(t,q,z) using plug-in mechanism
  * \param time current time
  * \param inter interaction that owns the relation
  * \param q the BlockVector of coordinates
  * \param z the BlockVector of parameters
  */
  virtual void computeh(double time, Interaction& inter, SP::BlockVector q, SP::BlockVector z);

  /** to compute hDot using plug-in mechanism
   * using plug-in mechanism with the data vector of the interaction
   * should be used as less as possible to avoid side--effects
   * prefer computehDot(double time, Interaction& inter, SP::BlockVector q, SP::BlockVector z)
   * \param time  current time
   * \param inter interaction that owns the relation
   */
  virtual void computehDot(double time, Interaction& inter);

  /** to compute hDot using plug-in mechanism
   * \param time current time
   * \param inter interaction that owns the relation
   * \param q the BlockVector of coordinates
   * \param z the BlockVector of parameters
   */
  virtual void computehDot(double time, Interaction& inter, SP::BlockVector q, SP::BlockVector z);

  /** to compute the jacobian of h using plug-in mechanism. Index shows which jacobian is computed
  * \param: double, current time
  * \param: unsigned int
  */
  virtual void computeJachq(double time, Interaction& inter);

  void computeJachqDot(double time, Interaction& inter)
  {
    /* \warning. This method should never be called, since we are only considering
     * rheonomic constraint
     */
    assert(0) ;
  }

  /* compute all the H Jacobian */
  void computeJach(double time, Interaction& inter)
  {
    computeJachq(time, inter);
    // computeJachqDot(time, inter);
    computeDotJachq(time, inter);
    // computeJachlambda(time, inter);
    computehDot(time,inter);
  }
  /* compute all the G Jacobian */
  void computeJacg(double time, Interaction& inter)
  {
    computeJacgq(time, inter);
    // computeJacgqDot(time, inter);
    // computeJacglambda(time, inter);
  }


  /** to compute output
  *  \param double : current time
  *  \param unsigned int: number of the derivative to compute, optional, default = 0.
  */
  void computeOutput(double time, Interaction& inter, unsigned int = 0);

  /** to compute p
  *  \param double : current time
  *  \param unsigned int: "derivative" order of lambda used to compute input
  */
  void computeInput(double time, Interaction& inter, unsigned int = 0);

  ACCEPT_STD_VISITORS();

};

TYPEDEF_SPTR(LagrangianRheonomousR)

#endif
