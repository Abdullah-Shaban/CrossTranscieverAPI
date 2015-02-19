#ifndef HAVE_SPECTRUMSENSORASYNC_H
#define HAVE_SPECTRUMSENSORASYNC_H

#include "SpectrumSensor.hpp"
#include "transceiver.hpp"

class SpectrumSensorAsync
{
	public:
		class Command
		{
			public:
				enum Type { SAMPLE_ON, SAMPLE_OFF };

				Command(Type type_) : type(type_) {};
				Command(Type type_, const VESNA::SweepConfig& sweep_config_) :
					type(type_), sweep_config(sweep_config_) {};

				Type type;
				VESNA::SweepConfig sweep_config;
		};

		SpectrumSensorAsync(const std::string &port, Transceiver::I_ReceiveDataPush* rx);
		~SpectrumSensorAsync();

		void command(const Command& c);

		VESNA::SpectrumSensor *ss;
		VESNA::ConfigList *config_list;
};

#endif
