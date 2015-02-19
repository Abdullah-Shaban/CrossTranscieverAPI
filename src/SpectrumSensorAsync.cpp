#include "SpectrumSensorAsync.hpp"

SpectrumSensorAsync::SpectrumSensorAsync(const std::string &port)
{
	ss = new VESNA::SpectrumSensor(port);
}

SpectrumSensorAsync::~SpectrumSensorAsync()
{
	delete ss;
}
