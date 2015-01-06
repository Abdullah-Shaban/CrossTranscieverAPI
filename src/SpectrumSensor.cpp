#include "SpectrumSensor.hpp"

#include <boost/regex.hpp>

#include <stdio.h>

using namespace VESNA;

void ConfigList::parse(std::vector<std::string> lines)
{
	std::vector<std::string>::iterator line = lines.begin();

	boost::regex rdevice("device ([0-9]+): (.*)\n");
	boost::regex rdevsam("  device supports channel sampling\n");
	boost::regex rconfig("  channel config [0-9]+,([0-9]+): (.*)\n");
	boost::regex rcparam("    ([a-z]+): ([0-9]+).*");

	for(; line != lines.end(); ++line) {
		boost::smatch g;
		if(boost::regex_match(*line, g, rdevice)) {
			int id = atoi(std::string(g[1]).c_str());
			std::string name = g[2];

			Device device(id, name);
			devices.push_back(device);
		} else if(boost::regex_match(*line, g, rdevsam)) {
			devices.rbegin()->supports_sampling = true;
		} else if(boost::regex_match(*line, g, rconfig)) {

			int id = atoi(std::string(g[1]).c_str());
			std::string name = g[2];

			DeviceConfig config(id, name, &(*devices.rbegin()));
			configs.push_back(config);
		} else if(boost::regex_match(*line, g, rcparam)) {
			std::string param = g[1];

			long long value = atoll(std::string(g[2]).c_str());

			if(!param.compare("base")) {
				configs.rbegin()->base = value;
			} else if(!param.compare("spacing")) {
				configs.rbegin()->spacing = value;
			} else if(!param.compare("num")) {
				configs.rbegin()->num = value;
			} else if(!param.compare("bw")) {
				configs.rbegin()->bw = value;
			} else if(!param.compare("time")) {
				configs.rbegin()->time = value;
			}
		}
	}
}

DeviceConfig* ConfigList::get_config(int device_id, int config_id)
{
	std::vector<DeviceConfig>::iterator i = configs.begin();
	for(; i != configs.end(); ++i) {
		if(i->id == config_id && i->device->id == device_id) {
			return &(*i);
		}
	}

	return NULL;
}

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
