#include "DeviceController.hpp"

DeviceController::DeviceController(Transceiver::I_ReceiveDataPush* rx, VESNA::I_SpectrumSensor* ss) :
	sensor(ss), receiver(rx)
{
	config_list = sensor->get_config_list();
}

DeviceController::~DeviceController()
{
	stop();
	delete config_list;
}

VESNA::ConfigList* DeviceController::get_config_list()
{
	return config_list;
}

void DeviceController::start(VESNA::SweepConfig* sc)
{
	stop();

	{
		boost::unique_lock<boost::mutex> lock(want_stop_m);
		want_stop = false;
	}

	thread = boost::thread(&DeviceController::loop, this, sc);
}

void DeviceController::stop()
{
	if(thread.joinable()) {
		{
			boost::unique_lock<boost::mutex> lock(want_stop_m);
			want_stop = true;
		}

		thread.join();
	}
}

void DeviceController::loop(VESNA::SweepConfig* sc)
{
	sensor->sample_run(sc, boost::bind(&DeviceController::cb, this, _1, _2));
}

bool DeviceController::cb(const VESNA::SweepConfig* sc, const VESNA::TimestampedData* samples)
{
	{
		boost::unique_lock<boost::mutex> lock(want_stop_m);
		if(want_stop) {
			return false;
		}
	}

	std::vector<Transceiver::BBSample> bbsamples;
	std::vector<VESNA::data_t>::const_iterator i = samples->data.begin();
	for(; i != samples->data.end(); ++i) {
		Transceiver::BBSample bb(*i, 0.);
		bbsamples.push_back(bb);
	}

	Transceiver::BBPacket packet(sc->nsamples, &bbsamples.front());
	//FIXME: Is it ok to always set endOfBurst to true here?
	receiver->pushBBSamplesRx(&packet, true);

	return true;
}
