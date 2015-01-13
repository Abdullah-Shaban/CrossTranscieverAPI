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
