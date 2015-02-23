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
