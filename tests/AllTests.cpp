#include "CppUTest/CommandLineTestRunner.h"

#include "DeviceImp.hpp"
#include "SpectrumSensor.hpp"

TEST_GROUP(OffLineTestGroup)
{
};

TEST(OffLineTestGroup, TestConstructor)
{
	DeviceImp di;
}

static void create_config_list(VESNA::ConfigList& cl)
{
	std::vector<std::string> lines;
	lines.push_back("device 0: Test device\n");
	lines.push_back("  channel config 0,0: Test config\n");
	lines.push_back("    base: 0 Hz\n");
	lines.push_back("    spacing: 1 Hz\n");
	lines.push_back("    bw: 1 Hz\n");
	lines.push_back("    num: 1000000000\n");
	lines.push_back("    time: 5 ms\n");

	cl.parse(lines);
}

TEST(OffLineTestGroup, TestConfigList)
{
	VESNA::ConfigList cl;

	create_config_list(cl);
	VESNA::DeviceConfig *c = cl.get_config(0, 0);

	CHECK(c != NULL);

	CHECK_EQUAL(0, c->id);
	STRCMP_EQUAL("Test config", c->name.c_str());

	CHECK(0 == c->base);
	CHECK(1 == c->spacing);
	CHECK(1 == c->bw);
	CHECK(1000000000 == c->num);
	CHECK(5 == c->time);

	CHECK_EQUAL(c->device->id, 0);
	STRCMP_EQUAL(c->device->name.c_str(), "Test device");
	CHECK_EQUAL(c->device->supports_sampling, false);
}

TEST(OffLineTestGroup, TestConfigListGetNum)
{
	VESNA::ConfigList cl;

	create_config_list(cl);

	CHECK_EQUAL(cl.get_config_num(), 1);
	CHECK_EQUAL(cl.get_device_num(), 1);
}

TEST(OffLineTestGroup, TestChToHz)
{
	VESNA::ConfigList cl;
	create_config_list(cl);

	VESNA::DeviceConfig *c = cl.get_config(0, 0);

	CHECK(c->ch_to_hz(0) == c->base);
}

TEST(OffLineTestGroup, TestHzToCh)
{
	VESNA::ConfigList cl;
	create_config_list(cl);

	VESNA::DeviceConfig *c = cl.get_config(0, 0);

	CHECK(c->hz_to_ch(c->base) == 0);
}

TEST(OffLineTestGroup, TestChToHzToCh)
{
	VESNA::ConfigList cl;
	create_config_list(cl);

	VESNA::DeviceConfig *c = cl.get_config(0, 0);

	VESNA::ch_t ch = c->num/2;
	CHECK(c->hz_to_ch(c->ch_to_hz(ch)) == ch);
}

TEST(OffLineTestGroup, TestSweepConfig)
{
	VESNA::ConfigList cl;
	create_config_list(cl);

	VESNA::DeviceConfig *c = cl.get_config(0, 0);

	VESNA::SweepConfig(c, 0, 1, 1, 100);
}

TEST(OffLineTestGroup, TestGetSampleConfig)
{
	VESNA::ConfigList cl;
	create_config_list(cl);

	VESNA::DeviceConfig *c = cl.get_config(0, 0);

	VESNA::SweepConfig *sc = c->get_sample_config(10, 100);

	CHECK(sc->start_ch == c->hz_to_ch(10));
	CHECK(sc->stop_ch == sc->start_ch + 1);
	CHECK(sc->step_ch == 1);
	CHECK(sc->nsamples == 100);

	delete sc;
}

TEST(OffLineTestGroup, TestTimestampedData)
{
	VESNA::TimestampedData d;
	bool r = d.parse("TS 1 CH 2 DS 3 4 5 6 DE");

	CHECK(r);

	CHECK(d.timestamp == 1.0);
	CHECK(d.channel == 2);

	CHECK(d.data.size() == 4);
	CHECK(d.data[0] == 3.0);
	CHECK(d.data[1] == 4.0);
	CHECK(d.data[2] == 5.0);
	CHECK(d.data[3] == 6.0);
}

TEST(OffLineTestGroup, TestTimestampedDataLength)
{
	VESNA::TimestampedData d;
	bool r = d.parse("TS 1 CH 2 DS 3 4 5 6 DE", 4);

	CHECK(r);
}

TEST(OffLineTestGroup, TestTimestampedDataLengthError)
{
	VESNA::TimestampedData d;
	bool r = d.parse("TS 1 CH 2 DS 3 4 5 6 DE", 5);

	CHECK(!r);
}

TEST(OffLineTestGroup, TestTimestampedDataError)
{
	VESNA::TimestampedData d;
	bool r = d.parse("TS 1 C 2 DS 3 4 5 6 DE", 4);

	CHECK(!r);
}


TEST_GROUP(OnLineTestGroup)
{
};

TEST(OnLineTestGroup, TestError)
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

TEST(OnLineTestGroup, TestSpectrumSensor)
{
	VESNA::SpectrumSensor ss("/dev/ttyUSB0");
}

TEST(OnLineTestGroup, TestGetConfigList)
{
	VESNA::SpectrumSensor ss("/dev/ttyUSB0");

	VESNA::ConfigList* cl = ss.get_config_list();

	CHECK(cl != NULL);

	VESNA::DeviceConfig* c = cl->get_config(0, 0);

	CHECK(c != NULL);

	delete cl;
}

TEST(OnLineTestGroup, TestSelectSweepChannel)
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

bool test_cb(const VESNA::SweepConfig* sc, const VESNA::TimestampedData* samples, void* priv)
{
	std::vector<VESNA::data_t>::const_iterator i = samples->data.begin();
	for(; i != samples->data.end(); ++i) {
		CHECK(*i >= 0.0);
		CHECK(*i < 4096.0);
	}

	test_cb_cnt++;
	return false;
}

TEST(OnLineTestGroup, TestSampleRun)
{
	VESNA::SpectrumSensor ss("/dev/ttyUSB0");
	VESNA::ConfigList* cl = ss.get_config_list();

	VESNA::DeviceConfig* c = cl->get_config(0, 2);
	VESNA::SweepConfig* sc = c->get_sample_config(c->base, 25000);

	test_cb_cnt = 0;
	ss.sample_run(sc, test_cb, NULL);

	CHECK_EQUAL(1, test_cb_cnt);

	delete sc;
	delete cl;
}

int main(int ac, char** av)
{
	int result = 0;
	ConsoleTestOutput output;

	{
		CommandLineTestRunner runner(ac, const_cast<const char**>(av), &output);
		result = runner.runAllTestsMain();
	}

	return result;
}
