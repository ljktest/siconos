/* Siconos is a program dedicated to modeling, simulation and control
 * of non smooth dynamical systems.
 *
 * Copyright 2016 INRIA.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/

/*!\file BouncingBallNETS.cpp
  \brief \ref EMBouncingBall - C++ input file, Time-Stepping version -
  V. Acary, O. Bonnefon.

  A Ball bouncing on the ground.
  Direct description of the model.
  Simulation with a Time-Stepping scheme.
*/
#include "SiconosKernel.hpp"

//#define WITH_PROJ
#define WITH_FC3D
using namespace std;
#ifdef WITH_FC3D
#define R_CLASS NewtonEulerFrom3DLocalFrameR
#else
#define R_CLASS NewtonEulerFrom1DLocalFrameR
#endif

class my_NewtonEulerR : public R_CLASS
{

  double _sBallRadius ;

public:

  my_NewtonEulerR(double radius): R_CLASS(), _sBallRadius(radius) { };

  virtual void computeOutput(double t, Interaction& inter, unsigned int derivativeNumber)
  {
    VectorOfBlockVectors& DSlink = inter.linkToDSVariables();
    if (derivativeNumber == 0)
    {
      computeh(t, *DSlink[NewtonEulerR::q0], *inter.y(0));
    }
    else
    {
      R_CLASS::computeOutput(t, inter,  derivativeNumber);
    }

  }
  void computeh(double time, BlockVector& q0, SiconosVector& y)
  {
    double height = fabs(q0.getValue(0)) - _sBallRadius;
    // std::cout <<"my_NewtonEulerR:: computeh _jachq" << std:: endl;
    // _jachq->display();
    y.setValue(0, height);
    _Nc->setValue(0, 1);
    _Nc->setValue(1, 0);
    _Nc->setValue(2, 0);
    _Pc1->setValue(0, height);
    _Pc1->setValue(1, q0.getValue(1));
    _Pc1->setValue(2, q0.getValue(2));

    //_Pc2->setValue(0,hpc);
    //_Pc2->setValue(1,data[q0]->getValue(1));
    //_Pc2->setValue(2,data[q0]->getValue(2));
    //printf("my_NewtonEulerR N, Pc\n");
    //_Nc->display();
    //_Pc1->display();
  }
  //ACCEPT_VISITORS();
};
TYPEDEF_SPTR(my_NewtonEulerR);





int main(int argc, char* argv[])
{
  try
  {


    // ================= Creation of the model =======================

    // User-defined main parameters
    unsigned int nDof = 3;           // degrees of freedom for the ball
    unsigned int qDim = 7;           // degrees of freedom for the ball
    unsigned int nDim = 6;           // degrees of freedom for the ball
    double t0 = 0;                   // initial computation time
    double T = 10.0;                  // final computation time
    double h = 0.005;                // time step
    double position_init = 1.0;      // initial position for lowest bead.
    double velocity_init = 2.0;      // initial velocity for lowest bead.
    double omega_initx = 0.0;
    double omega_initz = 0.0;// initial velocity for lowest bead.
    double theta = 0.5;              // theta for MoreauJeanOSI integrator
    double m = 1; // Ball mass
    double g = 9.81; // Gravity
    double radius = 0.1;
    // -------------------------
    // --- Dynamical systems ---
    // -------------------------

    cout << "====> Model loading ..." << endl << endl;

    // -- Initial positions and velocities --
    SP::SiconosVector q0(new SiconosVector(qDim));
    SP::SiconosVector v0(new SiconosVector(nDim));
    SP::SimpleMatrix I(new SimpleMatrix(3, 3));
    v0->zero();
    q0->zero();
    I->eye();
    (*q0)(0) = position_init;
    /*initial quaternion equal to (1,0,0,0)*/
    (*q0)(3) = 1.0;

    (*v0)(0) = velocity_init;
    (*v0)(3) = omega_initx;
    (*v0)(5) = omega_initz;
    // -- The dynamical system --
    SP::NewtonEulerDS ball(new NewtonEulerDS(q0, v0, m, I));

    // -- Set external forces (weight) --
    SP::SiconosVector weight(new SiconosVector(nDof));
    (*weight)(0) = -m * g;
    ball->setFExtPtr(weight);

    // --------------------
    // --- Interactions ---
    // --------------------

    // -- nslaw --
    double e = 0.9;

    // Interaction ball-floor
    //

    //     vector<SP::SiconosMatrix> vecMatrix1;
    //     vecMatrix1.push_back(H);
    //     SP::BlockMatrix H_block(new BlockMatrix(vecMatrix1,1,1));

    //     SP::SiconosMatrix HT(new SimpleMatrix(1,nDim));
    //     vector<SP::SiconosMatrix> vecMatrix2;
    //     vecMatrix2.push_back(HT);
    //     SP::BlockMatrix HT_block(new BlockMatrix(vecMatrix2,1,1));

#ifdef WITH_FC3D
    int nslawsize = 3;
    SP::NonSmoothLaw nslaw0(new NewtonImpactFrictionNSL(e, e, 0.6, 3));
#else
    int nslawsize = 1;
    SP::NonSmoothLaw nslaw0(new NewtonImpactNSL(e));
#endif


    //     Version with NewtonEulerR()
    //
    //     SP::SimpleMatrix H(new SimpleMatrix(nslawsize,qDim));
    //     H->zero();
    //     (*H)(0,0) = 1.0;
    // #ifdef WITH_FC3D
    //     (*H)(1,1) = 1.0;
    //     (*H)(2,2) = 1.0;
    // #endif
    //     //SP::NewtonEulerR relation0(new SphereNEDSPlanR(0.1,1.0,0.0,0.0,0.0));
    //     //SP::NewtonEulerR relation0(new NewtonEulerR());
    //     //relation0->setJachq(H);
    //     //    relation0->setJacQH(H_block);
    //     //    relation0->setJacQHT(HT_block);
    //     //cout<<"main jacQH"<<endl;
    //     //relation0->jachq()->display();

    // Version with my_NewtonEulerR()
    SP::NewtonEulerR relation0(new my_NewtonEulerR(radius));
    SP::Interaction inter(new Interaction(nslaw0, relation0));

    // -------------
    // --- Model ---
    // -------------
    SP::NonSmoothDynamicalSystem bouncingBall(new NonSmoothDynamicalSystem(t0, T));
    // add the dynamical system in the non smooth dynamical system
    bouncingBall->insertDynamicalSystem(ball);

    // link the interaction and the dynamical system
    bouncingBall->link(inter, ball);

    // ------------------
    // --- Simulation ---
    // ------------------

    // -- (1) OneStepIntegrators --
    SP::MoreauJeanGOSI OSI(new MoreauJeanGOSI(theta));

    // -- (2) Time discretisation --
    SP::TimeDiscretisation t(new TimeDiscretisation(t0, h));

    // -- (3) one step non smooth problem
    SP::OneStepNSProblem osnspb(new GlobalFrictionContact(3));
    // -- (4) Simulation setup with (1) (2) (3)

    SP::TimeStepping s(new TimeStepping(bouncingBall, t, OSI, osnspb));
    s->setNewtonTolerance(1e-10);
    s->setNewtonMaxIteration(10);

    // =========================== End of model definition ===========================

    // ================================= Computation =================================

    int N = ceil((T - t0) / h); // Number of time steps

    // --- Get the values to be plotted ---
    // -> saved in a matrix dataPlot
    unsigned int outputSize = 16;
    SimpleMatrix dataPlot(N + 1, outputSize);

    SP::SiconosVector q = ball->q();
    SP::SiconosVector v = ball->twist();
    SP::SiconosVector p = ball->p(1);
    SP::SiconosVector lambda = inter->lambda(1);

    dataPlot(0, 0) = bouncingBall->t0();
    dataPlot(0, 1) = (*q)(0);
    dataPlot(0, 2) = (*v)(0);
    dataPlot(0, 3) = (*p)(0);
    dataPlot(0, 4) = (*lambda)(0);
    dataPlot(0, 5) = acos((*q)(3));
    dataPlot(0, 6) = relation0->contactForce()->norm2();
    dataPlot(0, 7) = (*q)(0);
    dataPlot(0, 8) = (*q)(1);
    dataPlot(0, 9) = (*q)(2);
    dataPlot(0, 10) = (*q)(3);
    dataPlot(0, 11) = (*q)(4);
    dataPlot(0, 12) = (*q)(5);
    dataPlot(0, 13) = (*q)(6);
    dataPlot(0, 14) = (*v)(1);
    dataPlot(0, 15) = (*v)(2);

    // --- Time loop ---
    cout << "====> Start computation ... " << endl << endl;
    // ==== Simulation loop - Writing without explicit event handling =====
    int k = 1;
    boost::progress_display show_progress(N);

    boost::timer time;
    time.restart();
    dataPlot(k, 6) = relation0->contactForce()->norm2();
    while (s->hasNextEvent())
    {
      //      s->computeOneStep();
      s->advanceToEvent();
      // --- Get values to be plotted ---
      dataPlot(k, 0) =  s->nextTime();
      dataPlot(k, 1) = (*q)(0);
      dataPlot(k, 2) = (*v)(0);
      dataPlot(k, 3) = (*p)(0);
      dataPlot(k, 4) = (*lambda)(0);
      dataPlot(k, 5) = acos((*q)(3));
      dataPlot(k, 6) = relation0->contactForce()->norm2();
      dataPlot(k, 7) = (*q)(0);
      dataPlot(k, 8) = (*q)(1);
      dataPlot(k, 9) = (*q)(2);
      dataPlot(k, 10) = (*q)(3);
      dataPlot(k, 11) = (*q)(4);
      dataPlot(k, 12) = (*q)(5);
      dataPlot(k, 13) = (*q)(6);
      dataPlot(k, 14) = (*v)(1);
      dataPlot(k, 15) = (*v)(2);
      s->nextStep();
      ++show_progress;
      k++;
    }
    cout << endl << "End of computation - Number of iterations done: " << k - 1 << endl;
    cout << "Computation Time " << time.elapsed()  << endl;

    // --- Output files ---
    cout << "====> Output file writing ..." << endl;
    dataPlot.resize(k, outputSize);
    ioMatrix::write("result.dat", "ascii", dataPlot, "noDim");

   double error=0.0, eps=1e-12;
   if ((error=ioMatrix::compareRefFile(dataPlot, "BouncingBallNETS.ref", eps)) >= 0.0
       && error > eps)
     return 1;
  
  }

  catch (SiconosException e)
  {
    cout << e.report() << endl;
    return 1;
  }
  catch (...)
  {
    cout << "Exception caught in BouncingBallNETS.cpp" << endl;
  }

}