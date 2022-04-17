#pragma once

#include "JsonableObject.h"
#include "CommonTypes.h"
#include "Time.h"
#include <QUuid>

class TBEvent : public JsonableObject
{
public:
	// Sorting
	bool operator<(const TBEvent& other) const;
	bool operator>(const TBEvent& other) const;
	bool operator==(const TBEvent& other) const;
	bool operator!=(const TBEvent& other) const;

private:
	// Member variables
	QString EventName;
	QString EventDescription;
	bool HasStartDate;
	bool HasEndDate;
	TBDate StartDate;
	TBDate EndDate;
	TBSignificance Significance;

	QUuid EventID;
	QUuid ParentEventID;
	QList<QUuid> PrerequisiteEvents;
};