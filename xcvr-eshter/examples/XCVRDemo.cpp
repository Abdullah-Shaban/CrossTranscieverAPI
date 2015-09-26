/*
 * Copyright (C) 2015 SensorLab, Jozef Stefan Institute
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 *
 * Written by Tomaz Solc, tomaz.solc@ijs.si
 */
#include <stdio.h>

#include "SpectrumSensor.hpp"
#include "DeviceImp.hpp"

class DemoReceiver : public Transceiver::I_ReceiveDataPush
{
	public:
		void pushBBSamplesRx(Transceiver::BBPacket* thePushedPacket,
				Transceiver::Boolean endOfBurst)
		{
			printf("==> pushBBSamplesRx received %lu samples\n", thePushedPacket->SampleNumber);

			printf("    [");
			int n;
			for(n = 0; n < 5; n++) {
				printf("%f ", thePushedPacket->packet[n].valueI);
			}
			printf("...]\n");

		};
};

void undefined_stop_demo(DeviceImp* eshter)
{
	Transceiver::Time start(Transceiver::immediateDiscriminator);
	Transceiver::Time stop(Transceiver::undefinedDiscriminator);

	Transceiver::ULong packet_size = 2048;

	// 1 MHz bandwidth, 2 MHz sampling freq
	Transceiver::UShort tuning_preset = 2;

	// 700 MHz center frequency
	Transceiver::Frequency freq = 700000000;

	printf("  Creating receive cycle\n");
	Transceiver::ULong id;
	id = eshter->receiveChannel.createReceiveCycleProfile(start, stop, packet_size, tuning_preset, freq);

	printf("  Main thread sleeping for 5 seconds\n");
	// receive for 5 seconds
	usleep(5000000);
	printf("  Main thread waking up\n");

	printf("  Setting stop time to immediate\n");
	stop = Transceiver::Time(Transceiver::immediateDiscriminator);
	eshter->receiveChannel.setReceiveStopTime(id, stop);
}

void five_second_cycle_demo(DeviceImp* eshter)
{
	// start receiving immediately
	Transceiver::Time start(Transceiver::immediateDiscriminator);

	// stop receiving 5 seconds after start
	Transceiver::EventBasedTime event(Transceiver::receiveStartTime, 5000000000);
	event.eventCount = 0;
	Transceiver::Time stop(event);

	Transceiver::ULong packet_size = 2048;

	// 1 MHz bandwidth, 2 MHz sampling freq
	Transceiver::UShort tuning_preset = 2;

	// 700 MHz center frequency
	Transceiver::Frequency freq = 700000000;

	printf("  Creating receive cycle\n");
	eshter->receiveChannel.createReceiveCycleProfile(start, stop, packet_size, tuning_preset, freq);
}

int main(int argc, char** argv)
{
	char *device;
	int demo_num;

	if(argc != 3) {
		printf("USAGE: %s [device] [demo number]\n\ne.g. %s /dev/ttyUSB0 1\n", argv[0], argv[0]);
		return 1;
	} else {
		device = argv[1];
		demo_num = atoi(argv[2]);
	}

	DemoReceiver receiver;
	VESNA::SpectrumSensor sensor(device);

	printf("Creating transceiver object (if device is not connected, it will fail here)\n");
	DeviceImp* eshter = new DeviceImp(&receiver, &sensor);

	switch(demo_num) {
		case 1:
			undefined_stop_demo(eshter);
			break;

		case 2:
			five_second_cycle_demo(eshter);
			break;

		default:
			printf("Invalid demo number %d\n", demo_num);
			break;
	}

	printf("Destroying transceiver object (waits until all cycles are complete)\n");
	delete eshter;
	printf("Exiting\n");

	return 0;
}
