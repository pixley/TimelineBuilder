#pragma once

#include "CommonTypes.h"
#include "JsonableObject.h"

/*
	Generic date struct
	Note that this contains no bounds on what values are valid, as the timeline's settings drive validity.
*/
class TBDate : public JsonableObject
{
public:
	TBDate();
	TBDate(int64 inYear, uint16 inMonth = 0, uint16 inDay = 0, uint8 inSeason = 0);
	explicit TBDate(const QJsonObject& jsonObject) : JsonableObject(jsonObject) {}

	bool IsValid() const;

	bool operator<(const TBDate& other) const;
	bool operator>(const TBDate& other) const;
	bool operator==(const TBDate& other) const;
	bool operator!=(const TBDate& other) const;

	virtual bool LoadFromJson(const QJsonObject& jsonObject) override;
	virtual void PopulateJson(QJsonObject& jsonObject) const override;

	int64 Year;			// Years can be negative.
	uint16 Month;		// Being 0 means that the month is unspecified
	uint16 Day;			// Being 0 means that the day is unspecified; ignored if Month is 0
	uint8 Season;		// Only specified if Month is 0

private:
	bool ValidDate;
};

/*
	Generic timespan struct
	As with TBDate, timeline settings drive validity, not set bounds
*/
struct TBTimespan
{
	TBTimespan();

	int64 GetSignedTotalDays() const;

	bool operator<(const TBTimespan& other) const;
	bool operator>(const TBTimespan& other) const;
	bool operator==(const TBTimespan& other) const;
	bool operator!=(const TBTimespan& other) const;

	bool Past;							// Indicates whether the timespan is reversed (the dates compared were in reverse chronological order)

	uint64 Years;						// Absolute value of years
	uint16 Months;						// How many months beyond whole years
	uint16 Days;						// How many days beyond whole months
	uint8 Seasons;						// How many seasons beyond whole years; only used when one or both compared dates only have season-level precision

	uint16 WeeksRemainingAfterYears;	// How many weeks beyond whole years
	uint16 DaysRemainingAfterYears;		// How many days beyond whole years

	uint8 WeeksRemainingAfterMonths;	// How many weeks beyond whole months

	uint64 TotalMonths;					// Total months represented by the timespan, rounded down
	uint64 TotalWeeks;					// Total weeks represented by the timespan, rounded down
	uint64 TotalDays;					// Total days represented by the timespan
	uint64 TotalSeasons;				// Total seasons represented by the timespan; only used when one or both compared dates only have season-level precision
};