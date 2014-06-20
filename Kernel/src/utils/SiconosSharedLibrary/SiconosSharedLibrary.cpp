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
#include "SiconosSharedLibrary.hpp"
#include "SiconosSharedLibraryException.hpp"

#include <map>
#include <dlfcn.h>                      // for dlerror, dlclose, dlopen, etc
#include <stddef.h>                     // for NULL
#include <iostream>                     // for operator<<, basic_ostream, etc
#include <utility>                      // for make_pair, pair

namespace SiconosSharedLibrary
{

std::multimap<const std::string, PluginHandle> openedPlugins;
typedef std::multimap<const std::string, PluginHandle>::iterator iter;

PluginHandle loadPlugin(const std::string& pluginPath)
{
  PluginHandle HandleRes;
#ifdef _WIN32
  HandleRes = LoadLibrary(pluginPath.c_str());
  if (!HandleRes)
  {
    DWORD err = GetLastError();
    std::cout << "SiconosSharedLibrary::loadPlugin Error returned : " << err << std::endl;
    std::cout << "Arguments: pluginPath = " << pluginPath << std::endl;
    SiconosSharedLibraryException::selfThrow("SiconosSharedLibrary::loadPlugin, can not open or find " + pluginPath);
  }
#endif
#ifdef _SYS_UNX
  HandleRes = dlopen(pluginPath.c_str(), RTLD_LAZY);
  if (!HandleRes)
  {
    std::cout << "dlerror() :" << dlerror() <<std::endl;
    SiconosSharedLibraryException::selfThrow("SiconosSharedLibrary::loadPlugin, can not open or find " + pluginPath);
  }
#endif
  openedPlugins.insert(std::make_pair(pluginPath, HandleRes));
  return HandleRes;
}

void * getProcAddress(PluginHandle plugin, const std::string& procedure)
{
  void* ptr;
#ifdef _WIN32
  ptr = (void*) GetProcAddress(plugin, procedure.c_str());
  if (NULL == ptr)
  {
    DWORD err = GetLastError();
    std::cout << "SiconosSharedLibrary::getProcAddress Error returned : " << err << std::endl;
    std::cout << "Arguments: procedure = " << procedure << std::endl;
    SiconosSharedLibraryException::selfThrow("SiconosSharedLibrary::getProcAddress, can not find procedure " + procedure);
  }
#endif
#ifdef _SYS_UNX
  ptr = dlsym(plugin, procedure.c_str());
  if (ptr == NULL)
  {
    std::cout << "SiconosSharedLibrary::getProcAddress Error returned : " << dlerror() << std::endl;
    std::cout << "Arguments: procedure = " << procedure << std::endl;
    throw SiconosSharedLibraryException("SiconosSharedLibrary::getProcAddress, can not find procedure " + procedure);
  }
#endif
  return ptr;
}

void closePlugin(const std::string& pluginFile)
{
  iter it = openedPlugins.find(pluginFile);
  if (it == openedPlugins.end())
  {
    std::cout << "SiconosSharedLibrary::closePlugin - could not find an opened plugin named " << pluginFile << std::endl;
    std::cout << "Plugins in openedPlugins:" << std::endl;
    for (iter it2 = openedPlugins.begin(); it2 != openedPlugins.end(); ++it2) std::cout <<  it2->first << std::endl;
    SiconosSharedLibraryException::selfThrow("SiconosSharedLibrary::closePlugin - could not find an opened plugin named " + pluginFile);
  }
  PluginHandle plugin = it->second;
#ifdef _WIN32
  FreeLibrary(plugin);
#endif
#ifdef _SYS_UNX
  dlclose(plugin);
#endif

  openedPlugins.erase(it);
}

}
