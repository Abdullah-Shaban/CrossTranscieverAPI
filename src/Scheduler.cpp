#include "Scheduler.hpp"

Scheduler::Scheduler()
{
	struct tm tm1 = {
		.tm_sec = 0,
		.tm_min = 0,
		.tm_hour = 0,
		.tm_mday = 1,
		.tm_mon = 0,
		.tm_year = 2000 - 1900
	};
	epoch = boost::chrono::system_clock::from_time_t(mktime(&tm1));

	work = new boost::asio::io_service::work(io);
	thread = boost::thread(&Scheduler::loop, this);
}

Scheduler::~Scheduler()
{
	if(work != NULL) {
		stop();
	}
}

void Scheduler::stop()
{
	delete work;
	work = NULL;
	thread.join();
}

void Scheduler::schedule(const Transceiver::Time& time, scheduler_cb_t handler_cb)
{
	if(time.discriminator == Transceiver::immediateDiscriminator) {
		handler(NULL, handler_cb);
	}
	/*
	boost::asio::deadline_timer* timer = new boost::asio::deadline_timer(io);
	timer->expires_from_now(boost::posix_time::seconds(seconds));
	timers.push_back(timer);

	timer->async_wait(boost::bind(&Scheduler::handler, this, timer, handler_cb));
	*/
}

Transceiver::Time Scheduler::to_absolute_time(const boost::chrono::system_clock::time_point& time)
{
	boost::chrono::system_clock::duration d = time - epoch;

	boost::chrono::seconds seconds = boost::chrono::duration_cast<boost::chrono::seconds>(d);
	boost::chrono::nanoseconds nanoseconds = d;

	Transceiver::Time t(Transceiver::AbsoluteTime(seconds.count(), nanoseconds.count() % 1000000000ul));
	return t;
}

void Scheduler::handler(boost::asio::deadline_timer* timer, scheduler_cb_t handler_cb)
{
	handler_cb();

	if(timer != NULL) {
		timers.remove(timer);
		delete timer;
	}
}

void Scheduler::loop()
{
	io.run();
}
