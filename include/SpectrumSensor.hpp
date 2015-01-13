#include "serial/serial.h"

namespace VESNA {

typedef long long ch_t;
typedef long long hz_t;
typedef long long ms_t;

class SweepConfig;

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

		hz_t base;
		hz_t spacing;
		hz_t bw;
		ch_t num;
		ms_t time;

		DeviceConfig(int id_, const std::string &name_, Device* device_) :
			id(id_), name(name_), device(device_) {};

		hz_t ch_to_hz(ch_t ch);
		ch_t hz_to_ch(hz_t hz);

		hz_t get_start_hz();
		hz_t get_stop_hz();

		bool covers(hz_t start_hz, hz_t stop_hz);

		SweepConfig* get_sample_config(hz_t hz, int nsamples);
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

class SweepConfig
{
	public:
		DeviceConfig* config;
		ch_t start_ch;
		ch_t stop_ch;
		ch_t step_ch;
		int nsamples;

		SweepConfig(DeviceConfig* config_, ch_t start_ch_, ch_t stop_ch_, ch_t step_ch_, 
				int nsamples_) :
			config(config_), start_ch(start_ch_), stop_ch(stop_ch_), step_ch(step_ch_),
			nsamples(nsamples_) {};
};

class SpectrumSensor
{
	public:
		SpectrumSensor(const std::string &port);
		~SpectrumSensor();

		ConfigList* get_config_list();
		void select_sweep_channel(SweepConfig* sc);

	private:
		serial::Serial *comm;

		void wait_for_ok();
};

};
