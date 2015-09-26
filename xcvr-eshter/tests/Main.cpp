#include "CppUTest/CommandLineTestRunner.h"

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
