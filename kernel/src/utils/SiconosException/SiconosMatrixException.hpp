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


/*! \file SiconosMatrixException.hpp
 */
#ifndef __SiconosMatrixException__
#define __SiconosMatrixException__

#include "SiconosException.hpp"
#include <misc.h>

/** Exception caused by a SiconosMatrix
 *
 *  \author SICONOS Development Team - copyright INRIA
 *   \version 3.0.0.
 *   \date (Creation) 05/25/2004
 *
 *
 *
 * SiconosMatrixException must be throws when an error is find in a SiconosMatrix
 * This exception can be catched by "catch(SiconosMatrixException)" or "catch(SiconosException)"
 *
 */
class SiconosMatrixException : public SiconosException
{
public:

  /** constructor
   */
  SiconosMatrixException();

  /** constructor with a report
   * \param report exception description
   */
  SiconosMatrixException(const std::string& report);

  /** destructor
   */
  ~SiconosMatrixException();

  /** static function which throw a SiconosMatrixException
   */
  static void selfThrow() NO_RETURN ;

  /** static function which throw a SiconosMatrixException with a report
   * \param report exception description
   * \exception SiconosMatrixException
   */
  static void selfThrow(const std::string& report) NO_RETURN;
};

#endif //__SiconosMatrixException__
