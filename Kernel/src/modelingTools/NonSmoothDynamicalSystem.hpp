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
/*! \file NonSmoothDynamicalSystem.hpp
 */
#ifndef NSDS_H
#define NSDS_H

/** Available Dynamical Systems types*/
//enum dynamicalsystem {LAGRANGIANNLDS, LAGRANGIANTIDS, LINEARTIDS};

#include "SiconosPointers.hpp"
#include "InteractionsSet.hpp"
#include "DynamicalSystemsSet.hpp"
#include "Topology.hpp"

class Interaction;
class DynamicalSystem;
class Topology;


/** the Non Smooth Dynamical System composed with dynamical systems
 *  that interact alltogether.
 *
 *  \author SICONOS Development Team - copyright INRIA
 *  \version 3.0.0.
 *  \date (Creation) Apr 23, 2004
 *
 */
class NonSmoothDynamicalSystem
{
private:
  /** serialization hooks
  */
  ACCEPT_SERIALIZATION(NonSmoothDynamicalSystem);


  /** TRUE if the NonSmoothDynamicalSystem is a boundary value problem*/
  bool _BVP;

  /** the topology of the system */
  SP::Topology _topology;

  /** the XML object linked to the NonSmoothDynamicalSystem to read XML data */
  SP::NonSmoothDynamicalSystemXML _nsdsxml;

  NonSmoothDynamicalSystem(const NonSmoothDynamicalSystem& nsds);

  /** False is one of the interaction is non-linear.
   */
  bool _mIsLinear;

public:

  /** default constructor
   */
  NonSmoothDynamicalSystem();

  /** xml constructor
   *  \param: the XML object corresponding to the NonSmoothDynamicalSystem
   */
  NonSmoothDynamicalSystem(SP::NonSmoothDynamicalSystemXML newNsdsxml);

  /** destructor
   */
  ~NonSmoothDynamicalSystem();

  // --- GETTERS/SETTERS ---

  /** get problem type (true if BVP)
   *  \return a bool
   */
  inline bool isBVP() const
  {
    return _BVP;
  }

  /** get problem type (true if IVP)
   *  \return a bool
   */
  inline bool isIVP() const
  {
    return !_BVP;
  }

  /** set the NonSmoothDynamicalSystem to BVP, else it is IVP
   *  \param newBvp true if BVP, false otherwise
   */
  inline void setBVP(const bool& newBvp)
  {
    _BVP = newBvp;
  }

  // === DynamicalSystems management ===

  /** get the number of Dynamical Systems present in the NSDS
      \return an unsigned int
   */
  inline unsigned int getNumberOfDS() const
  {
    return _topology->dSG(0)->size();
  }

  /** get all the dynamical systems declared in the NonSmoothDynamicalSystem.
   * \return a SP::DynamicalSystemsGraph
   */
  inline const SP::DynamicalSystemsGraph dynamicalSystems() const
  {
    return _topology->dSG(0);
  }
  
  // === Interactions management ===

  /** get the number of Interactions present in the NSDS.
   *  \return an unsigned int
   */
  inline unsigned int getNumberOfInteractions() const
  {
    return _topology->interactions()->size();
  };

  /** get all the Interactions of the NonSmoothDynamicalSystem problem (saved in a set)
   *  \return an InteractionsSet *
   */
  inline const SP::InteractionsSet interactions() const
  {
    return _topology->interactions();
  }

  /** remove an interaction to the system
   * \param inter a pointer to the interaction to remove
   */
  inline void removeInteraction(SP::Interaction inter)
  {
    _topology->removeInteraction(inter);
  };

  /** add a dynamical system
   * \param ds a pointer to the system to remove
   */
  inline void insertDynamicalSystem(SP::DynamicalSystem ds)
  {
    _topology->insertDynamicalSystem(ds);
    _mIsLinear = ((ds)->isLinear() && _mIsLinear);
  };


  /** remove a dynamical system
   * \param a shared pointer to a dynamical system
   */
  inline void removeDynamicalSystem(SP::DynamicalSystem ds)
  {
    _topology->removeDynamicalSystem(ds);
  };

  /** link an interaction to two dynamical systems
   * \param inter a SP::Interaction
   * \param ds1 a SP::DynamicalSystem
   * \param ds2 a SP::DynamicalSystem (optional)
   */
  inline void link(SP::Interaction inter, SP::DynamicalSystem ds1, SP::DynamicalSystem ds2 = SP::DynamicalSystem())
  {
    _mIsLinear = ((inter)->relation()->isLinear() && _mIsLinear);
    _topology->link(inter, ds1, ds2);
  };

    /** specify id the given Interaction is for controlling the DS
   * \param inter the Interaction
   * \param isControlInteraction true if the Interaction is used for
   * control purposes
   **/
  void setControlProperty(SP::Interaction inter, const bool isControlInteraction)
  {
    _topology->setControlProperty(inter, isControlInteraction);
  }

  /** get Dynamical system number I
   * \param nb the identifier of the DynamicalSystem to get
   * \return a pointer on DynamicalSystem
   */
  inline SP::DynamicalSystem dynamicalSystem(int nb) const
  {
    return _topology->getDynamicalSystem(nb);
  }

  /** get the topology of the system
   *  \return a pointer on Topology
   */
  inline SP::Topology topology() const
  {
    return _topology;
  }

  /** get the xml linked object
   *  \return a pointer on NonSmoothDynamicalSystemXML
   */
  inline SP::NonSmoothDynamicalSystemXML nonSmoothDynamicalSystemXML()
  const
  {
    return _nsdsxml;
  }

  /** set the xml linked object
   *  \param newNsdsxml a pointer on NonSmoothDynamicalSystemXML to link
   */
  inline void setNonSmoothDynamicalSystemXMLPtr(SP::NonSmoothDynamicalSystemXML newNsdsxml)
  {
    _nsdsxml = newNsdsxml;
  }

  // --- OTHER FUNCTIONS ---

  /** copy the data of the NonSmoothDynamicalSystem to the XML tree
   *
   */
  void saveNSDSToXML();

  /** display the data of the Non Smooth Dynamical System
   */
  void display() const;

  /** calculate an indicator that gives convergence information for
   *  the DSs
   *  \return a double
   */
  double nsdsConvergenceIndicator();

  /** return false is one of the interations is not linear.  else
   *  return true.
   *  \return a bool
   */
  inline bool isLinear() const
  {
    return _mIsLinear;
  };

  void clear();

  /** set symmetry in the blocks computation
   * \param val a bool
   */
  void setSymmetric(bool val)
  {
    topology()->setSymmetric(val);
  }

  /** use this function to add a control input to the DynamicalSystem ds. Internaly it adds
   * an optional property to the DynamicalSystem
   * \param ds the DynamicalSystem
   * \param B the B matrix */
  void setControlInput(SP::DynamicalSystem ds, SP::SiconosMatrix B);

  /** use this function to flag this DynamicalSystem as an Observer. Internaly it adds
   * an optional property to the DynamicalSystem
   *  \param ds the DynamicalSystem
   *  \param L the observer gain matrix */
  void setObserverInput(SP::DynamicalSystem ds, SP::SiconosMatrix L);
};

#endif
