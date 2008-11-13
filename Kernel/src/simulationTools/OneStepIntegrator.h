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
/*! \file

Basic class to handle with dynamical system integrators over a time step.
 */

#ifndef ONESTEPINTEGRATOR_H
#define ONESTEPINTEGRATOR_H

#include "SiconosConst.h"
#include "SimulationTypeDef.hpp"
#include "DynamicalSystemsSet.hpp"
#include "InteractionsSet.hpp"
#include "OneStepIntegratorTypes.hpp"

class OneStepIntegratorXML;
class Simulation;

/**  Generic object to manage DynamicalSystem(s) time-integration
 *
 *  \author SICONOS Development Team - copyright INRIA
 *  \version 3.0.0.
 *  \date (Creation) Apr 26, 2004
 *
 * !!! This is a virtual class, interface for some specific integrators !!!
 *
 * At the time, available integrators are: Moreau and Lsodar
 *
 */
class OneStepIntegrator : public boost::enable_shared_from_this<OneStepIntegrator>
{
protected:

  /** type/name of the Integrator */
  OSI::TYPES integratorType;

  /** a set of DynamicalSystem to integrate */
  SP::DynamicalSystemsSet OSIDynamicalSystems;

  /** a set of Interactions to define a list of *
   * dynamical systems to be integrated, with some *
   * constraints to be taken into account */
  SP::InteractionsSet OSIInteractions;

  /** size of the memory for the integrator */
  unsigned int sizeMem;

  /** A link to the simulation that owns this OSI */
  SP::Simulation simulationLink;

  /** Work map to save state-related data for the dynamical systems of the osi - DSVector: map<DS * , SP::SiconosVector> */
  DSVectors workX;

  /** the corresponding XML object */
  SP::OneStepIntegratorXML integratorXml;

  /** basic constructor with Id
   *  \param integrator type/name
   */
  OneStepIntegrator(const OSI::TYPES&);

  /** constructor from xml file
      \param integrator type/name
      \param OneStepIntegratorXML* : the corresponding XML object
      \param the set of all DS in the NSDS
      \param the set of all interactions in the NSDS
  */
  OneStepIntegrator(const OSI::TYPES&, SP::OneStepIntegratorXML, SP::DynamicalSystemsSet , SP::InteractionsSet);

  /** constructor from a minimum set of data
   *  \param string, integrator type/name
   *  \param DynamicalSystemsSet : a set of DynamicalSystem to be integrated
   */
  OneStepIntegrator(const OSI::TYPES&, const DynamicalSystemsSet&);

private:

  /** default constructor
   */
  OneStepIntegrator();

  /** copy constructor, private, no copy nor pass-by value allowed */
  OneStepIntegrator(const OneStepIntegrator&);

  /** assignment (private => forbidden) */
  OneStepIntegrator& operator=(const OneStepIntegrator&);

public:

  /** destructor
   */
  virtual ~OneStepIntegrator() {};

  // --- GETTERS/SETTERS ---

  /** get the type of the OneStepIntegrator
   *  \return string : the type of the OneStepIntegrator
   */
  inline const OSI::TYPES getType() const
  {
    return integratorType;
  }

  /** set the type of the OneStepIntegrator
   *  \return string : the type of the OneStepIntegrator
   */
  inline void setType(const OSI::TYPES& newType)
  {
    integratorType = newType;
  }

  /** get the set of DynamicalSystem associated with the Integrator
   *  \return a DynamicalSystemsSet
   */
  inline SP::DynamicalSystemsSet getDynamicalSystems() const
  {
    return OSIDynamicalSystems;
  };

  /** gets an iterator to the first element of the OSIDynamicalSystems set.
   *  \return a DSIterator.
   */
  inline DSIterator dynamicalSystemsBegin()
  {
    return OSIDynamicalSystems->begin();
  };

  /** gets an iterator equal to OSIDynamicalSystems->end().
   *  \return a DSIterator.
   */
  inline DSIterator dynamicalSystemsEnd()
  {
    return OSIDynamicalSystems->end();
  };

  /** gets a const iterator to the first element of the OSIDynamicalSystems set.
   *  \return a ConstDSIterator.
   */
  inline ConstDSIterator dynamicalSystemsBegin() const
  {
    return OSIDynamicalSystems->begin();
  };

  /** gets a const iterator equal to OSIDynamicalSystems->end().
   *  \return a ConstDSIterator.
   */
  inline ConstDSIterator dynamicalSystemsEnd() const
  {
    return OSIDynamicalSystems->end();
  };

  /** set the DynamicalSystem list of this Integrator
   *  \param a DynamicalSystemsSet
   */
  void setDynamicalSystems(const DynamicalSystemsSet&);

  /** get the set of Interactions associated with the Integrator
   *  \return an InteractionsSet
   */
  inline const SP::InteractionsSet getInteractions() const
  {
    return OSIInteractions;
  };

  /** gets an iterator to the first element of the OSIInteractions set.
   *  \return a InteractionsIterator.
   */
  inline InteractionsIterator interactionsBegin()
  {
    return OSIInteractions->begin();
  };

  /** gets an iterator equal to OSIInteractions->end().
   *  \return a InteractionsIterator.
   */
  inline InteractionsIterator interactionsEnd()
  {
    return OSIInteractions->end();
  };

  /** gets a const iterator to the first element of the OSIInteractions set.
   *  \return a ConstInteractionsIterator.
   */
  inline ConstInteractionsIterator interactionsBegin() const
  {
    return OSIInteractions->begin();
  };

  /** gets a const iterator equal to OSIInteractions->end().
   *  \return a ConstInteractionsIterator.
   */
  inline ConstInteractionsIterator interactionsEnd() const
  {
    return OSIInteractions->end();
  };

  /** set the Interaction list of this Integrator
   *  \param an InteractionsSet
   */
  void setInteractions(const InteractionsSet&);

  /** get sizeMem value
   *  \return an unsigned int
   */
  inline const unsigned int getSizeMem() const
  {
    return sizeMem;
  };

  /** set sizeMem
   *  \param an unsigned int
   */
  inline void setSizeMem(unsigned int newValue)
  {
    sizeMem = newValue;
  };

  /** get the Simulation that owns the OneStepIntegrator
   *  \return a pointer to Simulation
   */
  inline SP::Simulation getSimulationPtr() const
  {
    return simulationLink;
  }

  /** set the Simulation of the OneStepIntegrator
   *  \param a pointer to Simulation
   */
  inline void setSimulationPtr(SP::Simulation newS)
  {
    simulationLink = newS;
  }

  /** get the OneStepIntegratorXML of the OneStepIntegrator
   *  \return a pointer on the OneStepIntegratorXML of the OneStepIntegrator
   */
  inline SP::OneStepIntegratorXML getOneStepIntegratorXMLPtr() const
  {
    return integratorXml;
  }

  /** set the OneStepIntegratorXML of the OneStepIntegrator
   *  \param OneStepIntegratorXML* : the pointer to set the OneStepIntegratorXML
   */
  inline void setOneStepIntegratorXMLPtr(SP::OneStepIntegratorXML newIntegratorXml)
  {
    integratorXml = newIntegratorXml;
  }

  /** get workX vector which corresponds to input ds
   \param a SP::DynamicalSystem
   \return a SP::SiconosVector
  */
  SP::SiconosVector getWorkX(SP::DynamicalSystem);

  // --- OTHERS ... ---

  /** initialise the integrator
      \param the simulation that owns this OSI
   */
  virtual void initialize(SP::Simulation) = 0;

  /** Save Dynamical Systems data into memory.
   */
  void saveInMemory();

  /** return the maximum of all norms for the discretized residus of DS
      \return a double
   */
  virtual double computeResidu();

  /** integrates the Dynamical System linked to this integrator, without taking constraints
   * into account.
   */
  virtual void computeFreeState();

  /** integrate the system, between tinit and tend (->iout=true), with possible stop at tout (->iout=false)
   *  \param double: tinit, initial time
   *  \param double: tend, end time
   *  \param double: tout, real end time
   *  \param int: flag used in Lsodar.
   */
  virtual void integrate(double&, double&, double&, int&) = 0;

  /** set to zero all the r vectors of the DynamicalSystems of the present OSI
   */
  void resetNonSmoothPart();

  /** update the state of the DynamicalSystem attached to this Integrator
   *  \param unsigned int: level of interest for the dynamics
   */
  virtual void updateState(unsigned int) = 0;

  /** print the data to the screen
   */
  virtual void display() = 0;

  /** copy the data of the OneStepNSProblem to the XML tree
   */
  void saveIntegratorToXML();

};

#endif // ONESTEPINTEGRATOR_H
