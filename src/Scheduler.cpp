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
	} else if(time.discriminator == Transceiver::absoluteDiscriminator) {
		timer_t* timer = new timer_t(io);
		timer->expires_at(from_absolute_time(time));

		timer->async_wait(boost::bind(&Scheduler::handler, this, timer, handler_cb));
	} else if(time.discriminator == Transceiver::eventBasedDiscriminator) {
		EventRegistryEntry entry;
		entry.cb = handler_cb;
		entry.time = time;

		registry[time.eventBased.eventSourceId].entries.push_back(entry);
	}
	/*
	boost::asio::deadline_timer* timer = new boost::asio::deadline_timer(io);
	timer->expires_from_now(boost::posix_time::seconds(seconds));
	timers.push_back(timer);

	timer->async_wait(boost::bind(&Scheduler::handler, this, timer, handler_cb));
	*/
}

void Scheduler::event(Transceiver::EventSource es)
{
	std::list<EventRegistryEntry>::iterator i = registry[es].entries.begin();
	for(; i != registry[es].entries.end(); ++i) {
		handler(NULL, i->cb);
	}
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
		//timers.remove(timer);
		delete timer;
	}
}

void Scheduler::loop()
{
	io.run();
}
