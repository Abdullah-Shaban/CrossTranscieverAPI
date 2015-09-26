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
#include "DeviceController.hpp"

DeviceController::DeviceController(Transceiver::I_ReceiveDataPush* rx, VESNA::I_SpectrumSensor* ss) :
	sensor(ss), receiver(rx)
{
	config_list = sensor->get_config_list();
}

DeviceController::~DeviceController()
{
	stop();
}

boost::shared_ptr<VESNA::ConfigList> DeviceController::get_config_list()
{
	return config_list;
}

void DeviceController::start(boost::shared_ptr<const VESNA::SweepConfig> sc)
{
	stop();

	{
		boost::unique_lock<boost::mutex> lock(want_stop_m);
		want_stop = false;
	}

	thread = boost::thread(&DeviceController::loop, this, sc);
}

void DeviceController::stop()
{
	if(thread.joinable()) {
		{
			boost::unique_lock<boost::mutex> lock(want_stop_m);
			want_stop = true;
		}

		thread.join();
	}
}

void DeviceController::loop(boost::shared_ptr<const VESNA::SweepConfig> sc)
{
	sensor->sample_run(sc, boost::bind(&DeviceController::cb, this, _1, _2));
}

bool DeviceController::cb(boost::shared_ptr<const VESNA::SweepConfig> sc,
		const VESNA::TimestampedData* samples)
{
	{
		boost::unique_lock<boost::mutex> lock(want_stop_m);
		if(want_stop) {
			return false;
		}
	}

	std::vector<Transceiver::BBSample> bbsamples;
	std::vector<VESNA::data_t>::const_iterator i = samples->data.begin();
	for(; i != samples->data.end(); ++i) {
		Transceiver::BBSample bb(*i, 0.);
		bbsamples.push_back(bb);
	}

	Transceiver::BBPacket packet(sc->nsamples, &bbsamples.front());
	//FIXME: Is it ok to always set endOfBurst to true here?
	receiver->pushBBSamplesRx(&packet, true);

	return true;
}
