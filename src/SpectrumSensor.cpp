#include "SpectrumSensor.hpp"

#include <stdio.h>

using namespace VESNA;

SpectrumSensor::SpectrumSensor(const std::string &port)
{
	comm = new serial::Serial(port, 576000, serial::Timeout::simpleTimeout(1000));
	
	comm->write("sweep-off\n");
	wait_for_ok();
	comm->write("sample-off\n");
	wait_for_ok();
}

SpectrumSensor::~SpectrumSensor()
{
	delete comm;
}

void SpectrumSensor::wait_for_ok()
{
	while(1) {
		std::string r = comm->readline();
		if(!r.compare("ok\n")) {
			return;
		}
	}
}
