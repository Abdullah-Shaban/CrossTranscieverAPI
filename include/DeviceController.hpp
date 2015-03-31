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

		VESNA::ConfigList* get_config_list();

		void start(boost::shared_ptr<const VESNA::SweepConfig> sc);
		void stop();

		bool cb(boost::shared_ptr<const VESNA::SweepConfig> sc, const VESNA::TimestampedData* samples);

	private:
		VESNA::ConfigList* config_list;
		boost::thread thread;

		void loop(boost::shared_ptr<const VESNA::SweepConfig> sc);

		bool want_stop;
		boost::mutex want_stop_m;

		VESNA::I_SpectrumSensor* sensor;
		Transceiver::I_ReceiveDataPush* receiver;
};

#endif
