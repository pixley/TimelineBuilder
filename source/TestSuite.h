/*
	Copyright (c) 2022 Tyler Pixley, all rights reserved.

	This file (TestSuite.h) is part of TimelineBuilder.

	TimelineBuilder is free software: you can redistribute it and/or modify it under
	the terms of the GNU General Public License as published by the Free Software
	Foundation, either version 3 of the License, or (at your option) any later version.

	TimelineBuilder is distributed in the hope that it will be useful, but WITHOUT ANY
	WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
	PARTICULAR PURPOSE. See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along with
	TimelineBuilder. If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include <QtCore/QCommandLineParser>
#include <QtCore/QCommandLineOption>
#include <QtCore/QCoreApplication>

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