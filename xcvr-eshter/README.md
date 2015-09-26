# WINNF transceiver facility implementation for SNE-ESHTER

This is the [WINNF transceiver facility][1] implementation for the [VESNA
spectrum sensor][2], specifically for the SNE ESHTER hardware.

The implementation is based on a C++ port of the vesna-spectrum-sensor Python
module. The `SpectrumSensor` C++ class should provide an almost identical
interface to the hardware as in Python. Transceiver facility interface is
implemented as a layer on top of the `SpectrumSensor` class.

This repository also includes a GNU Radio block built on top of the transceiver
facility interface. It allows a SNE ESHTER receiver to be included as a signal
source into a GNU Radio flow graph.

[1]: http://www.crew-project.eu/portal/transceiver-facility-specification
[2]: https://github.com/avian2/vesna-spectrum-sensor

## Compiling

### Transceiver facility library

Requirements:

 * Boost libraries (`apt-get install libboost1.49-dev`)
 * libserial (https://github.com/wjwwood/serial)
 * cpputest (`apt-get install libcpputest-dev`)

To compile:

    $ mkdir build
    $ cd build
    $ cmake ..
    $ make

To run off-line tests:

    $ make test

To run on-line tests (you need a device connected to `/dev/ttyUSB0`:

    $ tests/OnLineTests

To install:

    $ make install

### GNU Radio block

Requirements:

 * All of the steps above (including `make install`)
 * GNU Radio (v3.7.5.1 is known to work)

To compile:

    $ mkdir gr-xcvr/build
    $ cd gr-xcvr/build
    $ cmake ..
    $ make

To install:

    $ make install

A `SNE-ESHTER` block should now be visible in GNU Radio Companion under the
`XCVR API` category.

## Implementation notes

### Device limitations

There is no support for signal transmission (e.g. `TransmitChannel` and related
classes). As the name implies, VESNA spectrum sensor is a receive-only device.

Transceiver facility is specified for devices capable of supplying a continuous
stream of signal samples to the application. However,
vesna-spectrum-sensor can only provide a packet of continuous samples up to a
certain length (currently 25000 samples). In between two consecutive
invocations of `pushBBSamplesRx` an unspecified length of time and dropped
samples will occur.

Transceiver facility specifies complex signal samples, however
vesna-spectrum-sensor provides only real samples. Q values in `BBPacket.packet`
are always 0.

Transceiver tuning profiles are directly translated into vesna-spectrum-sensor
device configurations. In the context of this facility, SNE-ESHTER
configurations 2 (2 MHz sampling frequency/1 MHz bandwidth) and 3 (1 MHz
sampling frequency/500 kHz bandwidth) are most useful.

While transceiver facility specifies nanosecond precision event scheduling, the
latency in this implementation is fairly bad. No real-time methods are used
(nor supported in the current vesna-spectrum-sensor firmware).
vesna-spectrum-sensor can be slow to stop reception, specifically with large
packet sizes (reception can not be interrupted in the middle of a packet).

### Coverage of the transceiver facility specification

Error handling is largely missing on the transceiver facility layer (but is
implemented in `SpectrumSensor` class).

Event scheduling according to transceiver facility specification is mostly
complete and supports all discriminators (immediate, absolute, event based and
undefined). Event based discriminator correctly supports selection of event
count origin, event count and time offset. Not all possible combinations of
discriminators have been tested and likely there are unsupported corner cases
that are not detected properly. Events scheduled in the past or with impossible
latencies might be silently dropped or cause a deadlock.

Two events are available for use in event based discriminator:
`receiveStartTime` and `receiveStopTime`.

Event based discriminator supports up to one event in the past as the origin
(i. e. you can use event origin `Previous` with `eventCount >= 0`, but not
event origin `Beginning` with `eventCount` less than one event in the past)

`setReceiveStopTime()` is only supported when the original stop time used the
undefined discriminator.

`configureReceiveCycle()` is not currently supported at all.

### Regarding the GNU Radio block

In contrast to ordinary signal sources in GNU Radio, the `SNE-ESHTER` block
operates on vectors of samples. Each vector corresponds to one
`pushBBSamplesRx` call in the transceiver API. Hence, samples within one vector
are guaranteed to be consecutive, while an unspecified number of dropped
samples occurs between two vectors. Vector length is equal to `packet_size`.

Central frequency can be set during run time.

## Usage

There is a `eshter_rx_cfile` utility available for recording signal samples
that closely mimics `uhd_rx_cfile` from the USRP hardware driver suite (given
device limitations mentioned above)

    $ examples/eshter_rx_cfile --help

There is also a `eshter_xcvr_demo` that demonstrates a basic usage of the
transceiver facility interface. To run the demos, use:

    $ examples/eshter_xcvr_demo /dev/ttyUSB0 1
    $ examples/eshter_xcvr_demo /dev/ttyUSB0 2

An example GNU Radio flow graph using the SNE ESHTER block can be found under
`gr-xcvr/examples/eshter_fft_example.grc`

## License

transceiver.hpp is Copyright Thales Communications & Security S.A.S. (TCS),
Alejandro Sanchez, Somsai Thao, Eric Nicollet, eric.nicollet@thalesgroup.com

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
