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
#include "SiconosMemory.h"
#include "BlockVector.h"
#include "SimpleVector.h"
#include "SiconosMemoryXML.h"

using namespace std;

// --- CONSTRUCTORS ---


// from data: maxSize
SiconosMemory::SiconosMemory(const unsigned int newValue):
  maxSize(newValue), nbVectorsInMemory(0)
{
  vectorMemory.reset(new MemoryContainer);
  vectorMemory->reserve(maxSize);
}

// from xml file + optional value of maxSize
SiconosMemory::SiconosMemory(SP::SiconosMemoryXML memXML):
  maxSize(0), nbVectorsInMemory(0), memoryXML(memXML)
{

  if (!memoryXML)
    SiconosMemoryException::selfThrow("SiconosMemory, xml constructor: xml file==NULL");

  maxSize = memoryXML->getSiconosMemorySize();
  vectorMemory.reset(new MemoryContainer);
  vectorMemory->reserve(maxSize);

  if (!memoryXML->hasMemory())
    SiconosMemoryException::selfThrow("SiconosMemory, xml constructor: no memory node found.");

  // get memory from xml file
  vectorMemory =  memoryXML->getSiconosMemoryVector();
  nbVectorsInMemory = vectorMemory->size();
}

// copy of a std::vector of siconos vectors
SiconosMemory::SiconosMemory(const MemoryContainer& V):
  maxSize(V.size()), nbVectorsInMemory(V.size())
{
  vectorMemory.reset(new MemoryContainer);
  vectorMemory->reserve(maxSize);
  for (unsigned int i = 0; i < maxSize; i++)
  {
    if (V[i]->isBlock())
      vectorMemory->push_back(SP::BlockVector(new BlockVector(*V[i])));
    else
      vectorMemory->push_back(SP::SimpleVector(new SimpleVector(*V[i])));
  }
}

// copy of a std::vector of siconos vectors  + maxSize
SiconosMemory::SiconosMemory(const unsigned int newMemorySize, const MemoryContainer& V):
  maxSize(newMemorySize), nbVectorsInMemory(V.size())
{
  vectorMemory.reset(new MemoryContainer);
  vectorMemory->reserve(maxSize);
  if (newMemorySize < V.size())
    SiconosMemoryException::selfThrow("SiconosMemory(int maxSize, vector<SP::SiconosVector> V) : V.size > maxSize");
  else
  {
    for (unsigned int i = 0; i < V.size(); i++)
    {
      if (V[i]->isBlock())
        vectorMemory->push_back(SP::BlockVector(new BlockVector(*V[i])));
      else
        vectorMemory->push_back(SP::SimpleVector(new SimpleVector(*V[i])));
    }
  }
}

// Destructor
SiconosMemory::~SiconosMemory()
{
  vectorMemory->clear();
}

// --- GETTERS/SETTERS ---

void SiconosMemory::setVectorMemory(const MemoryContainer& V)
{
  maxSize = V.size();
  vectorMemory->reserve(maxSize);
  nbVectorsInMemory = maxSize;
  vectorMemory->clear();
  for (unsigned int i = 0; i < V.size(); i++)
  {
    if (V[i]->isBlock())
      vectorMemory->push_back(SP::BlockVector(new BlockVector(*V[i])));
    else
      vectorMemory->push_back(SP::SimpleVector(new SimpleVector(*V[i])));
  }
}

SP::SiconosVector SiconosMemory::getSiconosVector(const unsigned int index) const
{
  assert(index < nbVectorsInMemory && "getSiconosVector(index) : inconsistent index value");
  return vectorMemory->at(index);
}

void SiconosMemory::swap(SP::SiconosVector v)
{
  unsigned int i;
  SP::SiconosVector tmp;
  double tmp2;

  // If vectorMemory size is maxSize, we remove its last element.
  if (vectorMemory->size() == maxSize)
    vectorMemory->resize(maxSize - 1);
  if (v->isBlock())
    vectorMemory->insert(vectorMemory->begin(), SP::BlockVector(new BlockVector(*v)));
  else // v = SimpleVector
    vectorMemory->insert(vectorMemory->begin(), SP::SimpleVector(new SimpleVector(*v)));
  nbVectorsInMemory = vectorMemory->size();
}

void SiconosMemory::display() const
{
  cout << " ====== Memory vector display ======= " << endl;
  cout << "| maxSize : " << maxSize << endl;
  cout << "| nbVectorsInMemory : " << nbVectorsInMemory << endl;
  cout << "| vectorMemory size : " << vectorMemory->size() << endl;
  for (unsigned int i = 0; i < nbVectorsInMemory; i++)
  {
    cout << "vector number " << i << ": adress = " << (*vectorMemory)[i] << " | " << endl; ;
    (*vectorMemory)[i]->display();
  }
  cout << " ===================================== " << endl;
}

void SiconosMemory::saveMemorySizeToXML()
{
  if (memoryXML) memoryXML->setSiconosMemorySize(maxSize);
  else SiconosMemoryException::selfThrow("SiconosMemory::saveMemorySizeToXML() - memoryXML object == NULL");
}
