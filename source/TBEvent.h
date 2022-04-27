/*
	Copyright (c) 2022 Tyler Pixley, all rights reserved.

	This file (TBEvent.h) is part of TimelineBuilder.

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