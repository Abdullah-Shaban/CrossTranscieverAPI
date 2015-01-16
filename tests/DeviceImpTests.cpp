#include "CppUTest/CommandLineTestRunner.h"

#include "DeviceImp.hpp"

class TestReceiver : public Transceiver::I_ReceiveDataPush
{
	public:
		void pushBBSamplesRx(Transceiver::BBPacket* thePushedPacket,
				Transceiver::Boolean endOfBurst) {};
		TestReceiver() {};
		~TestReceiver() {};
};

TEST_GROUP(DeviceImpTestGroup)
{
};

TEST(DeviceImpTestGroup, TestConstructor)
{
	TestReceiver rx;
	DeviceImp di(&rx);
}
