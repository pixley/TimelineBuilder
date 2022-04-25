#pragma once

#include "CommonTypes.h"
#include "Time.h"
#include "JsonableObject.h"

struct TBTimelineSettings
{
	int64 MinYear;
	int64 MaxYear;
	bool ShowDayOfWeek;
};

class TBTimeline : public JsonableObject
{
public:
	virtual void PopulateJson(QJsonObject& jsonObject) const;

	// Member variables
	TBTimelineSettings Settings;
	TBDate PresentDate;
};