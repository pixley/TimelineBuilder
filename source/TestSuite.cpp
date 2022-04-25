#include "TestSuite.h"

#include "JsonFiles.h"
#include "Calendar.h"

#include <QtGlobal>
#include <QJsonDocument>
#include <QtDebug>

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

	qInfo() << QString("Beginning calendar system test: %0").arg(testSystem);

	QString jsonPath = QString("scripts/%0.json").arg(testSystem);
	TBJsonFile jsonFile(jsonPath, QIODevice::ReadOnly);
	QJsonDocument calendarData;
	EJsonFileResult openResult = jsonFile.GetJsonDocument(calendarData);

	if (openResult == EJsonFileResult::Success)
	{
		qInfo() << "Calendar system data successfully loaded.";
	}
	else
	{
		switch (openResult)
		{
		case EJsonFileResult::FileNotFound:
			qWarning() << QString("Could not open calendar system file (%0).  Test aborted.").arg(jsonPath);
			break;
		case EJsonFileResult::FileNotJson:
			qWarning() << QString("Calendar system file (%0) did not contain valid JSON.  Test aborted.").arg(jsonPath);
			break;
		default:
			qWarning() << QString("Unknown error opening the calendar system file (%s).  Test aborted.").arg(jsonPath);
			break;
		}

		return true;
	}

	TBCalendarSystem calendarSystem;
	calendarSystem.LoadFromJson(calendarData.object());
	if (!calendarSystem.IsValid())
	{
		qWarning() << "Error populating calendar system from JSON data.  Test aborted.";
		return true;
	}

	qInfo() << "Calendar System Info:";
	qInfo() << QString("%0 --- %1").arg(calendarSystem.GetName(), calendarSystem.GetDescription());

	if (!calendarSystem.InitializeScript())
	{
		qWarning() << "Error initializing calendar script.  See Python exception above.  Test aborted.";
		return true;
	}
	
	qInfo() << "Calendar system script initialized.  Beginning test suite.";

	/*
		Test proper begins here.
	*/

	uint32 testIndex = 1;
	// Basic data checks.
	{
		qInfo() << QString("Test %0: Checking length of date format: %1").arg(testIndex++, calendarSystem.GetBrokenDateLength());
	}
	// Date format tests.
	{
		QList<int64> testDays({0, 64, 1024, 1 << 20, -1, -64, -1024});
		for (int64 day : testDays)
		{
			QString formattedDay = calendarSystem.FormatDate(day);
			qInfo() << QString("Test %0: Formatting day %1: %2").arg(testIndex++, day).arg(formattedDay);
		}
	}
	// Broken date format tests
	{
		// Test dates of the form "i/i/i" and "i/i/-i" for i [0,20]
		for (int32 testNum = 0; testNum <= 20; testNum++)
		{
			TBBrokenDate testDate(calendarSystem.GetBrokenDateLength(), testNum);
			QString formattedDate = calendarSystem.FormatDate(testDate);
			qInfo() << QString("Test %0: Formatting date of all '%1': %2").arg(testIndex++, testNum).arg(formattedDate);

			// Now test the corresponding date with negative year (or equivalent)
			testDate.last() *= -1;
			formattedDate = calendarSystem.FormatDate(testDate);
			qInfo() << QString("Test %0: Formatting date of all '%1' with negative year (or equivalent): %2").arg(testIndex++, testNum).arg(formattedDate);
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

				QString formattedSpan = calendarSystem.FormatDateSpan(testDays[indexOne], testDays[indexTwo]);
				qInfo() << QString("Test %0: Formatting span between day %1 and day %2 : %3").arg(testIndex++, testDays[indexOne], testDays[indexTwo]).arg(formattedSpan);
			}
		}
	}
	// Date combination tests
	{
		// Test dates of the form "i/i/i" for i [0,20]
		for (int32 testNum = 0; testNum <= 20; testNum++)
		{
			TBBrokenDate testDate(calendarSystem.GetBrokenDateLength(), testNum);
			if (calendarSystem.ValidateBrokenDate(testDate))
			{
				TBDate combinedDate = calendarSystem.CombineDate(testDate);
				qInfo() << QString("Test %0: Date of all '%1' is day %2").arg(testIndex++, testNum, combinedDate.GetDays());
			}
			else
			{
				qInfo() << QString("Test %0: Date of all '%1' was invalid and could not be combined.").arg(testIndex++, testNum);
			}

			// Now test the corresponding date with negative year (or equivalent)
			testDate.last() *= -1;
			if (calendarSystem.ValidateBrokenDate(testDate))
			{
				TBDate combinedDate = calendarSystem.CombineDate(testDate);
				qInfo() << QString("Test %0: Date of all '%1' with negative year is day %2").arg(testIndex++, testNum, combinedDate.GetDays());
			}
			else
			{
				qInfo() << QString("Test %0: Date of all '%1' with negative year was invalid and could not be combined.").arg(testIndex++, testNum);
			}
		}
	}
	// Date movement tests
	{
		TBDate baseDate(1 << 17);
		for (int32 testNum = 0; testNum <= 20; testNum++)
		{
			TBBrokenTimespan testSpan(calendarSystem.GetBrokenDateLength(), testNum);
			TBDate newDate = calendarSystem.MoveDate(baseDate, testSpan);
			qInfo() << QString("Test %0: Offsetting day %1 by span of all '%2': %3").arg(testIndex++, baseDate.GetDays(), testNum, newDate.GetDays());

			// Now test the corresponding negative offset
			TBBrokenTimespan negSpan(calendarSystem.GetBrokenDateLength(), -testNum);
			newDate = calendarSystem.MoveDate(baseDate, negSpan);
			qInfo() << QString("Test %0: Offsetting day %1 by span of all '-%2': %3").arg(testIndex++, baseDate.GetDays(), testNum, newDate.GetDays());
		}
	}

	qInfo() << "Calendar system test suite complete.";

	return true;
}