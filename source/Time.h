/*
	Copyright (c) 2022 Tyler Pixley, all rights reserved.

	This file (Time.h) is part of TimelineBuilder.

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

#include "CommonTypes.h"

// Aliases so that A) this doesn't have to be done a bunch of places, and B) people don't have to remember
// that TBBrokenDate is a QList<int64> all the time.
// Please remember to include "JsonableObject.h" when using these.
#define JsonArrayToBrokenDate JsonArrayToInt64List
#define BrokenDateToJsonArray(parentObject, key, inDate) ListToJsonArray(parentObject, key, inDate)

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
	Date as broken out into individual int components; values are in descending order (for example, year then month then day)
*/
typedef QList<int64> TBBrokenDate;

/*
	Timespan as broken out into individual int components; values are in descending order (for example, years then months then days)
*/
typedef QList<int64> TBBrokenTimespan;

// Defines how events/eras durations are bounded
ENUM_CLASS(TBPeriodBounds, uint8,
	NoDuration,
	StartOnly,
	EndOnly,
	StartAndEnd
)