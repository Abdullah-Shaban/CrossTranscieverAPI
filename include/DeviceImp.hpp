#ifndef HAVE_DEVICEIMP_H
#define HAVE_DEVICEIMP_H

#include "transceiver.hpp"

class ReceiveChannel : public Transceiver::I_ReceiveControl
{
	public:
		Transceiver::ULong createReceiveCycleProfile(
				Transceiver::Time requestedReceiveStartTime,
				Transceiver::Time requestedReceiveStopTime,
				Transceiver::ULong requestedPacketSize,
				Transceiver::UShort requestedPresetId,
				Transceiver::Frequency requestedCarrierFrequency);
		void configureReceiveCycle(
				Transceiver::ULong targetCycleId,
				Transceiver::Time requestedReceiveStartTime,
				Transceiver::Time requestedReceiveStopTime,
				Transceiver::ULong requestedPacketSize,
				Transceiver::Frequency requestedCarrierFrequency) ;
		void setReceiveStopTime(
			Transceiver::ULong targetCycleId,
			Transceiver::Time requestedReceiveStopTime);
};

class DeviceImp
{
	public:
		DeviceImp(Transceiver::I_ReceiveDataPush* rx_if);
		ReceiveChannel receiveChannel;
};

#endif
