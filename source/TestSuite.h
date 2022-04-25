#pragma once

#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QCoreApplication>

class TBTestSuite
{
public:
	TBTestSuite(const QCoreApplication& app);
	bool RunTests();

private:
	QCommandLineParser Parser;

	/*
		Note for the various tests : they should return true if the test is attempted (based on whether their respective
		command line params are valid), not if the test succeeds.
	*/

	// Calendar System
	QCommandLineOption CalendarParam;
	bool CalendarSystemTest();
};