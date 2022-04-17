#include "Time.h"

/*
	TBDate
*/
TBDate::TBDate() :
	Year(0),
	Month(0),
	Day(0),
	Season(0),
	ValidDate(false)
{

}

TBDate::TBDate(int64 inYear, uint16 inMonth, uint16 inDay, uint8 inSeason) :
	Year(inYear),
	Month(inMonth),
	Day(inDay),
	Season(inSeason),
	ValidDate(true)
{

}

bool TBDate::LoadFromJson(const QJsonObject& jsonObject)
{
	JsonableObject::LoadFromJson(jsonObject);

	Year = JsonToInt64(jsonObject, "year");
	Month = JsonToInt32(jsonObject, "month");
	Day = JsonToInt32(jsonObject, "day");
	Season = JsonToInt32(jsonObject, "season");
	ValidDate = true;

	return LoadSuccessful;
}

void TBDate::PopulateJson(QJsonObject& jsonObject) const
{
	jsonObject["year"] = Year;
	jsonObject["month"] = Month;
	jsonObject["day"] = Day;
	jsonObject["season"] = Season;
}

bool TBDate::IsValid() const
{
	// If a day but no month is specified, then the date is invalid, even if it had originally been validly generated.
	return ValidDate && (Day == 0 || Month != 0);
}

bool TBDate::operator<(const TBDate& other) const
{
	if (Year < other.Year)
	{
		return true;
	}
	else if (Year == other.Year)
	{
		if (Month < other.Month)
		{
			return true;
		}
		else if (Month == other.Month)
		{
			return Day < other.Day;
		}
	}

	return false;
}

bool TBDate::operator>(const TBDate& other) const
{
	return !(*this < other) && *this != other;
}

bool TBDate::operator==(const TBDate& other) const
{
	return Year == other.Year && Month == other.Month && Day == other.Day;
}

bool TBDate::operator!=(const TBDate& other) const
{
	return Year != other.Year || Month != other.Month || Day != other.Day;
}

/*
	TBTimespan
*/
TBTimespan::TBTimespan() :
	Past(false),
	Years(0),
	Months(0),
	Days(0),
	Seasons(0),
	WeeksRemainingAfterYears(0),
	DaysRemainingAfterYears(0),
	WeeksRemainingAfterMonths(0),
	TotalMonths(0),
	TotalWeeks(0),
	TotalDays(0),
	TotalSeasons(0)
{

}

int64 TBTimespan::GetSignedTotalDays() const
{
	return Past ? -((int64)TotalDays) : TotalDays;
}

bool TBTimespan::operator<(const TBTimespan& other) const
{
	if (Past)
	{
		if (other.Past)
		{
			return TotalDays > other.TotalDays;
		}
		else
		{
			return true;
		}
	}
	else
	{
		if (other.Past)
		{
			return false;
		}
		else
		{
			return TotalDays < other.TotalDays;
		}
	}
}

bool TBTimespan::operator>(const TBTimespan& other) const
{
	return !(*this < other) && *this != other;
}

bool TBTimespan::operator==(const TBTimespan& other) const
{
	return (Past == other.Past) && (TotalDays == other.TotalDays);
}

bool TBTimespan::operator!=(const TBTimespan& other) const
{
	return (Past != other.Past) || (TotalDays != other.TotalDays);
}