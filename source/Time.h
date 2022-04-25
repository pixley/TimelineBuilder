#pragma once

#include "CommonTypes.h"
#include "JsonableObject.h"

// Just so I don't have to type a whole bunch of operator declarations twice.
#define DECLARE_ONE_COMPARISON(type, operatorName) bool operatorName(const type& other) const;
#define DECLARE_ALL_COMPARISONS(type) \
DECLARE_ONE_COMPARISON(type, operator<) \
DECLARE_ONE_COMPARISON(type, operator>) \
DECLARE_ONE_COMPARISON(type, operator==) \
DECLARE_ONE_COMPARISON(type, operator!=) \
DECLARE_ONE_COMPARISON(type, operator>=) \
DECLARE_ONE_COMPARISON(type, operator<=)

/*
	Generic date struct, only storing the number of days since the 0-day of a calendar system
*/
class TBDate
{
private:
	int64 Days;

public:
	TBDate();
	TBDate(int64 inDays);

	int64 GetDays() const { return Days; }

	DECLARE_ALL_COMPARISONS(TBDate)
};

/*
	Timespan struct, only storing the number of days it represents.
*/
class TBTimespan
{
private:
	int64 Days;

public:
	TBTimespan();
	TBTimespan(int64 inDays);

	int64 GetDays() const { return Days; }

	DECLARE_ALL_COMPARISONS(TBTimespan)
};

/*
	Date as broken out into individual int components; values are in ascending order (for example, day then month then year)
*/
typedef QList<int64> TBBrokenDate;

/*
	Timespan as broken out into individual int components; values are in ascending order (for example, days then months then years)
*/
typedef QList<int64> TBBrokenTimespan;

/*
	Class with static methods for manipulating dates and timespans.
*/
class TBTimeOps
{
public:
	static TBTimespan TimeBetween(TBDate startDate, TBDate endDate);
	static TBDate GetAdjustedDate(TBDate startDate, TBTimespan deltaTime);
};