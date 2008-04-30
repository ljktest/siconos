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
#include "OSNSMatrix.h"
#include "SparseBlockMatrix.h"
#include "Tools.h"

using namespace std;
void OSNSMatrix::updateSizeAndPositions(unsigned int * dim, UnitaryRelationsSet* indexSet)
{
  // === Description ===
  // For a unitaryBlock (diagonal or extra diagonal) corresponding to a Unitary Relation, we need to know the position of its first element
  // in the full-matrix M. This position depends on the previous unitaryBlocks sizes.
  //
  // positions are saved in a map<UnitaryRelation*, unsigned int>, named unitaryBlocksPositions.
  //

  // Computes real size of the current matrix = sum of the dim. of all UR in indexSet
  *dim = 0;
  for (UnitaryRelationsIterator it = indexSet->begin(); it != indexSet->end(); ++it)
  {
    (*unitaryBlocksPositions)[*it] = *dim;
    *dim += (*it)->getNonSmoothLawSize();
  }
}
void OSNSMatrix::updateSizeAndPositions(unsigned int * dim, DynamicalSystemsSet* DSSet)
{
  // === Description ===
  // For a DSBlock (diagonal or extra diagonal) corresponding to a DynamicalSet, we need to know the position of its first element
  // in the full-matrix M. This position depends on the previous DSBlocks sizes.
  //
  // positions are saved in a map<DynamicalSystem*, unsigned int>, named DSBlocksPositions.
  //

  // Computes real size of the current matrix = sum of the dim. of all UR in indexSet
  *dim = 0;
  for (DSIterator it = DSSet->begin(); it != DSSet->end(); ++it)
  {
    (*DSBlocksPositions)[*it] = *dim;
    *dim += (*it)->getDim();
  }
}
void OSNSMatrix::updateSizeAndPositions(unsigned int * dim, DynamicalSystemsSet* DSSet, UnitaryRelationsSet* indexSet)
{
  // === Description ===
  // positions are saved in a map<UnitaryRelation*, unsigned int>, named unitaryBlocksPositions.
  // positions are saved in a map<DynamicalSystem*, unsigned int>, named DSBlocksPositions.
  //

  // Computes real size of the current matrix = sum of the dim. of all UR in indexSet
  *dim = 0;
  for (DSIterator it = DSSet->begin(); it != DSSet->end(); ++it)
  {
    (*DSBlocksPositions)[*it] = *dim;
    *dim += (*it)->getDim();
  }
  for (UnitaryRelationsIterator it = indexSet->begin(); it != indexSet->end(); ++it)
  {
    (*unitaryBlocksPositions)[*it] = *dim;
    *dim += (*it)->getNonSmoothLawSize();
  }
}
// Default constructor: empty matrix, default storage
// No allocation for M1 or M2
OSNSMatrix::OSNSMatrix():
  dimRow(0), storageType(0), unitaryBlocksPositions(NULL), DSBlocksPositions(NULL), numericsMat(NULL), M1(NULL), M2(NULL)
{
  unitaryBlocksPositions = new UR_int();
  DSBlocksPositions = new DS_int();
  numericsMat = new NumericsMatrix;
}

// Constructor with dimensions (one input: square matrix only)
OSNSMatrix::OSNSMatrix(unsigned int n, int stor):
  dimRow(n), storageType(stor), unitaryBlocksPositions(NULL), DSBlocksPositions(NULL),  numericsMat(NULL), M1(NULL), M2(NULL)
{
  // Note:
  // for storageType = 0 (dense) n represents the real dimRowension of the matrix
  // and for sparse storage (storageType == 1) the number of unitaryBlocks in a row or column.

  if (storageType == 0)
  {
    // A zero matrix M of size nXn is built.
    // unitaryBlocksPositions remains empty (=NULL) since we have no information concerning the UR.
    M1 = new SimpleMatrix(n, n);
  }
  else // if(storageType == 1)
    M2 = new SparseBlockMatrix(n);
  unitaryBlocksPositions = new UR_int();
  DSBlocksPositions = new DS_int();
  numericsMat = new NumericsMatrix;
}
OSNSMatrix::OSNSMatrix(unsigned int n, unsigned int m, int stor):
  dimRow(n),  dimColumn(m), storageType(stor), unitaryBlocksPositions(NULL), DSBlocksPositions(NULL), numericsMat(NULL), M1(NULL), M2(NULL)
{
  // Note:
  // for storageType = 0 (dense) n represents the real dimRowension of the matrix
  // and for sparse storage (storageType == 1) the number of unitaryBlocks in a row or column.

  if (storageType == 0)
  {
    // A zero matrix M of size nXn is built.
    // unitaryBlocksPositions remains empty (=NULL) since we have no information concerning the UR.
    M1 = new SimpleMatrix(n, m);
  }
  else // if(storageType == 1)
    //  M2 = new SparseBlockMatrix(n);

    unitaryBlocksPositions = new UR_int();
  DSBlocksPositions = new DS_int();
  numericsMat = new NumericsMatrix;
}

// Basic constructor
OSNSMatrix::OSNSMatrix(UnitaryRelationsSet* indexSet, MapOfMapOfUnitaryMatrices& unitaryBlocks, int stor):
  dimRow(0), storageType(stor), unitaryBlocksPositions(NULL), DSBlocksPositions(NULL), numericsMat(NULL), M1(NULL), M2(NULL)
{
  unitaryBlocksPositions = new UR_int();
  DSBlocksPositions = new DS_int();
  numericsMat = new NumericsMatrix;

  fill(indexSet, unitaryBlocks);
}

OSNSMatrix::OSNSMatrix(DynamicalSystemsSet* DSSet, MapOfDSMatrices& DSBlocks, int stor):
  dimRow(0), storageType(stor), unitaryBlocksPositions(NULL), DSBlocksPositions(NULL), numericsMat(NULL), M1(NULL), M2(NULL)
{
  unitaryBlocksPositions = new UR_int();
  DSBlocksPositions = new DS_int();
  numericsMat = new NumericsMatrix;

  fill(DSSet, DSBlocks);
}
OSNSMatrix::OSNSMatrix(DynamicalSystemsSet* DSSet, UnitaryRelationsSet* indexSet, MapOfDSMapOfUnitaryMatrices& DSUnitaryBlocks, int stor):
  dimRow(0), storageType(stor), unitaryBlocksPositions(NULL), DSBlocksPositions(NULL), numericsMat(NULL), M1(NULL), M2(NULL)
{
  unitaryBlocksPositions = new UR_int();
  DSBlocksPositions = new DS_int();
  numericsMat = new NumericsMatrix;

  fill(DSSet, indexSet, DSUnitaryBlocks);
}

OSNSMatrix::OSNSMatrix(UnitaryRelationsSet* indexSet, DynamicalSystemsSet* DSSet,  MapOfUnitaryMapOfDSMatrices& unitaryDSBlocks, int stor):
  dimRow(0), storageType(stor), unitaryBlocksPositions(NULL), DSBlocksPositions(NULL), numericsMat(NULL), M1(NULL), M2(NULL)
{
  unitaryBlocksPositions = new UR_int();
  DSBlocksPositions = new DS_int();
  numericsMat = new NumericsMatrix;

  fill(indexSet, DSSet, unitaryDSBlocks);
}

OSNSMatrix::OSNSMatrix(UnitaryRelationsSet* indexSet, DynamicalSystemsSet* DSSet, MapOfMapOfUnitaryMatrices& unitaryBlocks,   MapOfDSMatrices& DSBlocks, MapOfDSMapOfUnitaryMatrices& DSUnitaryBlocks, MapOfUnitaryMapOfDSMatrices& unitaryDSBlocks, int stor):
  dimRow(0), storageType(stor), unitaryBlocksPositions(NULL), DSBlocksPositions(NULL), numericsMat(NULL), M1(NULL), M2(NULL)
{
  unitaryBlocksPositions = new UR_int();
  DSBlocksPositions = new DS_int();
  numericsMat = new NumericsMatrix;

  fill(indexSet, DSSet, unitaryBlocks, DSBlocks, DSUnitaryBlocks, unitaryDSBlocks);
}






// Copy of a SiconosMatrix (used when OSNS xml constructor is called with M input in XML file)
OSNSMatrix::OSNSMatrix(const SiconosMatrix& MSource):
  dimRow(MSource.size(0)), storageType(0), unitaryBlocksPositions(NULL), DSBlocksPositions(NULL), numericsMat(NULL), M1(NULL), M2(NULL)
{
  unitaryBlocksPositions = new UR_int();
  DSBlocksPositions = new DS_int();
  numericsMat = new NumericsMatrix;
  M1 = new SimpleMatrix(MSource);
  // Warning: unitaryBlocksPositions remains empty since we have no information concerning indexSet and unitaryBlocks in MSource
}


// Destructor
OSNSMatrix::~OSNSMatrix()
{
  unitaryBlocksPositions->clear();
  delete unitaryBlocksPositions;
  unitaryBlocksPositions = NULL;
  DSBlocksPositions->clear();
  delete DSBlocksPositions;
  DSBlocksPositions = NULL;
  delete numericsMat;
  numericsMat = NULL;
  if (M1 != NULL) delete M1;
  M1 = NULL;
  if (M2 != NULL) delete M2;
  M2 = NULL;
}

unsigned int OSNSMatrix::getPositionOfUnitaryBlock(UnitaryRelation* UR) const
{
  UR_int::const_iterator it = unitaryBlocksPositions->find(UR);
  if (it == unitaryBlocksPositions->end())
    RuntimeException::selfThrow("OSNSMatrix::getPositionOfUnitaryBlock(UnitaryRelation ur), ur does not belong to the index set used to built the OSNS matrix.");
  return it->second;
}
unsigned int OSNSMatrix::getPositionOfDSBlock(DynamicalSystem* DS) const
{
  DS_int::const_iterator it = DSBlocksPositions->find(DS);
  if (it == DSBlocksPositions->end())
    RuntimeException::selfThrow("OSNSMatrix::getPositionOfDSBlock(DynamicalSystems ds), ds does not belong to the DynamicalSet used to built the OSNS matrix.");
  return it->second;
}

// Fill the SparseMat
void OSNSMatrix::fill(UnitaryRelationsSet* indexSet, MapOfMapOfUnitaryMatrices& unitaryBlocks, bool updateSize)
{
  if (indexSet == NULL)
    RuntimeException::selfThrow("OSNSMatrix::fill(IndexInt* i, ...), i is a null pointer");

  if (updateSize)
  {
    // Computes dimRow and unitaryBlocksPositions according to indexSet
    updateSizeAndPositions(&dimRow, indexSet);
  }

  if (storageType == 0)
  {
    // === Memory allocation, if required ===
    // Mem. is allocate only if M==NULL or if its size has changed.
    if (M1 == NULL)
      M1 = new SimpleMatrix(dimRow, dimRow);
    else
    {
      if (M1->size(0) != dimRow || M1->size(1) != dimRow)
        M1->resize(dimRow, dimRow);
      M1->zero();
    }

    // ======>  Aim: find UR1 and UR2 both in indexSet and which have common DynamicalSystems.
    // Then get the corresponding matrix from map unitaryBlocks, and copy it into M

    unsigned int pos = 0, col = 0; // index position used for unitaryBlock copy into M, see below.
    // === Loop through "active" Unitary Relations (ie present in indexSets[level]) ===
    for (UnitaryRelationsIterator itRow = indexSet->begin(); itRow != indexSet->end(); ++itRow)
    {
      // Look for UR connected (ie with common DS) to the current one
      // The connection is checked thanks to unitaryBlocks map
      for (UnitaryMatrixColumnIterator itCol = unitaryBlocks[*itRow].begin(); itCol != unitaryBlocks[*itRow].end(); ++itCol)
      {
        // We keep connected UR only if they are also in indexSets[level]
        if ((indexSet->find((*itCol).first)) != indexSet->end())
        {
          // UR1 = *itRow
          // UR2 = *itCol
          // corresponding matrix = unitaryBlocks[*itRow][(*itCol).first]

          // Case 1: basic storage
          pos = (*unitaryBlocksPositions)[*itRow];
          col = (*unitaryBlocksPositions)[(*itCol).first];
          static_cast<SimpleMatrix*>(M1)->setBlock(pos, col, *(unitaryBlocks[*itRow][(*itCol).first]));
        }
      }
    }
  }
  else // if storageType == 1
  {
    if (M2 == NULL)
      M2 = new SparseBlockMatrix(indexSet, unitaryBlocks);
    else
      M2->fill(indexSet, unitaryBlocks);
  }
  convert();
}
void OSNSMatrix::fillDiagonal(UnitaryRelationsSet* URSet, MapOfMapOfUnitaryMatrices& unitaryBlocks, bool updateSize)
{
  if (URSet == NULL)
    RuntimeException::selfThrow("OSNSMatrix::fill(IndexInt* i, ...), i is a null pointer");

  if (updateSize)
  {
    // Computes dimRow and unitaryBlocksPositions according to indexSet
    updateSizeAndPositions(&dimRow, URSet);
  }

  if (storageType == 0)
  {
    // === Memory allocation, if required ===
    // Mem. is allocate only if M==NULL or if its size has changed.
    if (M1 == NULL)
      M1 = new SimpleMatrix(dimRow, dimRow);
    else
    {
      if (M1->size(0) != dimRow || M1->size(1) != dimRow)
        M1->resize(dimRow, dimRow);
      M1->zero();
    }

    // ======>  Aim: find UR1 and UR2 both in indexSet and which have common DynamicalSystems.
    // Then get the corresponding matrix from map unitaryBlocks, and copy it into M

    unsigned int pos = 0, col = 0; // index position used for unitaryBlock copy into M, see below.
    // === Loop through "active" Unitary Relations (ie present in indexSets[level]) ===
    for (UnitaryRelationsIterator itRow = URSet->begin(); itRow != URSet->end(); ++itRow)
    {
      pos = (*unitaryBlocksPositions)[*itRow];
      static_cast<SimpleMatrix*>(M1)->setBlock(pos, pos, *(unitaryBlocks[*itRow][*itRow]));
    }
  }
  else // if storageType == 1
  {
    if (M2 == NULL)
      M2 = new SparseBlockMatrix(URSet, unitaryBlocks);
    else
      M2->fill(URSet, unitaryBlocks);
  }
  convert();
}

void OSNSMatrix::fill(DynamicalSystemsSet* DSSet, MapOfDSMatrices& DSBlocks, bool updateSize)
{
  // RuntimeException::selfThrow("OSNSMatrix::fill(DynamicalSystemsSet*, MapOfDSMatrices&), Not Yet Implemented");

  if (DSSet == NULL)
    RuntimeException::selfThrow("OSNSMatrix::fill(DynamicalSystemsSet* DSSet, ...), i is a null pointer");
  if (updateSize)
  {
    // Computes dimRow and unitaryBlocksPositions according to indexSet
    updateSizeAndPositions(&dimRow, DSSet);
  }
  if (storageType == 0)
  {
    // === Memory allocation, if required ===
    // Mem. is allocate only if M==NULL or if its size has changed.
    if (M1 == NULL)
      M1 = new SimpleMatrix(dimRow, dimRow);
    else
    {
      if (M1->size(0) != dimRow || M1->size(1) != dimRow)
        M1->resize(dimRow, dimRow);
      M1->zero();
    }

    // get the corresponding matrix from map unitaryBlocks, and copy it into M

    unsigned int pos = 0, col = 0; // index position used for unitaryBlock copy into M, see below.
    // === Loop through "active" Unitary Relations (ie present in indexSets[level]) ===
    for (DSIterator itDS = DSSet->begin(); itDS != DSSet->end(); ++itDS)
    {
      // DS = *itDS
      // corresponding matrix = DSBlocks[*itDS]

      // Case 1: basic storage
      pos = (*DSBlocksPositions)[*itDS];
      static_cast<SimpleMatrix*>(M1)->setBlock(pos, pos, *(DSBlocks[*itDS]));
    }
  }
  else // if storageType == 1
  {
    if (M2 == NULL)
      M2 = new SparseBlockMatrix(DSSet, DSBlocks);
    else
      M2->fill(DSSet, DSBlocks);
  }
  convert();

}
void OSNSMatrix::fill(DynamicalSystemsSet* DSSet, UnitaryRelationsSet* URSet, MapOfDSMapOfUnitaryMatrices& DSUnitaryBlocks, bool updateSize)
{
  if (URSet == NULL)
    RuntimeException::selfThrow("OSNSMatrix::fill(DynamicalSystemsSet* DSSet, UnitaryRelationsSet* URSet, MapOfDSMapOfUnitaryMatrices& DSUnitaryBlocks,bool updateSize)");
  if (DSSet == NULL)
    RuntimeException::selfThrow("OSNSMatrix::fill(DynamicalSystemsSet* DSSet, UnitaryRelationsSet* URSet, MapOfDSMapOfUnitaryMatrices& DSUnitaryBlocks,bool updateSize)");

  if (updateSize)
  {
    // Computes dimRow and unitaryBlocksPositions and  DSBlocksPositions according to indexSet
    updateSizeAndPositions(&dimColumn, URSet);


    updateSizeAndPositions(&dimRow, DSSet);
  }

  if (storageType == 0)
  {
    // === Memory allocation, if required ===
    // Mem. is allocate only if M==NULL or if its size has changed.
    if (M1 == NULL)
      M1 = new SimpleMatrix(dimRow, dimColumn);
    else
    {
      if (M1->size(0) != dimRow || M1->size(1) != dimColumn)
        M1->resize(dimRow, dimColumn);
      M1->zero();
    }


    // Get the  matrix from map unitaryDSBlocks which corresponds to UR and DS, and copy it into M

    unsigned int pos = 0, col = 0; // index position used for unitaryBlock copy into M, see below.
    // === Loop through "active" Unitary Relations (ie present in indexSets[level]) ===


    for (DSIterator itCol = DSSet->begin(); itCol != DSSet->end(); ++itCol)
    {
      // Look for DS
      // The connection is checked thanks to unitaryBlocks map
      for (UnitaryMatrixColumnIterator itRow = DSUnitaryBlocks[*itCol].begin(); itRow != DSUnitaryBlocks[*itCol].end(); ++itCol)
      {
        // DS = *itCol
        // UR = *itRow
        // corresponding matrix = DSUnitaryBlocks[*itCol][(*itRow).first]

        // Case 1: basic storage
        pos = (*DSBlocksPositions)[*itCol];
        col = (*unitaryBlocksPositions)[(*itRow).first];
        static_cast<SimpleMatrix*>(M1)->setBlock(pos, col, *(DSUnitaryBlocks[*itCol][(*itRow).first]));
      }
    }

  }
  else // if storageType == 1
  {
    RuntimeException::selfThrow("Not yet Implemented case storageType == 1:OSNSMatrix::fill(DynamicalSystemsSet* DSSet, UnitaryRelationsSet* URSet, MapOfDSMapOfUnitaryMatrices& DSUnitaryBlocks,bool updateSize) ");

    //       if(M2==NULL)
    //  M2 = new SparseBlockMatrix(DSSet,URSet,DSUnitaryBlocks);
    //       else
    //  M2->fill(DSSet,URSet,DSUnitaryBlocks);
  }
  convert();

}
void OSNSMatrix::fill(UnitaryRelationsSet* indexSet, DynamicalSystemsSet* DSSet,  MapOfUnitaryMapOfDSMatrices& unitaryDSBlocks, bool updateSize)
{
  if (indexSet == NULL)
    RuntimeException::selfThrow("OSNSMatrix::fill(UnitaryRelationsSet* indexSet,DynamicalSystemsSet* DSSet,  MapOfUnitaryMapOfDSMatrices& unitaryDSBlocks) , indexSet is a null pointer");
  if (DSSet == NULL)
    RuntimeException::selfThrow("OSNSMatrix::fill(UnitaryRelationsSet* indexSet,DynamicalSystemsSet* DSSet,  MapOfUnitaryMapOfDSMatrices& unitaryDSBlocks) , DSset is a null pointer");

  if (updateSize)
  {
    // Computes dimRow and unitaryBlocksPositions and  DSBlocksPositions according to indexSet
    updateSizeAndPositions(&dimRow, indexSet);


    updateSizeAndPositions(&dimColumn, DSSet);
  }

  if (storageType == 0)
  {
    // === Memory allocation, if required ===
    // Mem. is allocate only if M==NULL or if its size has changed.
    if (M1 == NULL)
      M1 = new SimpleMatrix(dimRow, dimColumn);
    else
    {
      if (M1->size(0) != dimRow || M1->size(1) != dimColumn)
        M1->resize(dimRow, dimColumn);
      M1->zero();
    }


    // Get the  matrix from map unitaryDSBlocks which corresponds to UR and DS, and copy it into M

    unsigned int pos = 0, col = 0; // index position used for unitaryBlock copy into M, see below.
    // === Loop through "active" Unitary Relations (ie present in indexSets[level]) ===


    for (UnitaryRelationsIterator itRow = indexSet->begin(); itRow != indexSet->end(); ++itRow)
    {
      // Look for UR
      // The connection is checked thanks to unitaryBlocks map
      for (MatIterator itCol = unitaryDSBlocks[*itRow].begin(); itCol != unitaryDSBlocks[*itRow].end(); ++itCol)
      {
        // UR = *itRow
        // DS= *itCol
        // corresponding matrix = unitaryDSBlocks[*itUR][(*itCol).first]

        // Case 1: basic storage
        pos = (*unitaryBlocksPositions)[*itRow];
        col = (*DSBlocksPositions)[(*itCol).first];
        static_cast<SimpleMatrix*>(M1)->setBlock(pos, col, *(unitaryDSBlocks[*itRow][(*itCol).first]));
      }
    }

  }
  else // if storageType == 1
  {
    if (M2 == NULL)
      M2 = new SparseBlockMatrix(indexSet, DSSet, unitaryDSBlocks);
    else
      M2->fill(indexSet, DSSet, unitaryDSBlocks);
  }
  convert();
}
void OSNSMatrix::fill(UnitaryRelationsSet* URSet, DynamicalSystemsSet* DSSet,  MapOfMapOfUnitaryMatrices& unitaryBlocks,  MapOfDSMatrices& DSBlocks, MapOfDSMapOfUnitaryMatrices& DSUnitaryBlocks ,  MapOfUnitaryMapOfDSMatrices& unitaryDSBlocks , bool updateSize)
{
  updateSizeAndPositions(&dimRow, DSSet, URSet);
  dimColumn = dimRow;
  //Fill the diagonal of the DSs block
  fill(DSSet, DSBlocks, false);
  //Fill the diagonal of the URs block
  fillDiagonal(URSet, unitaryBlocks, false);
  //Fill DSxUR
  fill(DSSet, URSet, DSUnitaryBlocks, false);
  //Fill URxDS
  fill(URSet, DSSet, unitaryDSBlocks, false);
}


// convert current matrix to NumericsMatrix structure
void OSNSMatrix::convert()
{
  numericsMat->storageType = storageType;
  numericsMat->size0 = dimRow;
  numericsMat->size1 = dimRow;
  if (storageType == 0)
  {
    numericsMat->matrix0 = M1->getArray(); // Pointer link
    // numericsMat->matrix1 = NULL;
    // matrix1 is not set to NULL: we keep previous allocation. May be usefull if we switch between different storages during simu
  }
  else
  {
    M2->convert();
    numericsMat->matrix1 = M2->getNumericsMatSparse();
  }
}

// Display data
void OSNSMatrix::display() const
{
  if (storageType == 0)
  {
    cout << "----- OSNS Matrix using default storage type for Numerics structure (SiconosMatrix -> double*)" << endl;
    if (M1 == NULL)
      cout << " matrix = NULL pointer" << endl;
    else M1->display();
  }
  else
  {
    cout << "----- OSNS Matrix using Sparse UnitaryBlock storage type for Numerics (SparseBlockStructuredMatrix)" << endl;
    if (M2 == NULL)
      cout << " matrix = NULL pointer" << endl;
    else M2->display();
  }
}
