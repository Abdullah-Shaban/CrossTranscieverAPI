#ifndef HAVE_SCHEDULER_H
#define HAVE_SCHEDULER_H

// workaround for boost/thread.hpp bug
#include <time.h>
#undef TIME_UTC

#include <list>

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

	private:
		boost::asio::io_service io;
		boost::asio::io_service::work *work;
		boost::thread thread;

		std::list<boost::asio::deadline_timer*> timers;

		void loop();
		void handler(boost::asio::deadline_timer* timer, scheduler_cb_t handler_cb);
};

#endif
