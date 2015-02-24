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

typedef void (*scheduler_cb_t)();

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
