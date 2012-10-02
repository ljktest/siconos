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
/*! \file TimeDiscretisation.hpp

  \brief A time-discretisation scheme for a given interval.
 */
#ifndef TIMEDISCRETISATION_H
#define TIMEDISCRETISATION_H

#include "TimeDiscretisationXML.hpp"
#include <vector>

/** A time discretisation scheme

    \author SICONOS Development Team - copyright INRIA
    \version 3.0.0.
    \date (Creation) Apr 26, 2004

    A TimeDiscretisation object is used to discretized a given time interval. \n
    TimeDiscretisation are used:
    - in the simulation, as a user-input to discretized [t0,T]
    - in Sensor and Actuator, to define the set of time instants where the sensor or actuator
    must operate.

    A TimeDiscretisation is defined with a starting time (t0), a time step size (h, non necessarily constant),
    the number of the current time step (k). \n
    The time instant values are saved in a vector tk. Depending on the way of construction of the TimeDiscretisation,
    all or only current and next times are saved in tk. The difference is just a question of saving memory. \n

    Note that the TimeDiscretisation is not linked to the Model. It's up to the user to check that the way he builds his time-discretisation fits with the t0 and T given in the model.

    \section tdMfunc Main functions:
    - setCurrentTimeStep(), to set current h. This value will be used for all future time steps, until next change.
    - increment(), shift to next time step (increment k, and shift t[k] and t[k+1])
    - currentTime(), return t[k]

    \section tdBuild Construction

    - input = the complete vector tk. This defines t0, T, number of time steps and time step size
    (which is not necessarily constant). In this case, the whole vector is saved in the memory.
    - inputs = number of time steps, t0 and T.
    size h is computed with t0,T and nSteps. Only two values are saved: t[k] and t[k+1] = t[k] + h.
    h can be changed at any time.
    - inputs = h and t0. Only two values are saved: t[k] and t[k+1] = t[k] + h.
    h can be changed at any time.
    - inputs = t0 and h.  Only two values are saved: t[k] and t[k+1] = t[k] + h.
    h can be changed at any time.

 */
class TimeDiscretisation
{
private:
  /** serialization hooks
  */
  ACCEPT_SERIALIZATION(TimeDiscretisation);


  /** Default value for the time step (tk+1 - tk) */
  double _h;

  /** Number of current time step (Simulation runs between steps k and k+1) */
  unsigned int _k;

  /** vector of time values at each step (=> size = n1+n2+1 - Default size = 2 - Max size= nSteps+1) */
  TkVector _tk;

  /** the XML object linked to the TimeDiscretisation to read XML data */
  SP::TimeDiscretisationXML _timeDiscretisationXML;

  /** Indicator flag : 1 when we have a complete vector for _tk (_h may vary), 2 when _h is fixed.*/
  int _tdCase;

  /** index in tk which corresponds to the current time step (tk[pos] = t[k]) -
      Required since the size of tk depends on tdCase.
      tdCase = 1 => pos = k - tdCase = 2 => pos = 0 .
  */
  int _pos;

  /** default constructor (private => no copy nor pass-by value)
   */
  TimeDiscretisation() {};

  /** Assignment Operator (private => forbidden) */
  TimeDiscretisation& operator =(const TimeDiscretisation&);

public:

  /** constructor with XML
      \param SP::TimeDiscretisationXML : the XML object corresponding
      \param initial time
      \param final time
  */
  TimeDiscretisation(SP::TimeDiscretisationXML, double, double);

  // --- Straightforward constructors ---

  /** constructor with tk vector of instant times values.
   *  \param a TkVector that describes the discretisation
   */
  TimeDiscretisation(const TkVector&);

  /** constructor with the number of time steps
   *  \param int (nb steps)
   *  \param t0, initial time
   *  \param T, final time
   */
  TimeDiscretisation(unsigned int, double, double);

  /** constructor with the size of the default time step and t0
   *  \param double, initial time value
   *  \param double, the time step
   */
  TimeDiscretisation(double, double);

  /** Copy constructor
   * \param td the TimeDiscretisation to copy
   */
  TimeDiscretisation(const TimeDiscretisation& td);

  // Destructor
  ~TimeDiscretisation();

  // --- GETTERS/SETTERS ---

  /** get the time step
   *  \return a double the value of _t[k+1] - _t[k], the current time step
   */
  inline double currentTimeStep() const
  {
    return _h;
  };

  inline unsigned int getK() const
  {
    return _k;
  };

  inline int getTDCase() const
  {
    return _tdCase;
  };

  /** set current _h (ie tk+1 - tk)
   *  \param the new value for _h.
   *  Warning: if the TimeDiscretisation has been built
   *  with a complete _tk vector (_tdCase = 1), a call to this function
   *  will switch to _tdCase = 2, ie _h = newH for all future steps (until a new set of _h)
   */
  void setCurrentTimeStep(double);

  inline const TkVector & getTk() const
  {
    return _tk;
  };
  /** get the value of tk at step k
   * \param ind the step
   * \return a double
   */
  inline double getTk(unsigned int ind) const
  {
    return _tk.at(ind);
  };

  /** set the TKVector _tk
   *  \param newTk the new value for _tk
   *  \warning it will szitch _tdCase to 1
   */
  void setTk(const TkVector& newTk);

  /** Get the current time instant value ( _tk[pos] )
   * \return a double : _tk[pos]
   */
  inline double currentTime() const
  {
    return _tk[_pos];
  };

  /** Get time instant value at index k+1 ( _tk[pos+1] )
   *  \return a double : _tk[pos+1]
   */
  inline double nextTime() const
  {
    return _tk[_pos + 1];
  };

  /** get the TimeDiscretisationXML of the TimeDiscretisation
   *  \return a pointer on the TimeDiscretisationXML of the TimeDiscretisation
   */
  inline SP::TimeDiscretisationXML timeDiscretisationXML() const
  {
    return _timeDiscretisationXML;
  }

  /** set the TimeDiscretisationXML of the TimeDiscretisation
   *  \param timediscrxml a SP::TimeDiscretisationXML
   */
  inline void setTimeDiscretisationXMLPtr(SP::TimeDiscretisationXML timediscrxml)
  {
    _timeDiscretisationXML = timediscrxml;
  }

  /** Steps to next time step. */
  void increment();

  // --- OTHER FUNCTIONS ---
  /** print the discretisation data to the screen
   */
  void display() const;

  // --- XML Functions ---

  /** saves the TimeDiscretisation to the XML tree
   *  \exception RuntimeException
   */
  void saveTimeDiscretisationToXML();
};

//DEFINE_SPTR(TimeDiscretisation)

#endif // TIMEDISCRETISATION_H
