/* Siconos is a program dedicated to modeling, simulation and control
 * of non smooth dynamical systems.
 *
 * Copyright 2018 INRIA.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/
/*!\file PathSearch.h
 * \brief Path search related functions and data
 */

#ifndef PATHSEARCH_H
#define PATHSEARCH_H

#include "NMS.h"
#include "Newton_methods.h"

/** struct ncp_pathsearch_data NCP_PathSearch.h
 * solver specific data
 */
typedef struct
{
  NMS_data* data_NMS; /**< struct for the NMS scheme */
  functions_LSA* lsa_functions; /**< functions for the search */
} pathsearch_data;

#include "SiconosConfig.h"

#if defined(__cplusplus) && !defined(BUILD_AS_CPP)
extern "C"
{
#endif

  /** set some default value for the solver option when the path search
   * algorithm is used
   * \param options the structure to be modified
   */
  void pathsearch_default_SolverOption(SolverOptions* options);

  /** free solverData specific for the path search
   * \param solverData the struct to free
   */
  void free_solverData_PathSearch(void* solverData);


#if defined(__cplusplus) && !defined(BUILD_AS_CPP)
}
#endif

#endif
