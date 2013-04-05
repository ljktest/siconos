/* Siconos-Numerics, Copyright INRIA 2005-2011.
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
#include "NumericsOptions.h"

/* Default value for verbose mode: turned to off
Warning: global variable
*/
int verbose = 0;
void setNumericsVerbose(int newVerboseMode)
{
  verbose = newVerboseMode;
}

void setNumericsOptions(NumericsOptions* opt)
{
  verbose = opt->verboseMode;
}

void numericsError(char * functionName, char* message)
{
  char output[200] = "Numerics error - ";
  strcat(output, functionName);
  strcat(output, message);
  strcat(output, ".\n");
  fprintf(stderr, "%s", output);
  exit(EXIT_FAILURE);
}

void numericsWarning(char * functionName, char* message)
{
  char output[200] = "Numerics warning - ";
  strcat(output, functionName);
  strcat(output, message);
  strcat(output, ".\n");
  fprintf(stderr, "%s", output);
  exit(EXIT_FAILURE);
}

void setDefaultNumericsOptions(NumericsOptions* opts)
{
  opts->verboseMode = 0;
  opts->outputMode = 0;
  opts->counter = 0;
  strcpy(opts->fileName, "NumericsDefaultOutputFile");
  strcpy(opts->title, "none");
  strcpy(opts->description, "without description");
  strcpy(opts->math_info, "none");
}
