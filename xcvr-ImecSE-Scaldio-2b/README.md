# WINNF transceiver facility implementation for the Imec Sensing Engine

This is the [WINNF transceiver facility][1] implementation for the [Imec Sensing Engine][2].

The implementation is based on a the C sensing abstraction library, enhanced for plug and play features for easy configuration of WARP based systems.

[1]: http://www.crew-project.eu/portal/transceiver-facility-specification
[2]: http://www.crew-project.eu/portal/imecdoc

## Compiling

### Transceiver facility library

Requirements:

 * imec sensing engine libraries for warp libraries

To compile:

    $ cmake CMakeLists.txt 
    $ make

## Implementation notes

### Device limitations

The Imec Sensing Engine is a receive only device, and hence there is no support for the TransmitChannel of the transceiver facility layer.

The Transceiver facility is specified for devices capable of supplying a continuous stream of signal samples to the application. However, the Imec Sensing Engine can only provide a packet of continuous samples up to a certain length (currently 24575 complex samples). In between two consecutive invocations of `pushBBSamplesRx` an unspecified length of time and dropped samples will occur.

A limitation of the Imec Sensing C API is that samples are captured in a debug mode, and the packet size is fixed at 24575 complex samples. The C++ API supports smaller sizes. These do not result in a speedup.

### Measured performance

The time to capture a set of samples depends on the amount of processing performed in the user defined pushBBSamplesRx() callback function. Typical recording times of a packet last 20 ms, this for packets up to 24575 samples. This when assuming a simple copy of a packet into a vector of samples.







