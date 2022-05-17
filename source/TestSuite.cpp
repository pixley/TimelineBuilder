/*
	Copyright (c) 2022 Tyler Pixley, all rights reserved.

	This file (TestSuite.cpp) is part of TimelineBuilder.

	TimelineBuilder is free software: you can redistribute it and/or modify it under
	the terms of the GNU General Public License as published by the Free Software
	Foundation, either version 3 of the License, or (at your option) any later version.

	TimelineBuilder is distributed in the hope that it will be useful, but WITHOUT ANY
	WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
	PARTICULAR PURPOSE. See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along with
	TimelineBuilder. If not, see <https://www.gnu.org/licenses/>.
*/

// Has to come first in order to not conflict with Qt stuff...
#include "PyBind.h"

#include "TestSuite.h"

#include "JsonFiles.h"
#include "Calendar.h"
#include "Logging.h"

#include <QJsonDocument>

TBTestSuite::TBTestSuite(const QCoreApplication& app) :
	Parser(),
	CalendarParam("calendar-test", "Tests the given calendar system without running the full app.", "system")
{
	Parser.addOption(CalendarParam);

	// Must run after adding all options.
	Parser.process(app);
}

bool TBTestSuite::RunTests()
{
	bool anyTestRan = false;

	anyTestRan |= CalendarSystemTest();

	return anyTestRan;
}

bool TBTestSuite::CalendarSystemTest()
{
	// Only try to run the test if a value has been specified
	QString testSystem = Parser.value(CalendarParam);
	if (testSystem.isEmpty())
	{
		return false;
	}

	/*
		Pre-test initialization.
	*/

	TBLog::Log("Beginning calendar system test: %0", testSystem);

	QString jsonPath = QString("scripts/%0.json").arg(testSystem);
	TBJsonFile jsonFile(jsonPath, QIODevice::ReadOnly);
	QJsonDocument calendarData;
	EJsonFileResult openResult = jsonFile.GetJsonDocument(calendarData);

	if (openResult == EJsonFileResult::Success)
	{
		TBLog::Log("Calendar system data successfully loaded.");
	}
	else
	{
		switch (openResult)
		{
		case EJsonFileResult::FileNotFound:
			TBLog::Error("Could not open calendar system file (%0).  Test aborted.", jsonPath);
			break;
		case EJsonFileResult::FileNotJson:
			TBLog::Error("Calendar system file (%0) did not contain valid JSON.  Test aborted.", jsonPath);
			break;
		default:
			TBLog::Error("Unknown error opening the calendar system file (%s).  Test aborted.", jsonPath);
			break;
		}

		return true;
	}

	TBCalendarSystem calendarSystem;
	calendarSystem.LoadFromJson(calendarData.object());
	if (!calendarSystem.IsValid())
	{
		TBLog::Error("Error populating calendar system from JSON data.  Test aborted.");
		return true;
	}

	TBLog::Log("Calendar System Info:");
	TBLog::Log(QString("%0 --- %1").arg(calendarSystem.GetName(), calendarSystem.GetDescription()));

	if (!calendarSystem.InitializeScript())
	{
		TBLog::Error("Error initializing calendar script.  See Python exception above.  Test aborted.");
		return true;
	}
	
	TBLog::Log("Calendar system script initialized.  Beginning test suite.");

	/*
		Test proper begins here.
	*/
	try
	{
		uint32 testIndex = 1;
		// Basic data checks.
		{
			try
			{
				TBLog::Log("Test %0: Checking length of date format: %1", testIndex++, calendarSystem.GetBrokenDateLength());
			}
			catch (py::error_already_set& pythonException)
			{
				TBLog::Warning("Exception thrown in Test %0!  Test failed!", testIndex);
			}
		}
		// Date format tests.
		{
			QList<int64> testDays({ 0, 64, 1024, 1 << 20, -1, -64, -1024 });
			for (int64 day : testDays)
			{
				try
				{
					QString formattedDay = calendarSystem.FormatDate(day);
					TBLog::Log("Test %0: Formatting day %1: %2", testIndex++, day, formattedDay);
				}
				catch (py::error_already_set& pythonException)
				{
					TBLog::Warning("Exception thrown in Test %0!  Test failed!", testIndex++);
				}
			}
		}
		// Broken date format tests
		{
			// Test dates of the form "i/i/i" and "i/i/-i" for i [0,20]
			for (int32 testNum = 0; testNum <= 20; testNum++)
			{
				TBBrokenDate testDate(calendarSystem.GetBrokenDateLength(), testNum);
				QString formattedDate = "";

				if (calendarSystem.ValidateBrokenDate(testDate)) {
					try
					{
						formattedDate = calendarSystem.FormatDate(testDate);
						TBLog::Log("Test %0: Formatting date of all '%1': %2", testIndex++, testNum, formattedDate);
					}
					catch (py::error_already_set& pythonException)
					{
						TBLog::Warning("Exception thrown in Test %0!  Test failed!", testIndex++);
					}
				}
				else
				{
					TBLog::Log("Test %0: Date of all '%1' is invalid and could not be formatted.", testIndex++, testNum);
				}

				// Now test the corresponding date with negative year (or equivalent)
				if (testNum == 0)
				{
					continue;
				}
				testDate.last() *= -1;
				if (calendarSystem.ValidateBrokenDate(testDate)) {
					try
					{
						formattedDate = calendarSystem.FormatDate(testDate);
						TBLog::Log("Test %0: Formatting date of all '%1' with negative year (or equivalent): %2", testIndex++, testNum, formattedDate);
					}
					catch (py::error_already_set& pythonException)
					{
						TBLog::Warning("Exception thrown in Test %0!  Test failed!", testIndex++);
					}
				}
				else
				{
					TBLog::Log("Test %0: Date of all '%1' with negative year (or equivalent) is invalid and could not be formatted.", testIndex++, testNum);
				}
			}
		}
		// Date span format tests
		{
			QList<int64> testDays({ 0, 64, 1024, 1 << 20, -1, -64, -1024 });
			for (int32 indexOne = 0; indexOne < testDays.length(); indexOne++)
			{
				for (int32 indexTwo = 0; indexTwo < testDays.length(); indexTwo++)
				{
					if (indexOne == indexTwo)
					{
						// We aren't bothering trying to compare a date against itself.
						continue;
					}

					try
					{
						QString formattedSpan = calendarSystem.FormatDateSpan(testDays[indexOne], testDays[indexTwo]);
						TBLog::Log("Test %0: Formatting span between day %1 and day %2 : %3", testIndex++, testDays[indexOne], testDays[indexTwo], formattedSpan);
					}
					catch (py::error_already_set& pythonException)
					{
						TBLog::Warning("Exception thrown in Test %0!  Test failed!", testIndex++);
					}
				}
			}
		}
		// Date combination tests
		{
			// Test dates of the form "i/i/i" for i [0,20]
			for (int32 testNum = 0; testNum <= 20; testNum++)
			{
				TBBrokenDate testDate(calendarSystem.GetBrokenDateLength(), testNum);
				try
				{
					if (calendarSystem.ValidateBrokenDate(testDate))
					{
						TBDate combinedDate = calendarSystem.CombineDate(testDate);
						TBLog::Log("Test %0: Date of all '%1' is day %2", testIndex++, testNum, combinedDate.GetDays());
					}
					else
					{
						TBLog::Log("Test %0: Date of all '%1' was invalid and could not be combined.", testIndex++, testNum);
					}
				}
				catch (py::error_already_set& pythonException)
				{
					TBLog::Warning("Exception thrown in Test %0!  Test failed!", testIndex++);
				}

				// Now test the corresponding date with negative year (or equivalent)
				if (testNum == 0)
				{
					continue;
				}
				try
				{
					testDate.last() *= -1;
					if (calendarSystem.ValidateBrokenDate(testDate))
					{
						TBDate combinedDate = calendarSystem.CombineDate(testDate);
						TBLog::Log("Test %0: Date of all '%1' with negative year is day %2", testIndex++, testNum, combinedDate.GetDays());
					}
					else
					{
						TBLog::Log("Test %0: Date of all '%1' with negative year was invalid and could not be combined.", testIndex++, testNum);
					}
				}
				catch (py::error_already_set& pythonException)
				{
					TBLog::Warning("Exception thrown in Test %0!  Test failed!", testIndex++);
				}
			}
		}
		// Date movement tests
		{
			TBDate baseDate(1 << 17);
			for (int32 testNum = 0; testNum <= 20; testNum++)
			{
				TBDate newDate(0);
				try
				{
					TBBrokenTimespan testSpan(calendarSystem.GetBrokenDateLength(), testNum);
					newDate = calendarSystem.MoveDate(baseDate, testSpan);
					TBLog::Log("Test %0: Offsetting day %1 by span of all '%2': %3", testIndex++, baseDate.GetDays(), testNum, newDate.GetDays());
				}
				catch (py::error_already_set& pythonException)
				{
					TBLog::Warning("Exception thrown in Test %0!  Test failed!", testIndex++);
				}

				// Now test the corresponding negative offset
				try
				{
					TBBrokenTimespan negSpan(calendarSystem.GetBrokenDateLength(), -testNum);
					newDate = calendarSystem.MoveDate(baseDate, negSpan);
					TBLog::Log("Test %0: Offsetting day %1 by span of all '-%2': %3", testIndex++, baseDate.GetDays(), testNum, newDate.GetDays());
				}
				catch (py::error_already_set& pythonException)
				{
					TBLog::Warning("Exception thrown in Test %0!  Test failed!", testIndex++);
				}
			}
		}
	}
	catch (...)
	{
		TBLog::Error("Exception thrown while running calendar system test!  Test failed!");
		return true;
	}

	TBLog::Log("Calendar system test suite complete.");

	return true;
}