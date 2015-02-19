#ifndef HAVE_SPECTRUMSENSORASYNC_H
#define HAVE_SPECTRUMSENSORASYNC_H

#include "SpectrumSensor.hpp"

class SpectrumSensorAsync
{
	public:
		SpectrumSensorAsync(const std::string &port);
		~SpectrumSensorAsync();

		VESNA::SpectrumSensor *ss;
		VESNA::ConfigList *config_list;
};

#endif
