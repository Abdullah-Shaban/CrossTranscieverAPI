#include "CppUTest/CommandLineTestRunner.h"

#include <stdio.h>

#include "Scheduler.hpp"

TEST_GROUP(SchedulerTestGroup)
{
};

static int handler_cnt;

void handler1()
{
	CHECK_EQUAL(0, handler_cnt);
	handler_cnt++;
}

void handler2()
{
	CHECK_EQUAL(1, handler_cnt);
	handler_cnt++;
}

TEST(SchedulerTestGroup, TestImmediateDiscriminator)
{
	Scheduler s;

	Transceiver::Time t(Transceiver::immediateDiscriminator);

	handler_cnt = 0;
	s.schedule(t, handler1);

	s.stop();
	CHECK_EQUAL(1, handler_cnt);
}

TEST(SchedulerTestGroup, TestAbsoluteDiscriminator)
{
	Scheduler s;

	Scheduler::sysclock_t::time_point tp = boost::chrono::system_clock::now();
	tp += boost::chrono::seconds(1);

	Transceiver::Time t = s.to_absolute_time(tp);

	handler_cnt = 0;
	s.schedule(t, handler1);

	s.stop();
	CHECK_EQUAL(1, handler_cnt);
}

TEST(SchedulerTestGroup, TestToFromAbsoluteTime)
{
	struct tm tm1 = {
		.tm_sec = 10, // 10 seconds past epoch
		.tm_min = 0,
		.tm_hour = 0,
		.tm_mday = 1,
		.tm_mon = 0,
		.tm_year = 2000 - 1900
	};

	time_t t = mktime(&tm1);
	Scheduler::sysclock_t::time_point tp = boost::chrono::system_clock::from_time_t(t);
	boost::chrono::nanoseconds d(20);
	tp += d;

	Scheduler s;

	Transceiver::Time tt = s.to_absolute_time(tp);

	CHECK(tt.discriminator == Transceiver::absoluteDiscriminator);
	CHECK(10 == tt.absolute.secondCount);
	CHECK(20 == tt.absolute.nanosecondCount);

	Scheduler::sysclock_t::time_point tp2 = s.from_absolute_time(tt);

	CHECK(tp2 == tp);
}

TEST(SchedulerTestGroup, TestEventBased)
{
	Scheduler s;

	Transceiver::EventBasedTime ev(Transceiver::receiveStartTime, 0);
	ev.eventCountOrigin = Transceiver::EventBasedTime::Next;
	ev.eventCount = 0;

	Transceiver::Time t(ev);

	handler_cnt = 0;
	s.schedule(t, handler1);

	s.event(Transceiver::receiveStartTime);

	s.stop();
	CHECK_EQUAL(1, handler_cnt);
}
/*
TEST(SchedulerTestGroup, TestEventBasedEventCount)
{
	Scheduler s;

	Transceiver::EventBasedTime ev(Transceiver::receiveStartTime, 0);
	ev.eventCountOrigin = Transceiver::EventBasedTime::Next;
	ev.eventCount = 1;

	Transceiver::Time t(ev);

	handler_cnt = 0;
	s.schedule(t, handler1);

	s.event(Transceiver::receiveStartTime);
	CHECK_EQUAL(0, handler_cnt);
	s.event(Transceiver::receiveStartTime);

	s.stop();
	CHECK_EQUAL(1, handler_cnt);
}
*/
