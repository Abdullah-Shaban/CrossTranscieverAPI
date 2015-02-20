#include "DeviceImp.hpp"

#include <stdio.h>

ReceiveChannel::ReceiveChannel(Transceiver::I_ReceiveDataPush* rx)
	: cycle_buffer_cnt(0), device_control_thread(&ReceiveChannel::device_control, this), want_stop(false)
{
}

ReceiveChannel::~ReceiveChannel()
{
	want_stop = true;
	cycle_buffer_cv.notify_all();

	device_control_thread.join();
}

Transceiver::ULong ReceiveChannel::createReceiveCycleProfile(
		Transceiver::Time requestedReceiveStartTime,
		Transceiver::Time requestedReceiveStopTime,
		Transceiver::ULong requestedPacketSize,
		Transceiver::UShort requestedPresetId,
		Transceiver::Frequency requestedCarrierFrequency)
{
	boost::unique_lock<boost::mutex> lock(cycle_buffer_m);

	Transceiver::ReceiveCycleProfile* p = new Transceiver::ReceiveCycleProfile(
			cycle_buffer_cnt,
			requestedPresetId,
			requestedCarrierFrequency,
			requestedPacketSize,
			requestedReceiveStartTime,
			requestedReceiveStopTime);

	cycle_buffer_cnt++;

	//FIXME: should probably have some order to the list depending on the
	//StartTime or something...
	cycle_buffer.push_back(p);

	cycle_buffer_cv.notify_one();

	return p->ReceiveCycle;
}

void ReceiveChannel::configureReceiveCycle(
		Transceiver::ULong targetCycleId,
		Transceiver::Time requestedReceiveStartTime,
		Transceiver::Time requestedReceiveStopTime,
		Transceiver::ULong requestedPacketSize,
		Transceiver::Frequency requestedCarrierFrequency)
{
	//FIXME: provide implementation here.
}

void ReceiveChannel::setReceiveStopTime(
	Transceiver::ULong targetCycleId,
	Transceiver::Time requestedReceiveStopTime)
{
	boost::unique_lock<boost::mutex> lock(cycle_buffer_m);

	std::list<Transceiver::ReceiveCycleProfile*>::iterator i = cycle_buffer.begin();
	for(; i != cycle_buffer.end(); ++i) {
		if((*i)->ReceiveCycle == targetCycleId) {
			(*i)->ReceiveStopTime = requestedReceiveStopTime;
			return;
		}
	}

	//FIXME: should raise an error here if cycle ID was not found.
};

void ReceiveChannel::device_control()
{
	while(!want_stop) {
		Transceiver::ReceiveCycleProfile *to_start = get_cycle();

		run_cycle(to_start);

		{
			boost::unique_lock<boost::mutex> lock(cycle_buffer_m);
			cycle_buffer.remove(to_start);
		}
	}
}

Transceiver::ReceiveCycleProfile* ReceiveChannel::get_cycle()
{
	boost::unique_lock<boost::mutex> lock(cycle_buffer_m);
	while(cycle_buffer.empty()) {
		cycle_buffer_cv.wait(lock);
	}

	return *cycle_buffer.begin();
}

void ReceiveChannel::run_cycle(Transceiver::ReceiveCycleProfile* cycle)
{
}

DeviceImp::DeviceImp(Transceiver::I_ReceiveDataPush* rx)
		: receiveChannel(rx)
{
}
