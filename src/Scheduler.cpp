#include "Scheduler.hpp"

Scheduler::Scheduler()
{
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
