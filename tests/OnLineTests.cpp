#include <unistd.h>

#include "CppUTest/CommandLineTestRunner.h"

#include "SpectrumSensor.hpp"
#include "SpectrumSensorAsync.hpp"

TEST_GROUP(SpectrumSensorTestGroup)
{
};

TEST_GROUP(SpectrumSensorAsyncTestGroup)
{
};

TEST(SpectrumSensorTestGroup, TestError)
{
	VESNA::SpectrumSensor ss("/dev/ttyUSB0");

	ss.comm->write("invalid-command\n");

	std::string what;
	int t = 0;
	try {
		ss.wait_for_ok();
	} catch(VESNA::SpectrumSensorException e) {
		what = e.what();
		t = 1;
	}

	CHECK(!what.compare("error: unknown command: invalid-command"));
	CHECK(t == 1);
}

TEST(SpectrumSensorTestGroup, TestSpectrumSensor)
{
	VESNA::SpectrumSensor ss("/dev/ttyUSB0");
}

TEST(SpectrumSensorTestGroup, TestGetConfigList)
{
	VESNA::SpectrumSensor ss("/dev/ttyUSB0");

	VESNA::ConfigList* cl = ss.get_config_list();

	CHECK(cl != NULL);

	VESNA::DeviceConfig* c = cl->get_config(0, 0);

	CHECK(c != NULL);

	delete cl;
}

TEST(SpectrumSensorTestGroup, TestSelectSweepChannel)
{
	VESNA::SpectrumSensor ss("/dev/ttyUSB0");
	VESNA::ConfigList* cl = ss.get_config_list();
	VESNA::DeviceConfig* c = cl->get_config(0, 0);
	VESNA::SweepConfig* sc = c->get_sample_config(c->base, 100);

	ss.select_sweep_channel(sc);

	delete sc;
	delete cl;
}

int test_cb_cnt;
const unsigned nsamples = 25000;

bool test_cb(const VESNA::SweepConfig* sc, const VESNA::TimestampedData* samples, void* priv)
{
	CHECK(nsamples == samples->data.size());
	std::vector<VESNA::data_t>::const_iterator i = samples->data.begin();
	for(; i != samples->data.end(); ++i) {
		CHECK(*i >= 0.0);
		CHECK(*i < 4096.0);
	}

	test_cb_cnt++;
	if(test_cb_cnt >= 2) {
		return false;
	} else {
		return true;
	}
}

TEST(SpectrumSensorTestGroup, TestSampleRun)
{
	VESNA::SpectrumSensor ss("/dev/ttyUSB0");
	VESNA::ConfigList* cl = ss.get_config_list();

	VESNA::DeviceConfig* c = cl->get_config(0, 2);
	VESNA::SweepConfig* sc = c->get_sample_config(c->base, nsamples);

	test_cb_cnt = 0;
	ss.sample_run(sc, test_cb, NULL);

	CHECK_EQUAL(2, test_cb_cnt);

	delete sc;
	delete cl;
}

class TestReceiver : public Transceiver::I_ReceiveDataPush
{
	public:
		unsigned sample_cnt;

		void pushBBSamplesRx(Transceiver::BBPacket* thePushedPacket,
				Transceiver::Boolean endOfBurst) {
			sample_cnt += thePushedPacket->SampleNumber;
		};
		TestReceiver() : sample_cnt(0) {};
		~TestReceiver() {};
};

TEST(SpectrumSensorAsyncTestGroup, TestAsyncConstructor)
{
	TestReceiver rx;
	SpectrumSensorAsync ssa("/dev/ttyUSB0", &rx);

	CHECK_EQUAL(0, rx.sample_cnt);
}

TEST(SpectrumSensorAsyncTestGroup, TestAsyncRun)
{
	TestReceiver rx;
	SpectrumSensorAsync ssa("/dev/ttyUSB0", &rx);

	VESNA::ConfigList* cl = ssa.config_list;

	VESNA::DeviceConfig* c = cl->get_config(0, 2);
	VESNA::SweepConfig* sc = c->get_sample_config(c->base, nsamples);

	SpectrumSensorAsync::Command cmd1(SpectrumSensorAsync::Command::SAMPLE_ON, *sc);
	ssa.command(cmd1);

	delete sc;

	sleep(5);

	SpectrumSensorAsync::Command cmd2(SpectrumSensorAsync::Command::SAMPLE_OFF);
	ssa.command(cmd2);

	CHECK(rx.sample_cnt > 0);
}
