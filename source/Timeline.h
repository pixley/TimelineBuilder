/*
	Copyright (c) 2022 Tyler Pixley, all rights reserved.

	This file (TBTimeline.h) is part of TimelineBuilder.

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
#include "Time.h"
#include "JsonableObject.h"

#include <QtCore/QUuid>

struct TBTimelineSettings
{
	int64 MinYear;
	int64 MaxYear;
};

class TBTimeline : public JsonableObject
{
public:
	TBTimeline();

	virtual bool LoadFromJson(const QJsonObject& jsonObject);
	virtual void PopulateJson(QJsonObject& jsonObject) const;

protected:
	// Member variables
	TBTimelineSettings Settings;
	TBDate PresentDate;
	QUuid DefaultCalendarSystem;
	TBMap<QUuid, class TBEra> Eras;
	TBMap<QUuid, class TBEvent> Events;
};