#include "CppUTest/CommandLineTestRunner.h"

#include "SpectrumSensor.hpp"

TEST_GROUP(SpectrumSensorTestGroup)
{
};

void create_config_list(VESNA::ConfigList& cl)
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

TEST(SpectrumSensorTestGroup, TestConfigList)
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

TEST(SpectrumSensorTestGroup, TestConfigListGetNum)
{
	VESNA::ConfigList cl;

	create_config_list(cl);

	CHECK_EQUAL(cl.get_config_num(), 1);
	CHECK_EQUAL(cl.get_device_num(), 1);
}

TEST(SpectrumSensorTestGroup, TestChToHz)
{
	VESNA::ConfigList cl;
	create_config_list(cl);

	VESNA::DeviceConfig *c = cl.get_config(0, 0);

	CHECK(c->ch_to_hz(0) == c->base);
}

TEST(SpectrumSensorTestGroup, TestHzToCh)
{
	VESNA::ConfigList cl;
	create_config_list(cl);

	VESNA::DeviceConfig *c = cl.get_config(0, 0);

	CHECK(c->hz_to_ch(c->base) == 0);
}

TEST(SpectrumSensorTestGroup, TestChToHzToCh)
{
	VESNA::ConfigList cl;
	create_config_list(cl);

	VESNA::DeviceConfig *c = cl.get_config(0, 0);

	VESNA::ch_t ch = c->num/2;
	CHECK(c->hz_to_ch(c->ch_to_hz(ch)) == ch);
}

TEST(SpectrumSensorTestGroup, TestSweepConfig)
{
	VESNA::ConfigList cl;
	create_config_list(cl);

	VESNA::DeviceConfig *c = cl.get_config(0, 0);

	VESNA::SweepConfig(c, 0, 1, 1, 100);
}

TEST(SpectrumSensorTestGroup, TestGetSampleConfig)
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

TEST(SpectrumSensorTestGroup, TestTimestampedData)
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

TEST(SpectrumSensorTestGroup, TestTimestampedDataLength)
{
	VESNA::TimestampedData d;
	bool r = d.parse("TS 1 CH 2 DS 3 4 5 6 DE", 4);

	CHECK(r);
}

TEST(SpectrumSensorTestGroup, TestTimestampedDataLengthError)
{
	VESNA::TimestampedData d;
	bool r = d.parse("TS 1 CH 2 DS 3 4 5 6 DE", 5);

	CHECK(!r);
}

TEST(SpectrumSensorTestGroup, TestTimestampedDataError)
{
	VESNA::TimestampedData d;
	bool r = d.parse("TS 1 C 2 DS 3 4 5 6 DE", 4);

	CHECK(!r);
}
