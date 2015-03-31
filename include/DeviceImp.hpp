#ifndef HAVE_DEVICEIMP_H
#define HAVE_DEVICEIMP_H

// workaround for boost/thread.hpp bug
#include <time.h>
#undef TIME_UTC

#include "DeviceController.hpp"
#include "Scheduler.hpp"
#include "SpectrumSensor.hpp"
#include "transceiver.hpp"

#include <boost/thread.hpp>
#include <list>

class ReceiveCycleProfileEntry
{
	public:
		Transceiver::ReceiveCycleProfile* cycle;
		boost::shared_ptr<VESNA::SweepConfig> sc;
};

class ReceiveChannel : public Transceiver::I_ReceiveControl
{
	public:
		ReceiveChannel(Transceiver::I_ReceiveDataPush* rx, VESNA::I_SpectrumSensor* ss);
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
		DeviceController dc;
		Scheduler scheduler;

		Transceiver::ULong cycle_buffer_cnt;
		std::list<ReceiveCycleProfileEntry*> cycle_buffer;
		boost::mutex cycle_buffer_m;

		void start_cb(ReceiveCycleProfileEntry* e);
		void stop_cb(ReceiveCycleProfileEntry* e);
};

class DeviceImp
{
	public:
		DeviceImp(Transceiver::I_ReceiveDataPush* rx, VESNA::I_SpectrumSensor* ss);
		ReceiveChannel receiveChannel;
};

#endif
