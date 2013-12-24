#ifndef MBTB_TSPROJ_H
#define MBTB_TSPROJ_H
#include "SiconosKernel.hpp"
/**
 * \brief This class implements the time stepping with projection of a multi-bodies system.
 * It inherits from Siconos::TimeSteppingDirectProjection.
 * It consists in update the CAD word during the simulation.
 */
class MBTB_TimeSteppingProj : public TimeSteppingDirectProjection
{
public:
  //! builder.
  MBTB_TimeSteppingProj(SP::TimeDiscretisation td,
                        SP::OneStepIntegrator osi,
                        SP::OneStepNSProblem osnspb_velo,
                        SP::OneStepNSProblem osnspb_pos,
                        unsigned int level)
    :TimeSteppingDirectProjection(td,osi,osnspb_velo,osnspb_pos,level) {};
  //! Overloading of updateWorldFromDS.
  /*!
    It consists in updating the cad model from siconos.
   */
  virtual void updateWorldFromDS();  

};
TYPEDEF_SPTR(MBTB_TimeSteppingProj);
#endif
