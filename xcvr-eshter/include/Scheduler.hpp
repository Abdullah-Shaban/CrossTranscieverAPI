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
#ifndef HAVE_SCHEDULER_H
#define HAVE_SCHEDULER_H

// workaround for boost/thread.hpp bug
#include <time.h>
#undef TIME_UTC

#include <list>
#include <map>

#include <boost/chrono.hpp>
#include <boost/thread.hpp>
#include <boost/asio.hpp>

#include "transceiver.hpp"

typedef boost::function<void()> scheduler_cb_t;

class EventRegistryEntry
{
	public:
		Transceiver::Time time;
		scheduler_cb_t cb;
		Transceiver::ULong target_event_cnt;
};

class EventRegistry
{
	public:
		std::list<EventRegistryEntry> entries;

		Transceiver::ULong event_cnt;
		boost::chrono::system_clock::time_point last_time;
};

class Scheduler
{
	public:
		typedef boost::chrono::system_clock sysclock_t;
		typedef boost::asio::basic_waitable_timer<sysclock_t> timer_t;

		Scheduler();
		~Scheduler();

		void schedule(const Transceiver::Time& time, scheduler_cb_t handler_cb);
		void event(Transceiver::EventSource es);
		void stop();

		Transceiver::Time to_absolute_time(const sysclock_t::time_point& time);
		sysclock_t::time_point from_absolute_time(const Transceiver::Time& time);

	private:
		boost::asio::io_service io;
		boost::asio::io_service::work *work;
		boost::thread thread;

		void loop();
		void handler(timer_t* timer, scheduler_cb_t handler_cb);

		boost::chrono::system_clock::time_point epoch;

		std::map<Transceiver::EventSource, EventRegistry> registry;
		boost::mutex registry_m;
};

#endif
