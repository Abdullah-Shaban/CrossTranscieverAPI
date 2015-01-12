#include "serial/serial.h"

namespace VESNA {

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

		long long base;
		long long spacing;
		long long bw;
		long long num;
		long long time;

		DeviceConfig(int id_, const std::string &name_, Device* device_) :
			id(id_), name(name_), device(device_) {};

		long long ch_to_hz(long long ch);
		long long hz_to_ch(long long hz);

		long long get_start_hz();
		long long get_stop_hz();

		bool covers(long long start_hz, long long stop_hz);
};

class ConfigList
{
	public:
		ConfigList() {};
		void parse(std::vector<std::string> lines);

		DeviceConfig* get_config(int device_id, int config_id);

	private:
		std::vector<DeviceConfig> configs;
		std::vector<Device> devices;
};

class SpectrumSensor
{
	public:
		SpectrumSensor(const std::string &port);
		~SpectrumSensor();

		ConfigList* get_config_list();

	private:
		serial::Serial *comm;

		void wait_for_ok();
};

};
