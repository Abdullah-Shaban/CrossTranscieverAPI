#include "SpectrumSensor.hpp"

using namespace VESNA;

SpectrumSensor::SpectrumSensor(const std::string &port)
{
	comm = new serial::Serial(port, 576000);
}

SpectrumSensor::~SpectrumSensor()
{
	delete comm;
}
