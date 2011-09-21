/* Siconos-Kernel, Copyright INRIA 2005-2011.
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
#include "SiconosMemory.hpp"
#include "BlockVector.hpp"
#include "SimpleVector.hpp"
#include "SiconosMemoryXML.hpp"

using namespace std;

// --- CONSTRUCTORS ---


// from data: _maxSize
SiconosMemory::SiconosMemory(const unsigned int newValue):
  _maxSize(newValue), _nbVectorsInMemory(0)
{
  _vectorMemory.reset(new MemoryContainer);
  _vectorMemory->reserve(_maxSize);
}

// from xml file + optional value of _maxSize
SiconosMemory::SiconosMemory(SP::SiconosMemoryXML memXML):
  _maxSize(0), _nbVectorsInMemory(0), memoryXML(memXML)
{

  if (!memoryXML)
    SiconosMemoryException::selfThrow("SiconosMemory, xml constructor: xml file==NULL");

  _maxSize = memoryXML->getSiconosMemorySize();
  _vectorMemory.reset(new MemoryContainer);
  _vectorMemory->reserve(_maxSize);

  if (!memoryXML->hasMemory())
    SiconosMemoryException::selfThrow("SiconosMemory, xml constructor: no memory node found.");

  // get memory from xml file
  _vectorMemory =  memoryXML->getSiconosMemoryVector();
  _nbVectorsInMemory = _vectorMemory->size();
}

// copy of a std::vector of siconos vectors
SiconosMemory::SiconosMemory(const MemoryContainer& V):
  _maxSize(V.size()), _nbVectorsInMemory(V.size())
{
  _vectorMemory.reset(new MemoryContainer);
  _vectorMemory->reserve(_maxSize);
  for (unsigned int i = 0; i < _maxSize; i++)
  {
    if (V[i]->isBlock())
      _vectorMemory->push_back(SP::BlockVector(new BlockVector(*V[i])));
    else
      _vectorMemory->push_back(SP::SimpleVector(new SimpleVector(*V[i])));
  }
}

// copy of a std::vector of siconos vectors  + _maxSize
SiconosMemory::SiconosMemory(const unsigned int newMemorySize, const MemoryContainer& V):
  _maxSize(newMemorySize), _nbVectorsInMemory(V.size())
{
  _vectorMemory.reset(new MemoryContainer);
  _vectorMemory->reserve(_maxSize);
  if (newMemorySize < V.size())
    SiconosMemoryException::selfThrow("SiconosMemory(int _maxSize, vector<SP::SiconosVector> V) : V.size > _maxSize");
  else
  {
    for (unsigned int i = 0; i < V.size(); i++)
    {
      if (V[i]->isBlock())
        _vectorMemory->push_back(SP::BlockVector(new BlockVector(*V[i])));
      else
        _vectorMemory->push_back(SP::SimpleVector(new SimpleVector(*V[i])));
    }
  }
}

// Destructor
SiconosMemory::~SiconosMemory()
{
  _vectorMemory->clear();
}

// --- GETTERS/SETTERS ---

void SiconosMemory::setVectorMemory(const MemoryContainer& V)
{
  _maxSize = V.size();
  _vectorMemory->reserve(_maxSize);
  _nbVectorsInMemory = _maxSize;
  _vectorMemory->clear();
  for (unsigned int i = 0; i < V.size(); i++)
  {
    if (V[i]->isBlock())
      _vectorMemory->push_back(SP::BlockVector(new BlockVector(*V[i])));
    else
      _vectorMemory->push_back(SP::SimpleVector(new SimpleVector(*V[i])));
  }
}

SP::SiconosVector SiconosMemory::getSiconosVector(const unsigned int index) const
{
  assert(index < _nbVectorsInMemory && "getSiconosVector(index) : inconsistent index value");
  return _vectorMemory->at(index);
}

void SiconosMemory::swap(SP::SiconosVector v)
{
  SP::SiconosVector tmp;

  // If vectorMemory size is _maxSize, we remove its last element.
  if (_vectorMemory->size() == _maxSize)
    _vectorMemory->resize(_maxSize - 1);
  if (v->isBlock())
    _vectorMemory->insert(_vectorMemory->begin(), SP::BlockVector(new BlockVector(*v)));
  else // v = SimpleVector
    _vectorMemory->insert(_vectorMemory->begin(), SP::SimpleVector(new SimpleVector(*v)));
  _nbVectorsInMemory = _vectorMemory->size();
}

void SiconosMemory::display() const
{
  cout << " ====== Memory vector display ======= " << endl;
  cout << "| _maxSize : " << _maxSize << endl;
  cout << "| _nbVectorsInMemory : " << _nbVectorsInMemory << endl;
  cout << "| vectorMemory size : " << _vectorMemory->size() << endl;
  for (unsigned int i = 0; i < _nbVectorsInMemory; i++)
  {
    cout << "vector number " << i << ": adress = " << (*_vectorMemory)[i] << " | " << endl; ;
    (*_vectorMemory)[i]->display();
  }
  cout << " ===================================== " << endl;
}

void SiconosMemory::saveMemorySizeToXML()
{
  if (memoryXML) memoryXML->setSiconosMemorySize(_maxSize);
  else SiconosMemoryException::selfThrow("SiconosMemory::saveMemorySizeToXML() - memoryXML object == NULL");
}
