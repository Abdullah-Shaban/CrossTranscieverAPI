#include "SpectrumSensorAsync.hpp"

SpectrumSensorAsync::SpectrumSensorAsync(const std::string &port, Transceiver::I_ReceiveDataPush* rx)
{
	ss = new VESNA::SpectrumSensor(port);

	config_list = ss->get_config_list();
}

SpectrumSensorAsync::~SpectrumSensorAsync()
{
	delete ss;
}
