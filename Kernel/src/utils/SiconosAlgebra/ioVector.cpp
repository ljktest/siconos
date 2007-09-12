#include "ioVector.h"
#include "SimpleVector.h"
#include "SiconosVectorException.h"
#include <boost/numeric/ublas/io.hpp>
#include<fstream>

// Default private
ioVector::ioVector(): ioObject() {}

ioVector::ioVector(const std::string& file, const std::string& mode): ioObject(file, mode) {}

ioVector::~ioVector(void) {}

const bool ioVector::read(SiconosVector& m) const
{

  std::ifstream infile;
  if (Mode == "ascii")
    infile.open(FileName.c_str(), std::ifstream::in);
  else if (Mode == "binary")
    infile.open(FileName.c_str(), std::ifstream::binary);
  else
    SiconosVectorException::selfThrow(" ioVector::read : Fail to open file \"" + FileName + "\"");

  if (!infile.good())
    SiconosVectorException::selfThrow("ioVector::read error : Fail to open \"" + FileName + "\"");

  if (m.isBlock())
    SiconosVectorException::selfThrow(" ioVector::read : read BlockVector is not implemented");

  infile.precision(15);

  DenseVect *p = m.getDensePtr();

  // Read the dimension of the vector in the first line of the input file
  // Just use to check that sizes are consistents.
  unsigned int dim;
  infile >> dim;

  if (dim != p->size())
    p->resize(dim);

  copy((std::istream_iterator<double>(infile)), std::istream_iterator<double>(), (p->data()).begin());

  infile.close();
  return true;
}

const bool ioVector::write(const SiconosVector& m, const std::string& outputType) const
{
  std::ofstream outfile;
  if (Mode == "ascii")
    outfile.open(FileName.c_str(), std::ofstream::out);
  else if (Mode == "binary")
    outfile.open(FileName.c_str(), std::ofstream::binary);
  else
    SiconosVectorException::selfThrow("ioVector::write - Incorrect mode for writing");

  if (!outfile.good())
    SiconosVectorException::selfThrow("ioVector:: write error : Fail to open \"" + FileName + "\"");

  if (m.isBlock())
    SiconosVectorException::selfThrow(" ioVector::write : write BlockVector is not implemented");

  outfile.precision(15);

  if (outputType != "noDim")
    outfile << m.size() << std::endl;

  if (m.getNum() == 1)
  {
    DenseVect*  p = m.getDensePtr();
    std::copy(p->begin(), p->end(), std::ostream_iterator<double>(outfile, " "));
  }
  else if (m.getNum() == 4)
  {
    SparseVect* p = m.getSparsePtr();
    std::copy(p->begin(), p->end(), std::ostream_iterator<double>(outfile, " "));
  }

  outfile.close();
  return true;
}

