WINNF transceiver facility implementation for SNE-ESHTER
========================================================

This is the [WINNF transceiver facility][1] implementation for the [VESNA
spectrum sensor][2], specifically for the SNE ESHTER hardware.

The implementation is based on a C++ port of the vesna-spectrum-sensor Python
module. The `SpectrumSensor` C++ class should provide an almost identical
interface to the hardware as in Python. Transceiver facility interface is
implemented as a layer on top of the `SpectrumSensor` class.

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

    $ tests/OnLineTests

Implementation notes
--------------------

Error handling is largely missing on the transceiver facility layer (but is
implemented in `SpectrumSensor` class).

There is no support for signal transmission (e.g. `TransmitChannel` and related
classes). As the name implies, VESNA spectrum sensor is a receive-only device.

Event scheduling according to transceiver facility specification is mostly
complete and supports all discriminators (immediate, absolute, event based and
undefined). Event based discriminator correctly supports selection of event
count origin, event count and time offset. Not all possible combinations of
discriminators have been tested though and corner cases are not properly
detected. Events scheduled in the past or with impossible latencies might be
silently dropped or cause a deadlock.

Two events are available for scheduling: `receiveStartTime` and
`receiveStopTime`.

Event based discriminator supports up to one event in the past as the origin
(i. e. you can use event origin `Previous` with `eventCount >= 0`, but not
event origin `Beginning` with `eventCount` more than one event in the past)

`setReceiveStopTime()` is only supported when the original stop time used the
undefined discriminator.

`configureReceiveCycle()` is not currently supported at all.

While transceiver facility specifies nanosecond precision event scheduling, the
latency in this implementation is fairly bad. No real-time methods are used
(not supported in the current vesna-spectrum-sensor firmware). VESNA spectrum
sensor can be slow to stop reception, specifically with large packet sizes
(reception can not be interrupted in the middle of a packet).

Usage
-----

Not much. There is a `eshter_rx_cfile` utility available for recording signal
samples that mimics `uhd_rx_cfile` from the USRP hardware driver suite.

License
-------

transceiver.hpp is Copyright Thales Communications S.A

All other code Copyright (C) 2015 SensorLab, Jozef Stefan Institute
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
