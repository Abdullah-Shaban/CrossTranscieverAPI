#include "DeviceImp.hpp"

Transceiver::ULong ReceiveChannel::createReceiveCycleProfile(
		Transceiver::Time requestedReceiveStartTime,
		Transceiver::Time requestedReceiveStopTime,
		Transceiver::ULong requestedPacketSize,
		Transceiver::UShort requestedPresetId,
		Transceiver::Frequency requestedCarrierFrequency)
{
	return 0;
}

void ReceiveChannel::configureReceiveCycle(
		Transceiver::ULong targetCycleId,
		Transceiver::Time requestedReceiveStartTime,
		Transceiver::Time requestedReceiveStopTime,
		Transceiver::ULong requestedPacketSize,
		Transceiver::Frequency requestedCarrierFrequency)
{
}

void ReceiveChannel::setReceiveStopTime(
	Transceiver::ULong targetCycleId,
	Transceiver::Time requestedReceiveStopTime)
{
};

DeviceImp::DeviceImp(Transceiver::I_ReceiveDataPush* rx_if)
{
}
