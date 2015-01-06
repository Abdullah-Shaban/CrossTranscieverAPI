#include "CppUTest/CommandLineTestRunner.h"

#include "DeviceImp.hpp"
#include "SpectrumSensor.hpp"

TEST_GROUP(OnLineTestGroup)
{
};

TEST_GROUP(OffLineTestGroup)
{
};

TEST(OffLineTestGroup, TestConstructor)
{
	DeviceImp di;
}

TEST(OnLineTestGroup, TestSpectrumSensor)
{
	VESNA::SpectrumSensor ss("/dev/ttyUSB0");
}

int main(int ac, char** av)
{
	return CommandLineTestRunner::RunAllTests(ac, av);
}
