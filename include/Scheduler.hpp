#ifndef HAVE_SCHEDULER_H
#define HAVE_SCHEDULER_H

// workaround for boost/thread.hpp bug
#include <time.h>
#undef TIME_UTC

#include <list>

#include <boost/chrono.hpp>
#include <boost/thread.hpp>
#include <boost/asio.hpp>

#include "transceiver.hpp"

typedef void (*scheduler_cb_t)();

class Scheduler
{
	public:
		Scheduler();
		~Scheduler();

		void schedule(const Transceiver::Time& time, scheduler_cb_t handler_cb);
		void stop();

		Transceiver::Time to_absolute_time(const boost::chrono::system_clock::time_point& time);

	private:
		boost::asio::io_service io;
		boost::asio::io_service::work *work;
		boost::thread thread;

		std::list<boost::asio::deadline_timer*> timers;

		void loop();
		void handler(boost::asio::deadline_timer* timer, scheduler_cb_t handler_cb);

		boost::chrono::system_clock::time_point epoch;
};

#endif
