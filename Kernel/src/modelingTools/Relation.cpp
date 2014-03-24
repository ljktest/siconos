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
#include "Relation.hpp"
#include "Interaction.hpp"
#include "PluggedObject.hpp"


// Default constructor
Relation::Relation(RELATION::TYPES newType,
                   RELATION::SUBTYPES newSub):
  _relationType(newType), _subType(newSub)
{
  zeroPlugin();
}

void Relation::zeroPlugin()
{
  _pluginh.reset(new PluggedObject());
  _pluginJachx.reset(new PluggedObject());
  _pluginJachz.reset(new PluggedObject());
  _pluginJachlambda.reset(new PluggedObject());
  _pluging.reset(new PluggedObject());
  _pluginJacLg.reset(new PluggedObject());
  _pluginf.reset(new PluggedObject());
  _plugine.reset(new PluggedObject());
}

Relation::~Relation()
{
}


const std::string Relation::getJachxName() const
{
  if (_pluginJachx->fPtr)
    return _pluginJachx->getPluginName();
  return "unamed";
}

const std::string Relation::gethName() const
{
  if (_pluginh->fPtr)
    return _pluginh->getPluginName();
  return "unamed";

}
const std::string Relation::getgName() const
{
  if (_pluging->fPtr)
    return _pluging->getPluginName();
  return "unamed";

}

const std::string Relation::getJacgName(unsigned int) const
{
  return "unamed";
}


void Relation::display() const
{
  std::cout << "=====> Relation of type "
            << _relationType
            << " and subtype "
            << _subType <<std::endl;
}

bool Relation::ishPlugged() const
{
  return _pluginh->isPlugged();
}

bool Relation::isJachxPlugged() const
{
  return _pluginJachx->isPlugged();
}

bool Relation::isJachlambdaPlugged() const
{
  return _pluginJachlambda->isPlugged();
}

bool Relation::isgPlugged() const
{
  return _pluging->isPlugged();
}

bool Relation::isJacLgPlugged() const
{
  return _pluginJacLg->isPlugged();
}

bool Relation::isfPlugged() const
{
  return _pluginf->isPlugged();
}

bool Relation::isePlugged() const
{
  return _plugine->isPlugged();
}


void Relation::computeg(double time, Interaction& inter)
{
  unsigned int i = inter.getRelativeDegree();
  if (i)
    i--;
  computeInput(time, inter, i);
}

void Relation::setComputeJachlambdaFunction(const std::string& pluginPath, const std::string& functionName)
{
  _pluginJachlambda->setComputeFunction(pluginPath, functionName);
}
void Relation::setComputeJachxFunction(const std::string& pluginPath, const std::string& functionName)
{
  _pluginJachx->setComputeFunction(pluginPath, functionName);
}

void Relation::setComputeJachzFunction(const std::string& pluginPath, const std::string& functionName)
{
  _pluginJachz->setComputeFunction(pluginPath, functionName);
}
void Relation::setComputegFunction(const std::string& pluginPath, const std::string& functionName)
{
  _pluging->setComputeFunction(pluginPath, functionName);
}
void Relation::setComputeFFunction(const std::string& pluginPath, const std::string& functionName)
{
  _pluginf->setComputeFunction(pluginPath, functionName);
}
void Relation::setComputeEFunction(const std::string& pluginPath, const std::string& functionName)
{
  _plugine->setComputeFunction(pluginPath, functionName);
}

void Relation::setComputeJacglambdaFunction(const std::string& pluginPath, const std::string& functionName)
{
  _pluginJacLg->setComputeFunction(pluginPath, functionName);
}

void Relation::setComputehFunction(const std::string& pluginPath, const std::string& functionName)
{
  _pluginh->setComputeFunction(pluginPath, functionName);
}
