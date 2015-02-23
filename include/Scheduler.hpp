#ifndef HAVE_SCHEDULER_H
#define HAVE_SCHEDULER_H

// workaround for boost/thread.hpp bug
#include <time.h>
#undef TIME_UTC

#include <list>

#include <boost/thread.hpp>
#include <boost/asio.hpp>

typedef void (*scheduler_cb_t)();

class Scheduler
{
	public:
		Scheduler();
		~Scheduler();

		void schedule_relative(float seconds, scheduler_cb_t handler);
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
