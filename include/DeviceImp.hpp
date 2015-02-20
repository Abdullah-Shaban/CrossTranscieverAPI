#ifndef HAVE_DEVICEIMP_H
#define HAVE_DEVICEIMP_H

// workaround for boost bug
#include <time.h>
#undef TIME_UTC

#include "transceiver.hpp"

#include <boost/thread.hpp>
#include <list>

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

	private:
		Transceiver::ULong cycle_buffer_cnt;
		std::list<Transceiver::ReceiveCycleProfile*> cycle_buffer;
		boost::mutex cycle_buffer_m;
		boost::condition_variable cycle_buffer_cv;
};

class DeviceImp
{
	public:
		DeviceImp(Transceiver::I_ReceiveDataPush* rx_if);
		ReceiveChannel receiveChannel;
};

#endif
