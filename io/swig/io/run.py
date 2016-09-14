#!/usr/bin/env python

import argparse
from siconos.io.mechanics_io import Hdf5

parser = argparse.ArgumentParser(
    description = 'Run a pre-generated Siconos HDF5 simulation file.',
    epilog = """Note that this script only provides a basic interface for the most
    common simulation options.  For more complex options, or to
    specify behaviour such as controllers, you must create a custom
    simulation script.  If a partially-completed simulation is found,
    it will be continued from the last time step until T.""")

parser.add_argument('file', metavar='filename', type=str, nargs=1,
                    help = 'simulation file (HDF5)')
parser.add_argument('-T', metavar='time', type=float,
                    help = 'time in seconds to run until (default T=1 second)',
                    default=1)
parser.add_argument('-p', '--period', metavar='period', type=float,
                    help = 'time in seconds between frames (default p=5e-3)',
                    default=5e-3)
parser.add_argument('-e','--every', metavar='interval', type=int,
                    help = 'output every nth frame (default=1)', default=1)
parser.add_argument('-f','--frequency', metavar='Hz', type=float,
                    help = 'alternative to -p, specify simulation frequency in Hz'+
                    ' (default=200 Hz)')

args = parser.parse_args()

if args.frequency is not None:
    args.p = 1.0 / args.frequency

# Run the simulation from the inputs previously defined and add
# results to the hdf5 file. The visualisation of the output may be done
# with the vview command.
with Hdf5(mode='r+',io_filename=args.file[0]) as io:

    # By default earth gravity is applied and the units are those
    # of the International System of Units.
    # Because of fixed collision margins used in the collision detection,
    # sizes of small objects may need to be expressed in cm or mm.
    io.run(output_frequency=args.every, T=args.T, h=args.period)
