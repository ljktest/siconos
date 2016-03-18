/* Siconos-sample version 2.0.0, Copyright INRIA 2005-2011.
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
#include <SiconosKernel.hpp>

using namespace std;

int main(int argc, char* argv[])
{

  double t0 = 0.0;
  double T = 2e-3;    // Total simulation time
  double h_step = 5.0e-8;  // Time step
  double Lfvalue = 0.4e-3;   // inductance
  double Cfvalue = 2.2e-6;   // capacitance
  double Lrvalue = 150e-6;   // inductance
  double Crvalue = 68e-9;   // capacitance
  double Rvalue = 33.0; // resistance
  string Modeltitle = "PRC";
  string Author = "Ivan Merillas Santos";
  string Description = " ";
  string Date = "February 2006";
  string Bnamefunction = "Vgen";
  //double Vgen = sin(2*M_PI*55000*0)/fabs(sin(2*M_PI*55000*0));

  // TIMER
  boost::timer time;
  time.restart();

  try
  {

    // --- Dynamical system specification ---

    cout << "====> Model loading ..." << endl << endl;
    SP::SiconosVector init_state(new SiconosVector(4));
    (*init_state)(0) = 0.0;
    (*init_state)(1) = 0.0;
    (*init_state)(2) = 0.0;
    (*init_state)(3) = 0.0;

    SP::SimpleMatrix LS_A(new SimpleMatrix(4, 4));
    (*LS_A)(0 , 1) = -1.0 / Lrvalue;
    (*LS_A)(1 , 0) = 1.0 / Crvalue;
    (*LS_A)(2 , 3) = -1.0 / Lfvalue;
    (*LS_A)(3 , 2) = 1.0 / Cfvalue;
    (*LS_A)(3 , 3) = -1.0 / (Rvalue * Cfvalue);

    SP::FirstOrderLinearDS LSPRC(new FirstOrderLinearDS(init_state, LS_A));

    // Lrvalue is required in the plug-in, thus we set z[0] = 100.0/ Lrvalue.
    SP::SiconosVector z(new SiconosVector(1));

    // z[0] is used as a parameter in the plug-in.
    (*z)(0) = 1.0 / Lrvalue;
    LSPRC->setzPtr(z);
    LSPRC->setComputebFunction("PRCPlugin", "computeU");

    // --- Interaction between linear system and non smooth system ---

    // -> Relation
    SP::SimpleMatrix Int_C(new SimpleMatrix(4, 4));
    (*Int_C)(0 , 1) = -1.0;
    (*Int_C)(1 , 1) = 1.0;
    (*Int_C)(2 , 2) = 1.0;
    (*Int_C)(3 , 2) = 1.0;

    SP::SimpleMatrix Int_D(new SimpleMatrix(4, 4));
    (*Int_D)(0 , 2) = 1.0;
    (*Int_D)(1 , 3) = 1.0;
    (*Int_D)(2 , 0) = -1.0;
    (*Int_D)(3 , 1) = -1.0;

    SP::SimpleMatrix Int_B(new SimpleMatrix(4, 4));
    (*Int_B)(1 , 0) = -1.0 / Crvalue;
    (*Int_B)(1 , 1) = 1.0 / Crvalue;
    (*Int_B)(2 , 2) = 1.0 / Lfvalue;
    (*Int_B)(2 , 3) = 1.0 / Lfvalue;
    SP::FirstOrderLinearTIR LTIRPRC(new FirstOrderLinearTIR(Int_C, Int_B));
    LTIRPRC->setDPtr(Int_D);

    // -> Non-smooth law
    SP::NonSmoothLaw nslaw(new ComplementarityConditionNSL(4));

    SP::Interaction InterPRC(new Interaction(4, nslaw, LTIRPRC));


    // --- Model creation ---
    SP::Model PRC(new Model(t0, T, Modeltitle, Author, Description, Date));
    // add the dynamical system in the non smooth dynamical system
    PRC->nonSmoothDynamicalSystem()->insertDynamicalSystem(LSPRC);
    // link the interaction and the dynamical system
    PRC->nonSmoothDynamicalSystem()->link(InterPRC, LSPRC);

    // ------------------
    // --- Simulation ---
    // ------------------
    // -- (1) OneStepIntegrators --
    double theta = 0.5;
    SP::EulerMoreauOSI aOSI(new EulerMoreauOSI(theta));
    aOSI->insertDynamicalSystem(LSPRC);

    // -- (2) Time discretisation --
    SP::TimeDiscretisation aTiDisc(new TimeDiscretisation(t0, h_step));

    // -- (3) Non smooth problem
    SP::LCP aLCP(new LCP());

    // -- (4) Simulation setup with (1) (2) (3)
    SP::TimeStepping aTS(new TimeStepping(aTiDisc, aOSI, aLCP));


    // =========================== End of model definition ===========================

    // ================================= Computation =================================

    cout << "====> Simulation initialisation ..." << endl << endl;
    PRC->initialize(aTS);

    double h = aTS->timeStep();
    int N = ceil((T - t0) / h); // Number of time steps

    // --- Get the values to be plotted ---
    // -> saved in a matrix dataPlot
    SimpleMatrix dataPlot(N, 5);

    SP::SiconosVector x =  LSPRC->x();

    // For the initial time step:
    int k = 0;
    // time
    dataPlot(k, 0) = PRC->t0();

    // inductor voltage
    dataPlot(k, 1) = (*x)(0);

    // inductor current
    dataPlot(k, 2) = (*x)(1);

    // diode R1 current
    dataPlot(k, 3) = (*x)(2);

    // diode R1 voltage
    dataPlot(k, 4) = (*x)(3);

    // --- Time loop  ---
    cout << "====> Start computation ... " << endl << endl;
    for (k = 1 ; k < N ; ++k)
    {
      aTS->computeOneStep();
      // --- Get values to be plotted ---
      dataPlot(k, 0) = aTS->nextTime();
      dataPlot(k, 1) = (*x)(0);
      dataPlot(k, 2) = (*x)(1);
      dataPlot(k, 3) = (*x)(2);
      dataPlot(k, 4) = (*x)(3);
      // solve ...
      aTS->nextStep();
    }
    // Number of time iterations
    cout << "End of computation - Number of iterations done: " << k - 1 << endl;

    // dataPlot (ascii) output
    cout << "====> Output file writing ..." << endl;
    ioMatrix::write("PRC.dat", "ascii", dataPlot, "noDim");
  }

  // --- Exceptions handling ---
  catch (SiconosException e)
  {
    cout << e.report() << endl;
  }
  catch (...)
  {
    cout << "Exception caught " << endl;
  }
  cout << "Computation Time " << time.elapsed()  << endl;
}