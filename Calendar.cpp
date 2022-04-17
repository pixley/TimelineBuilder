#include "Calendar.h"
#include <cmath>

TBMonthSettings::TBMonthSettings() : JsonableObject(),
	Name(""),
	Abbreviation(""),
	Days(0),
	LeapDays(0)
{}

bool TBMonthSettings::LoadFromJson(const QJsonObject& jsonObject)
{
	JsonableObject::LoadFromJson(jsonObject);

	Name = JsonToString(jsonObject, "name");
	Abbreviation = JsonToString(jsonObject, "abbreviation");
	Days = JsonToInt32(jsonObject, "days");
	LeapDays = JsonToInt32(jsonObject, "leapDays");

	return LoadSuccessful;
}

void TBMonthSettings::PopulateJson(QJsonObject& jsonObject) const
{
	jsonObject.insert("name", Name);
	jsonObject.insert("abbreviation", Abbreviation);
	jsonObject.insert("days", Days);
	jsonObject.insert("leapDays", LeapDays);
}

TBWeekdaySettings::TBWeekdaySettings() : JsonableObject(),
	Name(""),
	Abbreviation(""),
	MinimalAbbv("")
{}

bool TBWeekdaySettings::LoadFromJson(const QJsonObject& jsonObject)
{
	JsonableObject::LoadFromJson(jsonObject);

	Name = JsonToString(jsonObject, "name");
	Abbreviation = JsonToString(jsonObject, "abbreviation");
	MinimalAbbv = JsonToString(jsonObject, "minimalAbbv");

	return LoadSuccessful;
}

void TBWeekdaySettings::PopulateJson(QJsonObject& jsonObject) const
{
	jsonObject.insert("name", Name);
	jsonObject.insert("abbreviation", Abbreviation);
	jsonObject.insert("minimalAbbv", MinimalAbbv);
}

TBSeasonSettings::TBSeasonSettings() : JsonableObject(),
	Name(""),
	StartDate(),
	EndDate()
{}

bool TBSeasonSettings::LoadFromJson(const QJsonObject& jsonObject)
{
	JsonableObject::LoadFromJson(jsonObject);

	Name = JsonToString(jsonObject, "name");
	JsonToObject(jsonObject, "startDate", StartDate);
	JsonToObject(jsonObject, "endDate", EndDate);

	return LoadSuccessful;
}

void TBSeasonSettings::PopulateJson(QJsonObject& jsonObject) const
{
	jsonObject.insert("name", Name);
	ObjectToJson(jsonObject, "startDate", StartDate);
	ObjectToJson(jsonObject, "endDate", EndDate);
}

TBCalendarSettings::TBCalendarSettings() : JsonableObject(),
	LeapYearFrequency(0),
	Months(),
	Weekdays(),
	Seasons(),
	WeekdayOfStartDate(0),
	StartDateOverride(),
	HasYearZero(false),
	YearAffix(""),
	YearPrefix(false),
	HideNegativeYearSign(false),
	DateFormat(TBDateFormat::_INVALIDVALUE_),
	MonthFormat(TBMonthFormat::_INVALIDVALUE_),
	WeekdayFormat(TBWeekdayFormat::_INVALIDVALUE_),
	RoundingMethod(TBDateRounding::_INVALIDVALUE_)
{}

bool TBCalendarSettings::LoadFromJson(const QJsonObject& jsonObject)
{
	JsonableObject::LoadFromJson(jsonObject);

	LeapYearFrequency = JsonToInt32(jsonObject, "leapYearFreq");
	JsonArrayToObjectList(jsonObject, "months", Months);
	JsonArrayToObjectList(jsonObject, "weekdays", Weekdays);
	JsonArrayToObjectList(jsonObject, "seasons", Seasons);
	WeekdayOfStartDate = JsonToInt32(jsonObject, "startDateDay");
	JsonToObject(jsonObject, "startOverride", StartDateOverride);
	HasYearZero = JsonToBool(jsonObject, "hasYear0");

	YearAffix = JsonToString(jsonObject, "yearAffix");
	YearPrefix = JsonToBool(jsonObject, "yearPrefix");
	HideNegativeYearSign = JsonToBool(jsonObject, "hideYearSign");
	DateFormat = JsonToEnum(jsonObject, "dateFormat", TBDateFormat);
	MonthFormat = JsonToEnum(jsonObject, "monthFormat", TBMonthFormat);
	WeekdayFormat = JsonToEnum(jsonObject, "weekdayFormat", TBWeekdayFormat);
	RoundingMethod = JsonToEnum(jsonObject, "roundingMethod", TBDateRounding);

	return LoadSuccessful;
}

void TBCalendarSettings::PopulateJson(QJsonObject& jsonObject) const
{
	jsonObject.insert("leapYearFreq", LeapYearFrequency);
	ObjectListToJsonArray(jsonObject, "months", Months);
	ObjectListToJsonArray(jsonObject, "weekdays", Weekdays);
	ObjectListToJsonArray(jsonObject, "seasons", Seasons);
	jsonObject.insert("startDateDay", EnumToJson(WeekdayOfStartDate));
	ObjectToJson(jsonObject, "startOverride", StartDateOverride);
	jsonObject.insert("hasYear0", HasYearZero);

	jsonObject.insert("yearAffix", YearAffix);
	jsonObject.insert("yearPrefix", YearPrefix);
	jsonObject.insert("hideYearSign", HideNegativeYearSign);
	jsonObject.insert("dateFormat", EnumToJson(DateFormat));
	jsonObject.insert("monthFormat", EnumToJson(MonthFormat));
	jsonObject.insert("weekdayFormat", EnumToJson(WeekdayFormat));
	jsonObject.insert("roundingMethod", EnumToJson(RoundingMethod));
}

QString TBCalendarSettings::FormatDate(const TBDate& date) const
{
	if (!date.IsValid())
	{
		return "INVALID DATE";
	}

	QString workingString = "";

	const bool dateHasMonth = date.Month > 0;
	const bool dateHasDay = dateHasMonth && date.Day > 0;
	const bool dateHasSeason = !dateHasMonth && !dateHasDay && date.Season > 0;

	QString weekdayString = "";
	if (dateHasDay && WeekdayFormat != TBWeekdayFormat::None)
	{
		int8 dayOfWeek = GetDayOfWeek(date);

		if (dayOfWeek >= Weekdays.length() - 1)
		{
			return "INVALID WEEK DAY";
		}

		const TBWeekdaySettings& weekday = Weekdays[dayOfWeek];
		switch (WeekdayFormat)
		{
		case TBWeekdayFormat::Full:
			weekdayString = weekday.Name;
			break;
		case TBWeekdayFormat::Abbreviated:
			weekdayString = weekday.Abbreviation;
			break;
		default:
			return "INVALID WEEK DAY CONFIG";
		}
	}

	QString dayString = "";
	if (dateHasDay)
	{
		dayString = QString::number(date.Day);
	}

	QString monthString = "";
	if (dateHasMonth)
	{
		if (date.Month >= Months.length())
		{
			return "INVALID MONTH";
		}

		const TBMonthSettings& month = Months[date.Month - 1];
		switch (MonthFormat)
		{
		case TBMonthFormat::Name:
			monthString = month.Name;
			break;
		case TBMonthFormat::Abbreviation:
			monthString = month.Abbreviation;
			break;
		case TBMonthFormat::Number:
			monthString = QString::number(date.Month);
			break;
		default:
			return "INVALID MONTH CONFIG";
		}
	}

	QString seasonString = "";
	if (dateHasSeason)
	{
		if (date.Season >= Seasons.length())
		{
			return "INVALID SEASON";
		}

		seasonString = Seasons[date.Season - 1].Name;
	}

	QString yearString = "";
	int64 workingYear = date.Year;
	if (HideNegativeYearSign)
	{
		workingYear = abs(workingYear);
	}
	if (!YearAffix.isEmpty())
	{
		const QString yearFormatString = YearPrefix ? "%2 %1" : "%1 %2";
		yearString = yearFormatString.arg(QString::number(workingYear)).arg(YearAffix);
	}
	else
	{
		yearString = QString::number(workingYear);
	}
	
	/*
		The string arguments being put forth are as follows:
		%1 - day
		%2 - month
		%3 - year
		%4 - separator between month and day
		%5 - separator between month and year
	*/
	switch (DateFormat)
	{
	case TBDateFormat::DMY:
		workingString = "%1%4%2%5%3";
		break;
	case TBDateFormat::MDY:
		workingString = "%2%5%1%4%3";
		break;
	case TBDateFormat::YMD:
		workingString = "%3%5%2%4%1";
		break;
	default:
		return "INVALID DATE FORMAT";
	}

	// Use dash if we have a month and it's numeric
	QString separator = (dateHasMonth && MonthFormat == TBMonthFormat::Number) ? "-" : " ";

	if (dateHasSeason)
	{
		workingString = workingString.arg("").arg(seasonString).arg(yearString).arg("").arg(separator);
	}
	else if (dateHasDay)
	{
		workingString = workingString.arg(dayString).arg(monthString).arg(yearString).arg(separator).arg(separator);
	}
	else if (dateHasMonth)
	{
		workingString = workingString.arg("").arg(monthString).arg(yearString).arg("").arg(separator);
	}
	else
	{
		workingString = workingString.arg("").arg("").arg(yearString).arg("").arg("");
	}

	return workingString;
}

TBTimespan TBCalendarSettings::CreateTimespan(const TBDate& startDate, const TBDate& endDate) const
{
	if (!startDate.IsValid() || !endDate.IsValid())
	{
		return TBTimespan();
	}

	const bool datesReversed = startDate > endDate;
	const TBDate& dateOne = datesReversed ? endDate : startDate;
	const TBDate& dateTwo = datesReversed ? startDate : endDate;
	// After this point, DO NOT use startDate and endDate; use dateOne and dateTwo

	// Because we've made sure that dateOne comes before (or is equal to) dateTwo, their year difference will always be non-negative
	uint64 entireYearsBetween = dateTwo.Year - dateOne.Year;
	if (!HasYearZero && ((dateTwo.Year < 0) != (dateOne.Year < 0)))
	{
		entireYearsBetween -= 1;
	}

	// Counting leap years
	uint64 leapYearsBetween = 0;
	if (entireYearsBetween > 0 && LeapYearFrequency > 0)
	{
		leapYearsBetween = entireYearsBetween / LeapYearFrequency;

		int16 dateOneCyclePosition = dateOne.Year % LeapYearFrequency;
		int16 dateTwoCyclePosition = dateTwo.Year % LeapYearFrequency;
		// The modulo operator returns a negative result if the signs of the operands are different.
		// To compensate for this, we add the leap year frequency to a negative cycle position to get the true positive position.
		if (dateOneCyclePosition < 0)
		{
			dateOneCyclePosition += LeapYearFrequency;
		}
		if (dateTwoCyclePosition < 0)
		{
			dateTwoCyclePosition += LeapYearFrequency;
		}

		// If date one is further in its leap year cycle than date two is, then an extra leap year lies between them.
		if (dateOneCyclePosition > dateTwoCyclePosition)
		{
			leapYearsBetween++;
		}
	}

	// Count the days in a year
	uint64 daysInNormalYear = 0;
	uint64 leapDays = 0;
	for (const TBMonthSettings month : Months)
	{
		daysInNormalYear += month.Days;
		leapDays += month.LeapDays;
	}

	// Now we have all the information we need to count the total whole years' worth of days between the two dates.
	const uint64 totalDaysOfWholeNormalYears = daysInNormalYear * (entireYearsBetween - leapYearsBetween);
	const uint64 totalDaysOfWholeLeapYears = (daysInNormalYear + leapDays) * leapYearsBetween;

	// Next step: find timespan until end of year for dateOne and days after start of year for dateTwo
	const TBTimespan dateOneToYearEnd = GetTimeUntilYearEnd(dateOne);
	const TBTimespan dateTwoFromYearStart = GetTimeSinceYearStart(dateTwo);

	// We can start assembling the return timespan
	TBTimespan outTimespan;

	const uint32 daysInWeek = Weekdays.length();
	outTimespan.Past = datesReversed;
	outTimespan.TotalDays = totalDaysOfWholeNormalYears + totalDaysOfWholeLeapYears + dateOneToYearEnd.TotalDays + dateTwoFromYearStart.TotalDays;
	outTimespan.TotalWeeks = outTimespan.TotalDays / daysInWeek;

	outTimespan.WeeksRemainingAfterYears = outTimespan.DaysRemainingAfterYears / daysInWeek;

	return outTimespan;
}

TBTimespan TBCalendarSettings::GetTimeSinceDay1(const TBDate& date) const
{
	return CreateTimespan(TBDate(1, 1, 1), date);
}

TBTimespan TBCalendarSettings::GetTimeSinceStartDate(const TBDate& date) const
{
	if (!StartDateOverride.IsValid())
	{
		return GetTimeSinceDay1(date);
	}
	else
	{
		return CreateTimespan(StartDateOverride, date);
	}
}

TBTimespan TBCalendarSettings::GetTimeSinceYearStart(const TBDate& date) const
{
	

	return TBTimespan();
}

TBTimespan TBCalendarSettings::GetTimeUntilYearEnd(const TBDate& date) const
{
	return TBTimespan();
}

int8 TBCalendarSettings::GetDayOfWeek(const TBDate& date) const
{
	if (!date.IsValid())
	{
		return -1;
	}
	if (Weekdays.isEmpty())
	{
		return -1;
	}
	if (date.Month == 0 || date.Day == 0)
	{
		return -1;
	}

	const TBTimespan timeSinceDay1 = GetTimeSinceStartDate(date);
	return (timeSinceDay1.TotalDays + WeekdayOfStartDate) % Weekdays.length();
}

uint16 TBCalendarSettings::GetDaysInSeason(const TBSeasonSettings& season, int64 year) const
{
	TBDate seasonStart(year, season.StartDate.Month, season.StartDate.Day);
	TBDate seasonEnd(year, season.EndDate.Month, season.EndDate.Day);

	// Account for the case where a season straddles the new year
	if (seasonEnd < seasonStart)
	{
		seasonEnd.Year += 1;
	}

	return 0;
}

uint16 TBCalendarSettings::GetDaysInSeason(uint32 seasonIndex, int64 year) const
{
	if (seasonIndex < Seasons.count())
	{
		return GetDaysInSeason(Seasons[seasonIndex], year);
	}
	else
	{
		return 0;
	}
}