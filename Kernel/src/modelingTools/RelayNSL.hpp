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
/*! \file RelayNSL.hpp

*/
#ifndef RELAYNSLAW_H
#define RELAYNSLAW_H

#include "NonSmoothLaw.hpp"

/** Relay NonSmoothLaw
 *
 *  \author SICONOS Development Team - copyright INRIA
 *  \version 3.0.0.
 *  \date (Creation) Apr 27, 2004 (documentation) Nov 25, 2009
 *
 * This class formalizes the Relay nonsmooth law  i.e.
 * \f[
 * \begin{array}{l}
 * -y \in \mathcal{N}_{[lb,ub]}(\lambda),
 * \end{array}
 * \f]
 * where \f$lb\f$ is the lower bound and   \f$ub\f$ is the upper bound of the Relay law.
 *
 * In this default case, the lower bound is set to \f$lb=-1\f$ and the upper bound ub is set to \f$ub=1\f$. We get  the well-known form of the RelayNSL as
 * the multivalued sign function, i.e.
 * \f[
 * \begin{array}{l}
 * y \in -\mathcal{N}_{[-1,1]}(\lambda)\quad \Longleftrightarrow \quad\lambda \in -\mbox{sgn} (y)
 * \end{array}
 * \f]
 * where the multi-valued sign function is defined as
 * \f[
 *  \mbox{sgn} (y) =
 *  \left\{ \begin{array}{lcl}
 *  1 &\quad& y >0 \\
 *  \,[-1,1] &\quad& y =0 \\
 *  -1 &\quad& y <0 \\
 *  \end{array}\right.
 *  \f]
 * \todo Build the Sgn NonSmoothLaw as the default instance of Relay
 */
class RelayNSL : public NonSmoothLaw
{

private:
  /** serialization hooks
  */
  ACCEPT_SERIALIZATION(RelayNSL);

  /** represent the lower bound of the Relay */
  double _lb;

  /** represent the upper bound of the Relay*/
  double _ub;


  /** default constructor
   */
  RelayNSL();

public:

  /** constructor with the value of the RelayNSL attributes
  *  \param size size of the NonSmoothLaw
  *  \param lb lower endpoint of the interval, default value is -1.0
  *  \param ub upper endpoint of the interval, default value is 1.0
  */
  RelayNSL(unsigned int size, double lb = -1.0, double ub = 1.0);

  ~RelayNSL();

  /** check the ns law to see if it is verified
  *  \return true if the NS Law is verified, false otherwise
  */
  bool isVerified() const;

  /** to get lb
  *  \return the value of lb
  */
  inline double lb() const
  {
    return _lb;
  };

  /** to set the lower bound
  *  \param lb the new lower bound
  */
  inline void setLb(double lb)
  {
    _lb = lb;
  };

  /** to get ub
  *  \return the value of ub
  */
  inline double ub() const
  {
    return _ub;
  };


  /** to set ub
  *  \param ub the new upper bound
  */
  inline void setUb(double ub)
  {
    _ub = ub;
  };

  /** print the data to the screen
  */
  void display() const;

  /** visitors hook
   */
  ACCEPT_STD_VISITORS();

};

#endif // RELAYNSLAW_H
