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
		ReceiveChannel(Transceiver::I_ReceiveDataPush* rx);
		~ReceiveChannel();

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

		void wait();

	private:
		Transceiver::ULong cycle_buffer_cnt;
		std::list<Transceiver::ReceiveCycleProfile*> cycle_buffer;
		boost::mutex cycle_buffer_m;
		boost::condition_variable cycle_buffer_cv_add;
		boost::condition_variable cycle_buffer_cv_del;

		boost::thread device_control_thread;

		bool want_stop;

		void device_control();
		Transceiver::ReceiveCycleProfile* get_cycle();
		void run_cycle(Transceiver::ReceiveCycleProfile* cycle);
};

class DeviceImp
{
	public:
		DeviceImp(Transceiver::I_ReceiveDataPush* rx);
		ReceiveChannel receiveChannel;
};

#endif
