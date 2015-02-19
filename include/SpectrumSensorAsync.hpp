#ifndef HAVE_SPECTRUMSENSORASYNC_H
#define HAVE_SPECTRUMSENSORASYNC_H

#include "SpectrumSensor.hpp"
#include "transceiver.hpp"

class SpectrumSensorAsync
{
	public:
		SpectrumSensorAsync(const std::string &port, Transceiver::I_ReceiveDataPush* rx);
		~SpectrumSensorAsync();

		VESNA::SpectrumSensor *ss;
		VESNA::ConfigList *config_list;
};

#endif
