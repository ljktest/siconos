#ifndef MBTB_CONTACTRELATION
#define MBTB_CONTACTRELATION
#include "SiconosKernel.hpp"

#include "MBTB_Contact.hpp"
//! It is a relation dedicated for the simple unilateral (ie: without  Coulomb friction).
/*!
  Aggregation to the class MBTB_Contact, the member _pContact contains the CAD informations.
  It derivates from Siconos::NewtonEulerFrom1DLocalFrameR. This class does the link between CAD and Siconos.
 */
class MBTB_ContactRelation : public NewtonEulerFrom1DLocalFrameR
{

protected:
  MBTB_Contact * _pContact;
  MBTB_ContactRelation();
public:
  //! Builder
  /*
    \param [in] a pointer to the MBTB_Contact. Must be allocated/free by the caller.
   */
  MBTB_ContactRelation(MBTB_Contact * pC);
  //!This function has to compute the distance between the objects.
  virtual void computeh(double time, Interaction &inter);
  //! Doing nothing.
  virtual ~MBTB_ContactRelation();

};
TYPEDEF_SPTR(MBTB_ContactRelation);


#endif
