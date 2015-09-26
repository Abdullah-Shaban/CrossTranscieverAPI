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
#ifndef HAVE_DEVICEIMP_H
#define HAVE_DEVICEIMP_H

// workaround for boost/thread.hpp bug
#include <time.h>
#undef TIME_UTC

#include "DeviceController.hpp"
#include "Scheduler.hpp"
#include "SpectrumSensor.hpp"
#include "transceiver.hpp"

#include <boost/thread.hpp>
#include <list>

class ReceiveCycleProfileEntry
{
	public:
		Transceiver::ReceiveCycleProfile* cycle;
		boost::shared_ptr<VESNA::SweepConfig> sc;
};

class ReceiveChannel : public Transceiver::I_ReceiveControl
{
	public:
		ReceiveChannel(Transceiver::I_ReceiveDataPush* rx, VESNA::I_SpectrumSensor* ss);
		~ReceiveChannel();

		Transceiver::ULong createReceiveCycleProfile(
				Transceiver::Time requestedReceiveStartTime,
				Transceiver::Time requestedReceiveStopTime,
				Transceiver::ULong requestedPacketSize,
				Transceiver::UShort requestedPresetId,
				Transceiver::Frequency requestedCarrierFrequency);
		void configureReceiveCycle(
				Transceiver::ULong targetCycleId,
				Transceiver::Time requestedReceiveStartTime,
				Transceiver::Time requestedReceiveStopTime,
				Transceiver::ULong requestedPacketSize,
				Transceiver::Frequency requestedCarrierFrequency) ;
		void setReceiveStopTime(
			Transceiver::ULong targetCycleId,
			Transceiver::Time requestedReceiveStopTime);

		void wait();

	private:
		DeviceController dc;
		Scheduler scheduler;

		Transceiver::ULong cycle_buffer_cnt;
		std::list<ReceiveCycleProfileEntry*> cycle_buffer;
		boost::mutex cycle_buffer_m;

		void start_cb(ReceiveCycleProfileEntry* e);
		void stop_cb(ReceiveCycleProfileEntry* e);
};

class DeviceImp
{
	public:
		DeviceImp(Transceiver::I_ReceiveDataPush* rx, VESNA::I_SpectrumSensor* ss);
		ReceiveChannel receiveChannel;
};

#endif
