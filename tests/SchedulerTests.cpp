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

TEST(SchedulerTestGroup, TestImmediate)
{
	Scheduler s;

	Transceiver::Time t(Transceiver::immediateDiscriminator);

	handler_cnt = 0;
	s.schedule(t, handler1);

	s.stop();
	CHECK_EQUAL(1, handler_cnt);
}

TEST(SchedulerTestGroup, TestAbsolute)
{
	boost::chrono::system_clock::time_point t = boost::chrono::system_clock::now();
}

TEST(SchedulerTestGroup, TestToAbsoluteTime)
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
	boost::chrono::system_clock::time_point tp = boost::chrono::system_clock::from_time_t(t);
	boost::chrono::nanoseconds d(20);
	tp += d;

	Scheduler s;

	Transceiver::Time tt = s.to_absolute_time(tp);

	CHECK(tt.discriminator == Transceiver::absoluteDiscriminator);
	CHECK(10 == tt.absolute.secondCount);
	CHECK(20 == tt.absolute.nanosecondCount);
}

/*
TEST(SchedulerTestGroup, TestEventBased)
{
	Scheduler s;

	Transceiver::Time t1(Transceiver::EventBasedTime(Transceiver::receiveStartTime, 0));
	Transceiver::Time t2(Transceiver::EventBasedTime(Transceiver::receiveStartTime, 1));

	handler_cnt = 0;
	s.schedule(t2, handler2);
	s.schedule(t1, handler1);

	s.stop();
	CHECK_EQUAL(2, handler_cnt);
}
*/
