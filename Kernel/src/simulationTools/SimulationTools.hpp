/* Siconos-Kernel version 1.3.0, Copyright INRIA 2005-2008.
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
/*! \file SiconosKernel.h
Include files related to simulationTools
Note that not all files from the current location are listed below, since some of them are already included inside the ones below.
*/

#include "UnitaryRelation.h"
#include "Topology.h"
#include "EventDriven.h"
#include "EventsManager.h"
#include "EventFactory.h"
#include "FrictionContact.h"
#include "PrimalFrictionContact.h"
#include "TimeDiscretisation.h"
#include "TimeStepping.h"
#include "Equality.h"
#include "LCP.h"
#include "MLCP.h"
#include "MLCP2.h"
//#include "mlcpDefaultSolver.h"
#include "QP.h"
#include "Lsodar.h"
#include "Moreau.h"
#include "Moreau2.h"
#include "Relay.h"
#include "NonSmoothEvent.h"
#include "TimeDiscretisationEvent.h"
