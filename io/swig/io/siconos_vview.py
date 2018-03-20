#!/usr/bin/env @PYTHON_EXECUTABLE@

from siconos.io.vview import VView, VViewConfig, VViewOptions
from siconos.io.mechanics_hdf5 import MechanicsHdf5

if __name__=='__main__':
    ## Persistent configuration
    config = VViewConfig({'window_size': [600,600]})

    # Load it immediately
    config.load_configuration()

    # Parse command-line
    opts = VViewOptions()
    opts.parse()

    ## Options and config already loaded above
    with MechanicsHdf5(io_filename=opts.io_filename, mode='r') as io:
        vview = VView(io, opts, config)
        vview.run()

    # Update configuration and save it
    config['window_size'] = vview.renderer_window.GetSize()
    config.save_configuration(force=False)
