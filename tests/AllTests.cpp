#include "CppUTest/CommandLineTestRunner.h"

#include "DeviceImp.hpp"
#include "SpectrumSensor.hpp"

TEST_GROUP(TestGroup)
{
};

TEST(TestGroup, TestConstructor)
{
	DeviceImp di;
}

TEST(TestGroup, TestSpectrumSensor)
{
	VESNA::SpectrumSensor ss("/dev/ttyUSB0");
}

int main(int ac, char** av)
{
	return CommandLineTestRunner::RunAllTests(ac, av);
}
