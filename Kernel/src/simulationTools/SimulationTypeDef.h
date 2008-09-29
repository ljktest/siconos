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

Typedef for simulation-related objects
*/

#ifndef SimulationTypedef_H
#define SimulationTypedef_H

#include <vector>
#include <map>
#include <set>

#include "UnitaryRelationsSet.h"

#include <boost/shared_container_iterator.hpp>
#include "SiconosPointers.h"

/** double precision machine */
const double MACHINE_PREC = 1e-14;//dlamch_("e");
//#include "OneStepIntegrator.h"
class OneStepIntegrator;



// ================== Objects to handle DS ==================

/** Map of SiconosMatrix; key = the related DS*/
typedef std::map<SP::DynamicalSystem, SiconosMatrixSPtr> MapOfDSMatrices;

/** Iterator through a map of matrices */
typedef MapOfDSMatrices::iterator MatIterator;

/** Const iterator through a map of matrices */
typedef MapOfDSMatrices::const_iterator ConstMatIterator;

/** Map of SiconosVector; key = the related DS*/
typedef std::map<SP::DynamicalSystem, SiconosVectorSPtr> DSVectors;

/** Iterator through a map of matrices */
typedef DSVectors::iterator DSVectorsIterator;

/** Const iterator through a map of matrices */
typedef DSVectors::const_iterator ConstDSVectorsIterator;

/** Map of double; key = the related DS */
typedef std::map<SP::DynamicalSystem, double> MapOfDouble;

/** Iterator through a map of double */
typedef MapOfDouble::iterator DoubleIterator;

/** Map of double; key = the related DS */
typedef std::map<SP::DynamicalSystem, unsigned  int> DS_int;

/** Iterator through a map of double */
typedef MapOfDouble::iterator DoubleIterator;

/** Map of bool; key = the related DS */
typedef std::map<SP::DynamicalSystem, bool> MapOfDSBool;

// ================== Objects to handle UnitaryRelations ==================

/** Map of SiconosMatrices with a UnitaryRelations as a key - Used for diagonal unitaryBlock-terms in assembled matrices of LCP etc ...*/
typedef std::map< SP::UnitaryRelation, SiconosMatrixSPtr>  MapOfUnitaryMatrices;

/** Iterator through a MapOfUnitaryMatrices */
typedef MapOfUnitaryMatrices::iterator UnitaryMatrixColumnIterator ;

/** Const iterator through a MapOfUnitaryMatrices */
typedef MapOfUnitaryMatrices::const_iterator ConstUnitaryMatrixColumnIterator;




/** Map of MapOfUnitaryMatrices with a UnitaryRelation as a key - Used for extra-diagonal unitaryBlock-terms in assembled matrices of LCP etc ..*/
typedef std::map< SP::UnitaryRelation , MapOfUnitaryMatrices >  MapOfMapOfUnitaryMatrices;

/** Iterator through a MapOfMapOfUnitaryMatrices */
typedef MapOfMapOfUnitaryMatrices::iterator UnitaryMatrixRowIterator ;

/** Const iterator through a MapOfMapOfUnitaryMatrices */
typedef MapOfMapOfUnitaryMatrices::const_iterator ConstUnitaryMatrixRowIterator ;




/** Map of MapOfDSUnitaryMatrices with a DynamicalSystem as a key - Used for unitaryBlock-terms indexed by a DynamicalSystem and an UnitaryRelation in assembled matrices of LCP etc ..*/
typedef std::map< SP::DynamicalSystem , MapOfUnitaryMatrices >  MapOfDSMapOfUnitaryMatrices;

/** Iterator through a MapOfDSMapOfUnitaryMatrices */
typedef MapOfDSMapOfUnitaryMatrices::iterator DSUnitaryMatrixRowIterator ;

/** Const iterator through a MapOfDSMapOfUnitaryMatrices */
typedef MapOfDSMapOfUnitaryMatrices::const_iterator ConstDSUnitaryMatrixRowIterator ;




/** Map of MapOfUnitaryMapOfDSMatrices with a DynamicalSystem as a key - Used for unitaryBlock-terms indexed by a DynamicalSystem and an UnitaryRelation in assembled matrices of LCP etc ..*/
typedef std::map< SP::UnitaryRelation , MapOfDSMatrices >  MapOfUnitaryMapOfDSMatrices;

/** Iterator through a MapOfUnitaryMapOfDSMatrices */
typedef MapOfUnitaryMapOfDSMatrices::iterator UnitaryDSMatrixRowIterator ;

/** Const iterator through a MapOfUnitaryMapOfDSMatrices */
typedef MapOfUnitaryMapOfDSMatrices::const_iterator ConstUnitaryDSMatrixRowIterator ;



/** Map of map of bools, with UnitaryRelations as keys */
typedef std::map< SP::UnitaryRelation , std::map<SP::UnitaryRelation, bool> >  MapOfMapOfUnitaryBool;

/** Map of map of bools, with UnitaryRelations as keys */
typedef std::map< SP::UnitaryRelation , MapOfDSBool >  MapOfUnitaryMapOfDSBool;

/** Map of map of bools, with UnitaryRelations as keys */
typedef std::map< SP::DynamicalSystem , std::map<SP::UnitaryRelation, bool> >  MapOfDSMapOfUnitaryBool;

/** Vector that contains a sequel of sets of UnitaryRelations*/
typedef std::vector< UnitaryRelationsSetSPtr > VectorOfSetOfUnitaryRelations;

/** Map to link SP::UnitaryRelation with an int - Used for example in unitaryBlocksPositions for OSNSMatrix */
typedef std::map< SP::UnitaryRelation , unsigned int > UR_int;

/** list of indices */
typedef std::vector<int> IndexInt;

// ================== Objects to handle OSI ==================


/** Vector of OneStepIntegrator */
typedef std::set<OneStepIntegratorSPtr> OSISet;

/** Iterator through vector of OSI*/
typedef OSISet::iterator OSIIterator;

/** Const iterator through vector of OSI*/
typedef OSISet::const_iterator ConstOSIIterator;

/** Return type value for insert function - bool = false if insertion failed. */
typedef std::pair<OSISet::iterator, bool> CheckInsertOSI;

/** A map that links DynamicalSystems and their OneStepIntegrator. */
typedef std::map<SP::DynamicalSystem, OneStepIntegratorSPtr> DSOSIMap;

/** Iterator through a DSOSIMap. */
typedef DSOSIMap::iterator DSOSIIterator;

/** Const Iterator through a DSOSIMap. */
typedef DSOSIMap::const_iterator DSOSIConstIterator;

// ================== Objects to handle OSNS ==================

//#include "OneStepNSProblem.h"
class OneStepNSProblem;
/** Map of OSNS */
typedef std::map<std::string, OneStepNSProblemSPtr > OneStepNSProblems;

/** Iterator through OneStepNSProblems */
typedef OneStepNSProblems::iterator OSNSIterator;

/** Const iterator through OneStepNSProblems */
typedef OneStepNSProblems::const_iterator ConstOSNSIterator;

// ================== Misc ==================

/** default tolerance value, used to update index sets */
const double DEFAULT_TOLERANCE = 10 * MACHINE_PREC;

TYPEDEF_SPTR(OSISet);
TYPEDEF_SPTR(OneStepNSProblems);
TYPEDEF_SPTR(IndexInt);
#endif
