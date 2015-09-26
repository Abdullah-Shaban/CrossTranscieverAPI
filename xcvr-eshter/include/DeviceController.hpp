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
#ifndef HAVE_DEVICECONTROLLER_H
#define HAVE_DEVICECONTROLLER_H

// workaround for boost/thread.hpp bug
#include <time.h>
#undef TIME_UTC

#include "SpectrumSensor.hpp"
#include "transceiver.hpp"

#include <boost/thread.hpp>

class DeviceController
{
	public:
		DeviceController(Transceiver::I_ReceiveDataPush* rx, VESNA::I_SpectrumSensor* ss);
		~DeviceController();

		boost::shared_ptr<VESNA::ConfigList> get_config_list();

		void start(boost::shared_ptr<const VESNA::SweepConfig> sc);
		void stop();

		bool cb(boost::shared_ptr<const VESNA::SweepConfig> sc, const VESNA::TimestampedData* samples);

	private:
		boost::shared_ptr<VESNA::ConfigList> config_list;
		boost::thread thread;

		void loop(boost::shared_ptr<const VESNA::SweepConfig> sc);

		bool want_stop;
		boost::mutex want_stop_m;

		VESNA::I_SpectrumSensor* sensor;
		Transceiver::I_ReceiveDataPush* receiver;
};

#endif
