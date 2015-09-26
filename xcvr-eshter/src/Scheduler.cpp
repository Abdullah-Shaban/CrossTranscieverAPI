/*
 * Copyright (C) 2015 SensorLab, Jozef Stefan Institute
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 *
 * Written by Tomaz Solc, tomaz.solc@ijs.si
 */
#include "Scheduler.hpp"

Scheduler::Scheduler()
{
	struct tm epoch_tm = {
		.tm_sec = 0,
		.tm_min = 0,
		.tm_hour = 0,
		.tm_mday = 1,
		.tm_mon = 0,
		.tm_year = 2000 - 1900
	};
	epoch = boost::chrono::system_clock::from_time_t(mktime(&epoch_tm));

	work = new boost::asio::io_service::work(io);
	thread = boost::thread(&Scheduler::loop, this);
}

Scheduler::~Scheduler()
{
	stop();
}

void Scheduler::stop()
{
	if(work != NULL) {
		delete work;
		work = NULL;
		thread.join();
	}
}

void Scheduler::schedule(const Transceiver::Time& time, scheduler_cb_t handler_cb)
{
	if(time.discriminator == Transceiver::immediateDiscriminator) {
		handler(NULL, handler_cb);
	} else if(time.discriminator == Transceiver::absoluteDiscriminator) {
		timer_t* timer = new timer_t(io);
		timer->expires_at(from_absolute_time(time));

		timer->async_wait(boost::bind(&Scheduler::handler, this, timer, handler_cb));
	} else if(time.discriminator == Transceiver::eventBasedDiscriminator) {
		boost::unique_lock<boost::mutex> lock(registry_m);

		Transceiver::EventSource es = time.eventBased.eventSourceId;

		EventRegistryEntry entry;
		entry.cb = handler_cb;
		entry.time = time;

		if(time.eventBased.eventCountOrigin == Transceiver::EventBasedTime::Next) {
			entry.target_event_cnt = registry[es].event_cnt + time.eventBased.eventCount;
		} else if(time.eventBased.eventCountOrigin == Transceiver::EventBasedTime::Previous) {
			entry.target_event_cnt = registry[es].event_cnt + time.eventBased.eventCount - 1;
		} else {
			entry.target_event_cnt = registry[es].event_cnt;
		}

		if(entry.target_event_cnt >= registry[es].event_cnt) {
			registry[es].entries.push_back(entry);
		} else if(entry.target_event_cnt == registry[es].event_cnt - 1) {
			timer_t* timer = new timer_t(io);

			sysclock_t::time_point tp = registry[es].last_time;
			tp += boost::chrono::nanoseconds(time.eventBased.timeShift);

			timer->expires_at(tp);

			timer->async_wait(boost::bind(&Scheduler::handler, this, timer, handler_cb));
		} else {
			// we don't store history for more than one event back.
			assert(0);
		}
	}
}

void Scheduler::event(Transceiver::EventSource es)
{
	boost::unique_lock<boost::mutex> lock(registry_m);

	std::list<EventRegistryEntry>::iterator i = registry[es].entries.begin();
	for(; i != registry[es].entries.end(); ++i) {
		if(registry[es].event_cnt == i->target_event_cnt) {
			if(i->time.eventBased.timeShift == 0) {
				// note: this will result in a dead-lock if handler immediately
				// triggers another event.
				handler(NULL, i->cb);
			} else {
				timer_t* timer = new timer_t(io);
				timer->expires_from_now(boost::chrono::nanoseconds(i->time.eventBased.timeShift));
				timer->async_wait(boost::bind(&Scheduler::handler, this, timer, i->cb));
			}
		}
	}

	registry[es].event_cnt++;
	registry[es].last_time = Scheduler::sysclock_t::now();
}

Transceiver::Time Scheduler::to_absolute_time(const Scheduler::sysclock_t::time_point& time)
{
	Scheduler::sysclock_t::duration d = time - epoch;

	boost::chrono::seconds seconds = boost::chrono::duration_cast<boost::chrono::seconds>(d);
	boost::chrono::nanoseconds nanoseconds = d;

	Transceiver::Time t(Transceiver::AbsoluteTime(seconds.count(), nanoseconds.count() % 1000000000ul));
	return t;
}

Scheduler::sysclock_t::time_point Scheduler::from_absolute_time(const Transceiver::Time& time)
{
	Scheduler::sysclock_t::time_point tp = epoch;
	tp += boost::chrono::seconds(time.absolute.secondCount);
	tp += boost::chrono::nanoseconds(time.absolute.nanosecondCount);

	return tp;
}

void Scheduler::handler(timer_t* timer, scheduler_cb_t handler_cb)
{
	handler_cb();

	if(timer != NULL) {
		delete timer;
	}
}

void Scheduler::loop()
{
	io.run();
}
