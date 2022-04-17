#pragma once

#include "CommonTypes.h"
#include "JsonableObject.h"
#include "Time.h"
#include <QString>
#include <QList>

class TBMonthSettings : public JsonableObject
{
public:
	TBMonthSettings();
	explicit TBMonthSettings(const QJsonObject& jsonObject) : JsonableObject(jsonObject) {}
	virtual bool LoadFromJson(const QJsonObject& jsonObject) override;
	virtual void PopulateJson(QJsonObject& jsonObject) const override;

	QString Name;
	QString Abbreviation;	// Abbreviated name for the month, akin to "Sept" for September or "Nov" for November
	uint16 Days;			// How many days are present in this month during a normal year
	uint8 LeapDays;			// How many additional days are present in this month during a leap year
};

class TBWeekdaySettings : public JsonableObject
{
public:
	TBWeekdaySettings();
	explicit TBWeekdaySettings(const QJsonObject& jsonObject) : JsonableObject(jsonObject) {}
	virtual bool LoadFromJson(const QJsonObject& jsonObject) override;
	virtual void PopulateJson(QJsonObject& jsonObject) const override;

	QString Name;
	QString Abbreviation;	// Abbreviated name for the day of the week, akin to "Tue" for Tuesday or "Sat" for Saturday
	QString MinimalAbbv;	// Super-abbreviated name for the day of the week, akin to "Tu" for Tuesday or "F" for Friday
};

class TBSeasonSettings : public JsonableObject
{
public:
	TBSeasonSettings();
	explicit TBSeasonSettings(const QJsonObject& jsonObject) : JsonableObject(jsonObject) {}
	virtual bool LoadFromJson(const QJsonObject& jsonObject) override;
	virtual void PopulateJson(QJsonObject& jsonObject) const override;

	QString Name;
	TBDate StartDate;		// Only month and day fields are relevant
	TBDate EndDate;			// Only month and day fields are relevant
};

class TBCalendarSettings : public JsonableObject
{
public:
	TBCalendarSettings();
	explicit TBCalendarSettings(const QJsonObject& jsonObject) : JsonableObject(jsonObject) {}
	virtual bool LoadFromJson(const QJsonObject& jsonObject) override;
	virtual void PopulateJson(QJsonObject& jsonObject) const override;

	QString FormatDate(const TBDate& date) const;

	TBTimespan CreateTimespan(const TBDate& startDate, const TBDate& endDate) const;

	TBTimespan GetTimeSinceDay1(const TBDate& date) const;
	TBTimespan GetTimeSinceStartDate(const TBDate& date) const;		//Identical to GetTimeSinceDay1() unless StartDateOverride is valid
	TBTimespan GetTimeSinceYearStart(const TBDate& date) const;
	TBTimespan GetTimeUntilYearEnd(const TBDate& date) const;

	int8 GetDayOfWeek(const TBDate& date) const;

	uint16 GetDaysInSeason(const TBSeasonSettings& season, int64 year) const;
	uint16 GetDaysInSeason(uint32 seasonIndex, int64 year) const;

private:
	// Time parameters
	uint16 LeapYearFrequency;			// How many years from one leap year to the next; on Earth, this would be 4; 0 means no leap years
	// I'll deal with funky leap year situations later, like how Earth years divisible by 100 aren't leap years, but ones divisible by 400 are
	QList<TBMonthSettings> Months;
	QList<TBWeekdaySettings> Weekdays;
	QList<TBSeasonSettings> Seasons;
	uint8 WeekdayOfStartDate;			// What day of the week it was for the start date of the era (usually Day 1, Month 1, Year 1); starts at 0
	TBDate StartDateOverride;			// Not relevant for the timeline's default settings, but for eras where they reset the calendar, but not to 1/1/1
	bool HasYearZero;					// Many calendars go straight from -1 to 1; if that is not desired, then this should be true

	// Formatting
	QString YearAffix;
	bool YearPrefix;					// When true, the YearAffix will be written before the year; when false, it will be written after
	bool HideNegativeYearSign;			// For backward-counting calendars, such as Earth's BCE, where negative years are the default, we can hide the negative sign
	TBDateFormat DateFormat;
	TBMonthFormat MonthFormat;
	TBWeekdayFormat WeekdayFormat;
	TBDateRounding RoundingMethod;
};