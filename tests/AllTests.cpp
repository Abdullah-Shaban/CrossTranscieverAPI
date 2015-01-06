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

TEST(OffLineTestGroup, TestConfigList)
{
	VESNA::ConfigList cl;

	std::vector<std::string> lines;
	lines.push_back("device 0: Test device\n");
	lines.push_back("  channel config 0,0: Test config\n");
	lines.push_back("    base: 0 Hz\n");
	lines.push_back("    spacing: 1 Hz\n");
	lines.push_back("    bw: 1 Hz\n");
	lines.push_back("    num: 1000000000\n");
	lines.push_back("    time: 5 ms\n");

	cl.parse(lines);

	VESNA::DeviceConfig *c = cl.get_config(0, 0);

	CHECK(c != NULL);

	CHECK_EQUAL(0, c->id);
	STRCMP_EQUAL("Test config", c->name.c_str());

	CHECK_EQUAL(c->device->id, 0);
	STRCMP_EQUAL(c->device->name.c_str(), "Test device");
	CHECK_EQUAL(c->device->supports_sampling, false);
}

TEST_GROUP(OnLineTestGroup)
{
};

TEST(OnLineTestGroup, TestSpectrumSensor)
{
	VESNA::SpectrumSensor ss("/dev/ttyUSB0");
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
