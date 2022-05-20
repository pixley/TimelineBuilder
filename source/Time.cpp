/*
	Copyright (c) 2022 Tyler Pixley, all rights reserved.

	This file (Time.cpp) is part of TimelineBuilder.

	TimelineBuilder is free software: you can redistribute it and/or modify it under
	the terms of the GNU General Public License as published by the Free Software
	Foundation, either version 3 of the License, or (at your option) any later version.

	TimelineBuilder is distributed in the hope that it will be useful, but WITHOUT ANY
	WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
	PARTICULAR PURPOSE. See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along with
	TimelineBuilder. If not, see <https://www.gnu.org/licenses/>.
*/

#include "Time.h"

// Just so I don't have to write a whole bunch of operator implementations twice.
#define IMPLEMENT_ONE_COMPARISON(type, operatorName, operatorSymbol, comparedValue) \
bool type::operatorName(const type& other) const { \
	return comparedValue operatorSymbol other.comparedValue; \
}
#define IMPLEMENT_ALL_COMPARISONS(type, comparedValue) \
IMPLEMENT_ONE_COMPARISON(type, operator<, <, comparedValue) \
IMPLEMENT_ONE_COMPARISON(type, operator>, >, comparedValue) \
IMPLEMENT_ONE_COMPARISON(type, operator==, ==, comparedValue) \
IMPLEMENT_ONE_COMPARISON(type, operator!=, !=, comparedValue) \
IMPLEMENT_ONE_COMPARISON(type, operator<=, <=, comparedValue) \
IMPLEMENT_ONE_COMPARISON(type, operator>=, >=, comparedValue)

/*
	TBDate
*/
TBDate::TBDate() : Days(0)
{

}

TBDate::TBDate(int64 inDays) : Days(inDays)
{

}

IMPLEMENT_ALL_COMPARISONS(TBDate, Days)

/*
	TBTimespan
*/
TBTimespan::TBTimespan() : Days(0)
{

}

TBTimespan::TBTimespan(int64 inDays) : Days(inDays)
{

}

IMPLEMENT_ALL_COMPARISONS(TBTimespan, Days)