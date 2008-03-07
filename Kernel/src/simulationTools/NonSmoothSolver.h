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
/*! \file NonSmoothSolver.h
  \brief Definition of NonSmoothSolver class, set of parameters for OSNSProblem solver.
*/
#ifndef NONSMOOTHSOLVER_H
#define NONSMOOTHSOLVER_H

#include "Solver_Options.h" // Numerics header 
#include "NonSmoothSolverXML.h"
#include <vector>
#include <string>

/** Size of the vectors of parameters int_parameters and double_parameters. We set this size to NB_PARAM
    whatever the associated OneStepNSProblem is, to simplify the memory management for the Solver_Options structure fields.
*/
const unsigned int NB_PARAM = 4;

/** Object used to send parameters of type int */
typedef std::vector<int> IntParameters;

/** Object used to send parameters of type double */
typedef std::vector<double> DoubleParameters;

/** Non Smooth Solver parameters definition for One Step Non Smooth Problem.
 *
 *  \author SICONOS Development Team - copyright INRIA
 *  \version 3.0.0.
 *  \date (Creation) December 19, 2005
 *
 * NonSmoothSolver is an interface used to set non-smooth solver parameters in Kernel/OneStepNSProblem.
 *
 * A OneStepNSProblem is supposed to build a specific problem (LCP, FrictionContact ...) and then to call a driver of Numerics\n
 * that will call a specific algorithm (Lemke, NSGS ...) to solve the problem. \n
 * A driver needs a Solver_Options structure as input argument. This structure is used to set the chosen algorithm and its input parameters.\n
 * The present class is used to fill in this structure.
 *
 * Each OneStepNSProblem has a NonSmoothSolver* object member, in which the Solver_Options structure is embedded. \n
 * The parameters of the NonSmoothSolver can be:
 *     - read in an XML file
 *     - set using a data constructor, with a list of values
 *     - left empty. In that case, some default values will be uploaded during Numerics driver call, from a file named XXX_parameters.opt,\n
 * XXX being the name of the non-smooth problem (LCP ...).
 *
 *
 * Indeed a Solver is composed of:\n
 *      - a Solver_Options structure\n
 *      - a string, name of the algorithm/solver. The available solvers depends on the type of problem. \n
 *      - a DoubleParameters and a IntParameters, lists of double/int parameters
 *
 * For details on what are the available solvers and which parameters they need see the documentation of Numerics for " \ref NonSmoothSolvers ".\n
 *
 * Note that there is no checking of the validity of input parameters, of their number, the name of the algorithm ...\n
 * This is done in OneStepNSProblem or directly in Numerics driver.
 *
 *
 */
class NonSmoothSolver
{
private:

  /** Algorithm/solver name */
  std::string name;

  /** Vector of double parameters */
  IntParameters * int_parameters;

  /** Vector of int parameters */
  DoubleParameters * double_parameters;

  /** Numerics structure used to solve solver options */
  Solver_Options * numerics_solver_options;

  /** boolean, true if Solvers parameters have been set or read, else false
      (in that case, default options will be read during Numerics driver call)  */
  bool isSet;

  /** Fill Numerics structure Solver_Options using current object data (only if isSet = true)
   */
  void fillSolverOptions();

public:

  /** Default constructor - All parameters unset.
   */
  NonSmoothSolver();

  /** Copy constructor
   *  \param a NonSmoothSolver to be copied
   */
  NonSmoothSolver(const NonSmoothSolver&);

  /** Constructor from a set of data
      \param algorithm/solver name
      \param vector of int parameters
      \param vector of double parameters
  */
  NonSmoothSolver(const std::string&, IntParameters&, DoubleParameters&);

  /** Constructor from a set of data
       \param algorithm/solver name
       \param vector of int parameters
       \param vector of double parameters
       \param double * memory working zone
       \param int * memory working zone
   */
  NonSmoothSolver(const std::string&, IntParameters&, DoubleParameters&, double * dWork, int * iWork);

  /** Constructor from XML object
   *  \param a pointer to NonSmoothSolverXML
   */
  NonSmoothSolver(NonSmoothSolverXML*);

  /** Constructor, by reading parameters in file (XML)
   *  \param name of the input file
   */
  NonSmoothSolver(const std::string&);

  /** Destructor */
  ~NonSmoothSolver();

  /** To get the solver algorithm name
   *  \return a string
   */
  inline const std::string getName() const
  {
    return name;
  };

  /** To set the solver algorithm name
   *  \param a string
   */
  inline void setName(const std::string& newVal)
  {
    name = newVal;
  };

  /** To get the list of int. parameters
   *  \return an IntParameters*
   */
  inline IntParameters* getIntParametersPtr() const
  {
    return int_parameters;
  };

  /** To set int parameter number i
   *  \param i, the index of the parameter to be set
   *  \param val, value of parameter[i]
   */
  void setIntParameter(unsigned int i, int val);

  /** To get the list of double parameters
   *  \return a DoubleParameters*
   */
  inline DoubleParameters* getDoubleParametersPtr() const
  {
    return double_parameters;
  };

  /** To set double parameter number i
   *  \param i, the index of the parameter to be set
   *  \param val, value of parameter[i]
   */
  void setDoubleParameter(unsigned int i, double val);

  /** To get the Solver_Options structure
   *  \return , the numerics structure used to save solver parameters
   */
  inline Solver_Options* getNumericsSolverOptionsPtr() const
  {
    return numerics_solver_options;
  };

  /** To check if default values must be read or not (in Numerics driver)
   *  \return true if the solvers parameters has been set, else false (ie need default values)
   */
  inline bool isSolverSet() const
  {
    return isSet;
  };

  /** Switch to default values reading (or not) for solver
   *  \param input, true to read default values \n
   (warning: any previous input for parameters will be ignored!)\n
   else false (warning: it means that parameters must be properly set)
  */
  void setSolverToDefault(bool input);

  /** To display solver data
   */
  void display() const ;

  /** Save current object into the xml model
   */
  void saveNonSmoothSolverToXML();

};

#endif // NonSmoothSolver_H
