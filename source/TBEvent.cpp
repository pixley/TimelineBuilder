#include "TBEvent.h"

bool TBEvent::operator<(const TBEvent& other) const
{
	return StartDate < other.StartDate;
}

bool TBEvent::operator>(const TBEvent& other) const
{
	return StartDate > other.StartDate;
}

bool TBEvent::operator==(const TBEvent& other) const
{
	return EventID == other.EventID;
}

bool TBEvent::operator!=(const TBEvent& other) const
{
	return EventID != other.EventID;
}