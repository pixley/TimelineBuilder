/*
	Copyright (c) 2022 Tyler Pixley, all rights reserved.

	This file (TBEra.h) is part of TimelineBuilder.

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
#include <QtCore/QUuid>
#include <QtCore/QString>

class TBEra : public JsonableObject
{
public:
	TBEra();

	virtual bool LoadFromJson(const QJsonObject& jsonObject) override;
	virtual void PopulateJson(QJsonObject& jsonObject) const override;

private:
	// Member variables
	QString Name;
	QString Description;
	TBPeriodBounds BoundsType;

	// The start and end dates are in the base calendar system for the timeline
	TBBrokenDate StartDate;
	TBBrokenDate EndDate;

	// Null UUID if we don't override the base calendar system for the timeline
	QUuid CalendarOverride;

	QUuid EraID;
};