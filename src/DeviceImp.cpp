#include "DeviceImp.hpp"

#include <stdio.h>

ReceiveChannel::ReceiveChannel(Transceiver::I_ReceiveDataPush* rx, VESNA::I_SpectrumSensor* ss)
	: sensor(ss), receiver(rx), cycle_buffer_cnt(0),
	device_control_thread(&ReceiveChannel::device_control, this), want_stop(false)
{
}

ReceiveChannel::~ReceiveChannel()
{
	boost::unique_lock<boost::mutex> lock(cycle_buffer_m);
	want_stop = true;
	lock.unlock();

	cycle_buffer_cv_add.notify_all();

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

	lock.unlock();

	cycle_buffer_cv_add.notify_all();

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
}

void ReceiveChannel::wait()
{
	boost::unique_lock<boost::mutex> lock(cycle_buffer_m);
	while(!cycle_buffer.empty()) {
		cycle_buffer_cv_del.wait(lock);
	}

	return;
}

void ReceiveChannel::device_control()
{
	while(1) {
		Transceiver::ReceiveCycleProfile *to_start = get_cycle();
		if(to_start == NULL) {
			break;
		}

		run_cycle(to_start);

		{
			boost::unique_lock<boost::mutex> lock(cycle_buffer_m);
			cycle_buffer.remove(to_start);
			cycle_buffer_cv_del.notify_all();
		}
	}
}

Transceiver::ReceiveCycleProfile* ReceiveChannel::get_cycle()
{
	boost::unique_lock<boost::mutex> lock(cycle_buffer_m);
	while(!want_stop && cycle_buffer.empty()) {
		cycle_buffer_cv_add.wait(lock);
	}

	if(want_stop) {
		return NULL;
	} else {
		return *cycle_buffer.begin();
	}
}

bool ReceiveChannel::pushSamples(const VESNA::SweepConfig* sc, const VESNA::TimestampedData* samples,
		Transceiver::ReceiveCycleProfile* cycle)
{
	std::vector<Transceiver::BBSample> bbsamples;
	std::vector<VESNA::data_t>::const_iterator i = samples->data.begin();
	for(; i != samples->data.end(); ++i) {
		Transceiver::BBSample bb(*i, 0.);
		bbsamples.push_back(bb);
	}

	Transceiver::BBPacket packet(sc->nsamples, &bbsamples.front());
	//FIXME: Is it ok to always set endOfBurst to true here?
	receiver->pushBBSamplesRx(&packet, true);

	{
		boost::unique_lock<boost::mutex> lock(cycle_buffer_m);
		// FIXME: check other end conditions here
		Transceiver::Time stopTime = cycle->ReceiveStopTime;

		if(stopTime.discriminator == Transceiver::immediateDiscriminator) {
			return false;
		} else if(stopTime.discriminator == Transceiver::eventBasedDiscriminator &&
				stopTime.eventBased.eventSourceId == Transceiver::receiveStartTime &&
				samples->timestamp >= stopTime.eventBased.timeShift) {
			return false;
		} else {
			return true;
		}
	}
}

void ReceiveChannel::run_cycle(Transceiver::ReceiveCycleProfile* cycle)
{
	VESNA::ConfigList* cl = sensor->get_config_list();

	VESNA::DeviceConfig* c = cl->get_config(0, cycle->TuningPreset);
	VESNA::SweepConfig* sc = c->get_sample_config(cycle->CarrierFrequency, cycle->PacketSize);

	sensor->sample_run(sc, boost::bind(&ReceiveChannel::pushSamples, this, _1, _2, cycle));

	delete cl;
}

DeviceImp::DeviceImp(Transceiver::I_ReceiveDataPush* rx, VESNA::I_SpectrumSensor* ss)
		: receiveChannel(rx, ss)
{
}
