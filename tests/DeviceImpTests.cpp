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

TEST(DeviceImpTestGroup, TestCreateRXProfile)
{
	TestReceiver rx;
	DeviceImp di(&rx);

	Transceiver::ReceiveCycleProfile profile;
	profile.ReceiveStartTime.discriminator = Transceiver::immediateDiscriminator;
	profile.ReceiveStopTime.discriminator = Transceiver::undefinedDiscriminator;
	profile.PacketSize = 1024;
	profile.TuningPreset = 0;
	profile.CarrierFrequency = 700e6;

	Transceiver::ULong i = di.receiveChannel.createReceiveCycleProfile(
		profile.ReceiveStartTime,
		profile.ReceiveStopTime,
		profile.PacketSize,
		profile.TuningPreset,
		profile.CarrierFrequency);

	//usleep (1000000);

	Transceiver::Time stop(Transceiver::immediateDiscriminator);

	di.receiveChannel.setReceiveStopTime(i, stop);
}
