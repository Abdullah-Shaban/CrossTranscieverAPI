#include "CppUTest/CommandLineTestRunner.h"

#include <stdio.h>

#include "Scheduler.hpp"

TEST_GROUP(SchedulerTestGroup)
{
};

static int handler_cnt = 0;

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

TEST(SchedulerTestGroup, TestSchedule)
{
	Scheduler s;

	s.schedule_relative(1, handler2);
	s.schedule_relative(0, handler1);

	s.stop();

	CHECK_EQUAL(2, handler_cnt);
}
