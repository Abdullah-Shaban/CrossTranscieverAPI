WINNF transceiver facility implementation for SNE-ESHTER
========================================================

This is Work In Progress of the [WINNF transceiver facility][1] implementation
for the [VESNA spectrum sensor][2], specifically for the SNE ESHTER hardware.

The implementation is based on a C++ port of the vesna-spectrum-sensor Python
module. The SpectrumSensor C++ class should provide an almost identical
interface to the hardware as in Python. Transceiver facility interface is
implemented as a thin layer on top of the SpectrumSensor class.

[1]: http://www.crew-project.eu/portal/transceiver-facility-specification
[2]: https://github.com/avian2/vesna-spectrum-sensor

Compiling
---------

Requirements:

 * Boost libraries (`apt-get install libboost1.49-dev`)
 * libserial (https://github.com/wjwwood/serial)
 * cpputest (`apt-get install libcpputest-dev`)

To compile:

    $ mkdir build
    $ cmake ..
    $ make

To run on-line tests (you need a device connected to `/dev/ttyUSB0`:

    $ tests/AllTests -gOnLineTestGroup

Usage
-----

Not much. There is a `eshter_rx_cfile` utility available for recording signal
samples that mimics `uhd_rx_cfile` from the USRP hardware driver suite.

License
-------

Copyright (C) 2015 SensorLab, Jozef Stefan Institute
http://sensorlab.ijs.si

Written by Tomaz Solc, tomaz.solc@ijs.si

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see http://www.gnu.org/licenses/
