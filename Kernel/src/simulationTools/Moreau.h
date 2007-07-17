/* Siconos-Kernel version 2.1.1, Copyright INRIA 2005-2006.
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
/*! \file
  Moreau Time-Integrator for Dynamical Systems
*/

#ifndef MOREAU_H
#define MOREAU_H

#include "OneStepIntegrator.h"
#include "SimpleMatrix.h"

class Simulation;
class MoreauXML;
class SiconosMatrix;

const unsigned int MOREAUSTEPSINMEMORY = 1;
/** map of SiconosMatrix; key = the related DS*/
typedef std::map<DynamicalSystem*, SiconosMatrix*> MapOfMatrices;

/** map of SiconosMatrix; key = the related DS*/
typedef std::map<DynamicalSystem*, bool> MapOfBool;

/** map of double; key = the related DS */
typedef std::map<DynamicalSystem*, double> MapOfDouble;

/** iterator through a map of matrices */
typedef MapOfMatrices::iterator matIterator;

/** iterator through a map of double */
typedef MapOfDouble::iterator doubleIterator;

/**  Moreau Time-Integrator for Dynamical Systems
 *
 *  \author SICONOS Development Team - copyright INRIA
 *  \version 2.1.1.
 *  \date (Creation) Apr 26, 2004
 *
 */
class Moreau : public OneStepIntegrator
{
private:

  /** Stl map that associates a W Moreau matrix to each DynamicalSystem of the OSI */
  MapOfMatrices WMap;

  /** Stl map that associates a theta parameter for the integration scheme to each DynamicalSystem of the OSI */
  MapOfDouble thetaMap;

  /** Stl map that associates a bool to each DynamicalSystem of the OSI - If true, then W has been allocated inside the class.*/
  MapOfBool isWAllocatedInMap;

  /** Default constructor
   */
  Moreau();

public:

  /** constructor from xml file
   *  \param OneStepIntegratorXML* : the XML object corresponding
   *  \param Simulation * : the simulation that owns the osi
   */
  Moreau(OneStepIntegratorXML*, Simulation*);

  /** constructor from a minimum set of data: one DS and its theta
   *  \param DynamicalSystem* : the DynamicalSystem linked to the OneStepIntegrator
   *  \param Theta value
   *  \param Simulation * : the simulation that owns the osi
   */
  Moreau(DynamicalSystem*, const double, Simulation*);

  /** constructor from a minimum set of data
   *  \param DynamicalSystemsSet : the list of DynamicalSystems to be integrated
   *  \param theta value for all these DS.
   *  \param Simulation * : the simulation that owns the osi
   */
  Moreau(DynamicalSystemsSet&, const double, Simulation*);

  /** constructor from a minimum set of data
   *  \param DynamicalSystemsSet : the list of DynamicalSystems to be integrated
   *  \param Map of theta values for the DS.
   *  \param Simulation * : the simulation that owns the osi
   */
  Moreau(DynamicalSystemsSet&, const MapOfDouble&, Simulation*);

  /** destructor
   */
  ~Moreau();

  // --- GETTERS/SETTERS ---
  // -- W --

  /** get W map
   *  \return a MapOfMatrices
   */
  inline MapOfMatrices getWMap() const
  {
    return WMap;
  };

  /** get isWAllocatedIn map
   *  \return a MapOfBool
   */
  inline MapOfBool getIsWAllocatedInMap() const
  {
    return isWAllocatedInMap;
  };

  /** set W map to newMap
   *  \param a MapOfMatrices
   */
  void setWMap(const MapOfMatrices&);

  /** get the value of W corresponding to DynamicalSystem ds
   * \param a pointer to DynamicalSystem, optional, default = NULL. get W[0] in that case
   *  \return SimpleMatrix
   */
  const SimpleMatrix getW(DynamicalSystem* = NULL);

  /** get W corresponding to DynamicalSystem ds
   * \param a pointer to DynamicalSystem, optional, default = NULL. get W[0] in that case
   * \return pointer to a SiconosMatrix
   */
  SiconosMatrix* getWPtr(DynamicalSystem* ds);

  /** set the value of W[ds] to newValue
   * \param SiconosMatrix newValue
   * \param a pointer to DynamicalSystem,
   */
  void setW(const SiconosMatrix&, DynamicalSystem*);

  /** set W[ds] to pointer newPtr
   * \param SiconosMatrix * newPtr
   * \param a pointer to DynamicalSystem
   */
  void setWPtr(SiconosMatrix *newPtr, DynamicalSystem*);

  // -- theta --

  /** get theta map
   *  \return a MapOfDouble
   */
  inline MapOfDouble getThetaMap() const
  {
    return thetaMap;
  };

  /** set theta map
   *  \param a MapOfDouble
   */
  void setThetaMap(const MapOfDouble&);

  /** get thetaMap[ds]
   *  \param a DynamicalSystem
   *  \return a double
   */
  const double getTheta(DynamicalSystem*);

  /** set the value of thetaMap[ds]
   *  \param a double
   *  \param a DynamicalSystem
   */
  void setTheta(const double, DynamicalSystem*);

  // --- OTHER FUNCTIONS ---

  /** initialization of the Moreau integrator; for linear time invariant systems, we compute time invariant operator (example : W)
   */
  void initialize();

  /** compute WMap[ds] Moreau matrix at time t
   *  \param the time (double)
   *  \param a pointer to DynamicalSystem
   */
  void computeW(const double, DynamicalSystem*);

  /** integrates the Dynamical System linked to this integrator without boring the constraints
   */
  void computeFreeState();

  /** integrate the system, between tinit and tend (->iout=true), with possible stop at tout (->iout=false)
   *  \param double: tinit, initial time
   *  \param double: tend, end time
   *  \param double: tout, real end time
   *  \param int: useless flag (for Moreau, used in Lsodar)
   */
  void integrate(double&, double&, double&, int&);

  /** updates the state of the Dynamical Systems
   *  \param unsigned int: level of interest for the dynamics: not used at the time
   */
  void updateState(const unsigned int);

  /** copy the matrix W of the OneStepNSProblem to the XML tree
   *  \exception RuntimeException
   */
  void saveWToXML();

  /** Displays the data of the Moreau's integrator
   */
  void display();

  /** encapsulates an operation of dynamic casting. Needed by Python interface.
   *  \param OneStepIntegrator* : the integrator which must be converted
   * \return a pointer on the integrator if it is of the right type, 0 otherwise
   */
  static Moreau* convert(OneStepIntegrator* osi);

};

#endif // MOREAU_H
