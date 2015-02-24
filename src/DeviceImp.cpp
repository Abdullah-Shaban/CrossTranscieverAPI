#include "DeviceImp.hpp"

#include <stdio.h>

ReceiveChannel::ReceiveChannel(Transceiver::I_ReceiveDataPush* rx, VESNA::I_SpectrumSensor* ss)
	: dc(rx, ss), cycle_buffer_cnt(0)
{
}

ReceiveChannel::~ReceiveChannel()
{
	scheduler.stop();
}

void ReceiveChannel::start_cb(ReceiveCycleProfileEntry* e)
{
	dc.start(e->sc);
	scheduler.event(Transceiver::receiveStartTime);
}

void ReceiveChannel::stop_cb(ReceiveCycleProfileEntry* e)
{
	dc.stop();

	{
		boost::unique_lock<boost::mutex> lock(cycle_buffer_m);

		delete e->sc;
		delete e->cycle;

		cycle_buffer.remove(e);

		delete e;
	}

	scheduler.event(Transceiver::receiveStopTime);
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

	ReceiveCycleProfileEntry* e = new ReceiveCycleProfileEntry;
	e->cycle = p;

	VESNA::ConfigList* cl = dc.get_config_list();
	VESNA::DeviceConfig* c = cl->get_config(0, p->TuningPreset);
	e->sc = c->get_sample_config(p->CarrierFrequency, p->PacketSize);

	cycle_buffer.push_back(e);

	scheduler.schedule(p->ReceiveStartTime,
			boost::bind(&ReceiveChannel::start_cb, this, e));
	scheduler.schedule(p->ReceiveStopTime,
			boost::bind(&ReceiveChannel::stop_cb, this, e));

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

	std::list<ReceiveCycleProfileEntry*>::iterator i = cycle_buffer.begin();
	for(; i != cycle_buffer.end(); ++i) {
		if((*i)->cycle->ReceiveCycle == targetCycleId) {
			//FIXME: check if previous receiveStopTime was undefined,
			// because otherwise we don't support this.
			(*i)->cycle->ReceiveStopTime = requestedReceiveStopTime;

			lock.unlock();

			scheduler.schedule(requestedReceiveStopTime,
				boost::bind(&ReceiveChannel::stop_cb, this, (*i)));

			return;
		}
	}

	//FIXME: should raise an error here if cycle ID was not found.
}

void ReceiveChannel::wait()
{
	scheduler.stop();
}

DeviceImp::DeviceImp(Transceiver::I_ReceiveDataPush* rx, VESNA::I_SpectrumSensor* ss)
		: receiveChannel(rx, ss)
{
}
