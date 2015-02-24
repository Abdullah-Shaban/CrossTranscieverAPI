#include "CppUTest/CommandLineTestRunner.h"

#include "DeviceController.hpp"

void create_config_list(VESNA::ConfigList& cl);

class TestReceiver : public Transceiver::I_ReceiveDataPush
{
	public:
		int sample_count;
		boost::mutex sample_count_m;

		void pushBBSamplesRx(Transceiver::BBPacket* thePushedPacket,
				Transceiver::Boolean endOfBurst) {
			boost::unique_lock<boost::mutex> lock(sample_count_m);
			sample_count += thePushedPacket->SampleNumber;
		};
		TestReceiver() : sample_count(0) {};
		~TestReceiver() {};
};

class TestSpectrumSensor : public VESNA::I_SpectrumSensor
{
	public:
		VESNA::ConfigList* get_config_list()
		{
			VESNA::ConfigList* cl = new VESNA::ConfigList();
			create_config_list(*cl);
			return cl;
		}

		void sample_run(const VESNA::SweepConfig* sc, VESNA::sample_run_cb_t cb, void* priv)
		{
			CHECK(700000000 == sc->start_ch);

			VESNA::TimestampedData samples;
			samples.channel = sc->start_ch;

			int n;
			for(n = 0; n < sc->nsamples; n++) {
				samples.data.push_back(0);
			}

			while(1) {
				bool cont = cb(sc, &samples, priv);
				if(!cont) {
					break;
				}

				samples.timestamp += 1.;
			}
		}
};

TEST_GROUP(DeviceControllerTestGroup)
{
};

TEST(DeviceControllerTestGroup, TestConstructor)
{
	TestReceiver rx;
	TestSpectrumSensor ss;
	DeviceController dc(&rx, &ss);
}

TEST(DeviceControllerTestGroup, TestGetConfigList)
{
	TestReceiver rx;
	TestSpectrumSensor ss;
	DeviceController dc(&rx, &ss);

	VESNA::ConfigList* cl = dc.get_config_list();

	CHECK_EQUAL(1, cl->get_device_num());
}

TEST(DeviceControllerTestGroup, TestStartStop)
{
	TestReceiver rx;
	TestSpectrumSensor ss;
	DeviceController dc(&rx, &ss);

	VESNA::ConfigList* cl = dc.get_config_list();
	VESNA::SweepConfig* sc = cl->get_config(0, 0)->get_sample_config(700000000, 100);

	dc.start(sc);

	while(1) {
		{
			boost::unique_lock<boost::mutex> lock(rx.sample_count_m);
			if(rx.sample_count == 1000) {
				break;
			}
		}
	}

	dc.stop();

	delete sc;
}
