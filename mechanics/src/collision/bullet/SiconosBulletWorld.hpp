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

/*! \file SiconosBulletWorld.hpp
  \brief Definition of a Bullet-based interaction handler for contact
  detection.
*/

#ifndef SiconosBulletWorld_h
#define SiconosBulletWorld_h

#include <MechanicsFwd.hpp>

#include <SiconosMechanicsWorld.hpp>
#include <SiconosShape.hpp>
#include <SiconosContactor.hpp>

#include <map>

DEFINE_SPTR(SiconosBulletWorld_impl);

struct SiconosBulletOptions
{
  SiconosBulletOptions()
    : breakingThreshold(0.5)
    , worldScale(1.0)
    , useAxisSweep3(false)
    {}
  double breakingThreshold;
  double worldScale;
  bool useAxisSweep3;
};

struct SiconosBulletStatistics
{
  SiconosBulletStatistics()
    : new_interactions_created(0)
    , existing_interactions_processed(0)
    , interaction_warnings(0)
    {}
  int new_interactions_created;
  int existing_interactions_processed;
  int interaction_warnings;
};

class SiconosBulletWorld : public SiconosMechanicsWorld
{
protected:
  SP::SiconosBulletWorld_impl impl;

  void initialize_impl();

  // callback for contact point removal, and a global for context
  static bool bulletContactClear(void* userPersistentData);
  static SiconosBulletWorld *gBulletWorld;

public:
  SiconosBulletWorld();
  SiconosBulletWorld(const SiconosBulletOptions &options);
  virtual ~SiconosBulletWorld();

protected:
  SiconosBulletOptions _options;
  SiconosBulletStatistics _stats;

public:
  void insertStaticContactor(SP::SiconosContactor contactor);

  void updateInteractions(SP::Simulation simulation);
  SP::SiconosVisitor getDynamicalSystemsVisitor(SP::Simulation simulation);

  void insertNonSmoothLaw(SP::NonSmoothLaw, int group1, int group2);
  SP::NonSmoothLaw nonSmoothLaw(int group1, int group2);

  const SiconosBulletOptions &options() const { return _options; }
  const SiconosBulletStatistics &statistics() const { return _stats; }
  void resetStatistics() { _stats = SiconosBulletStatistics(); }
};

#endif /* SiconosBulletWorld.hpp */
