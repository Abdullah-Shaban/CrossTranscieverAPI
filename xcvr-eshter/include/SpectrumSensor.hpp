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
#ifndef HAVE_SPECTRUMSENSOR_H
#define HAVE_SPECTRUMSENSOR_H

#include "serial/serial.h"
#include "boost/shared_ptr.hpp"
#include "boost/function.hpp"

namespace VESNA {

typedef long long ch_t;
typedef long long hz_t;
typedef long long ms_t;

typedef double data_t;

class SweepConfig;

class SpectrumSensorException: public std::exception
{
	public:
		SpectrumSensorException(const char *w) : what_(w) {};
		SpectrumSensorException(const std::string &w) : what_(w) {};
		~SpectrumSensorException() throw() {};

		const char *what() const throw();
	private:

		std::string what_;
};

class Device
{
	public:
		int id;
		std::string name;
		bool supports_sampling;

		Device(int id_, const std::string &name_) :
			id(id_), name(name_), supports_sampling(false) {};
};

class DeviceConfig
{
	public:
		int id;
		std::string name;
		Device *device;

		hz_t base;
		hz_t spacing;
		hz_t bw;
		ch_t num;
		ms_t time;

		DeviceConfig(int id_, const std::string &name_, Device* device_) :
			id(id_), name(name_), device(device_) {};

		hz_t ch_to_hz(ch_t ch);
		ch_t hz_to_ch(hz_t hz);

		hz_t get_start_hz();
		hz_t get_stop_hz();

		bool covers(hz_t start_hz, hz_t stop_hz);

		boost::shared_ptr<SweepConfig> get_sample_config(hz_t hz, int nsamples);
};

class ConfigList
{
	public:
		ConfigList() {};
		void parse(std::vector<std::string> lines);

		DeviceConfig* get_config(int device_id, int config_id);

		int get_config_num();
		int get_device_num();

	private:
		std::vector<DeviceConfig> configs;
		std::vector<Device> devices;
};

class SweepConfig
{
	public:
		DeviceConfig* config;
		ch_t start_ch;
		ch_t stop_ch;
		ch_t step_ch;
		int nsamples;

		SweepConfig() {};
		SweepConfig(DeviceConfig* config_, ch_t start_ch_, ch_t stop_ch_, ch_t step_ch_, 
				int nsamples_) :
			config(config_), start_ch(start_ch_), stop_ch(stop_ch_), step_ch(step_ch_),
			nsamples(nsamples_) {};
};

class TimestampedData
{
	public:
		double timestamp;
		ch_t channel;

		std::vector<data_t> data;

		bool parse(std::string s, int ch_num = -1);
};

typedef boost::function<bool (boost::shared_ptr<const VESNA::SweepConfig> sc,
		const VESNA::TimestampedData* samples)> sample_run_cb_t;

class I_SpectrumSensor
{
	public:
		virtual boost::shared_ptr<ConfigList> get_config_list() = 0;
		virtual void sample_run(boost::shared_ptr<const SweepConfig> sc, sample_run_cb_t cb) = 0;
};

class SpectrumSensor : public I_SpectrumSensor
{
	public:
		serial::Serial *comm;

		SpectrumSensor(const std::string &port);
		~SpectrumSensor();

		boost::shared_ptr<ConfigList> get_config_list();
		void sample_run(boost::shared_ptr<const SweepConfig> sc, sample_run_cb_t cb);

		void select_sweep_channel(boost::shared_ptr<const SweepConfig> sc);
		void wait_for_ok();
};

};

#endif
