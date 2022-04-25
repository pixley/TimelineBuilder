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

/*
	TBTimeOps
*/
TBTimespan TBTimeOps::TimeBetween(TBDate startDate, TBDate endDate)
{
	return TBTimespan(endDate.GetDays() - startDate.GetDays());
}

TBDate TBTimeOps::GetAdjustedDate(TBDate startDate, TBTimespan deltaTime)
{
	return TBDate(startDate.GetDays() + deltaTime.GetDays());
}