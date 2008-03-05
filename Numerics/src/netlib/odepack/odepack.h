// Siconos-Numerics version 3.0.0, Copyright INRIA 2005-2008.
// Siconos is a program dedicated to modeling, simulation and control
// of non smooth dynamical systems.
// Siconos is a free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// Siconos is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Siconos; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//
// Contact: Vincent ACARY vincent.acary@inrialpes.fr
//

#include "blaslapack.h"

typedef void (*fpointer)(integer *, doublereal *, doublereal *, doublereal *);
typedef void (*gpointer)(integer *, doublereal *, doublereal*, integer *, doublereal *);
typedef void (*jacopointer)(integer *, doublereal *, doublereal *, integer* , integer *,  doublereal *, integer *);

#ifdef __cplusplus
extern "C" {
#endif

  void F77NAME(DLSODE)(fpointer, integer * neq, doublereal * y, doublereal *t, doublereal *tout, integer * itol, doublereal * rtol, doublereal *atol, integer * itask, integer *istate, integer * iopt, doublereal * rwork, integer * lrw, integer * iwork, integer * liw, jacopointer, integer * mf);

  void F77NAME(dlsodar)(fpointer, integer * neq, doublereal * y, doublereal *t, doublereal *tout, integer * itol, doublereal * rtol, doublereal *atol, integer * itask, integer *istate, integer * iopt, doublereal * rwork, integer * lrw, integer * iwork, integer * liw, jacopointer, integer * jt, gpointer, integer* ng, integer * jroot);

#ifdef __cplusplus
}
#endif

