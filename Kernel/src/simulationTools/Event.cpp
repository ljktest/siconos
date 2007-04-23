/* Siconos-Kernel version 2.0.1, Copyright INRIA 2005-2006.
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
#include "Event.h"
#include <math.h>
using namespace std;

// Default constructor (protected)
Event::Event(): type(DEFAULT_EVENT_TYPE), dTime(0.0)
{
  mpz_init(timeOfEvent);
}

double Event::tick = DEFAULT_TICK;

Event::Event(double time, const string& newType): type(newType), dTime(time)
{
  // Initialize and set timeOfEvent.
  mpz_init_set_d(timeOfEvent, ceil(time / tick)) ;
}

Event::~Event()
{
  mpz_clear(timeOfEvent);
}

void Event::display() const
{
  cout << "===== Event data display =====" << endl;
  cout << " - Type: " << type << endl;
  cout << " - time (mpz_t format, double format): (";
  mpz_out_str(stdout, 10, timeOfEvent);
  cout << ", " << dTime << ")" << endl;
  cout << "===== End of Event display =====" << endl;
}

