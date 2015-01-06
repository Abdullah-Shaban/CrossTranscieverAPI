#include "CppUTest/CommandLineTestRunner.h"

#include "DeviceImp.hpp"

TEST_GROUP(TestGroup)
{
};

TEST(TestGroup, TestConstructor)
{
	DeviceImp di;
}

int main(int ac, char** av)
{
	return CommandLineTestRunner::RunAllTests(ac, av);
}
