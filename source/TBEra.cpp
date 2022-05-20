/*
	Copyright (c) 2022 Tyler Pixley, all rights reserved.

	This file (TBEra.cpp) is part of TimelineBuilder.

	TimelineBuilder is free software: you can redistribute it and/or modify it under
	the terms of the GNU General Public License as published by the Free Software
	Foundation, either version 3 of the License, or (at your option) any later version.

	TimelineBuilder is distributed in the hope that it will be useful, but WITHOUT ANY
	WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
	PARTICULAR PURPOSE. See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along with
	TimelineBuilder. If not, see <https://www.gnu.org/licenses/>.
*/

#include "TBEra.h"

TBEra::TBEra() : JsonableObject(),
	Name(),
	Description(),
	BoundsType(TBPeriodBounds::_INVALIDVALUE_),
	StartDate(),
	EndDate(),
	CalendarOverride(),
	EraID()
{}

bool TBEra::LoadFromJson(const QJsonObject& jsonObject)
{
	JsonableObject::LoadFromJson(jsonObject);

	Name = JsonToString(jsonObject, "name");
	Description = JsonToString(jsonObject, "description");
	BoundsType = JsonToEnum(jsonObject, "bounds_type", TBPeriodBounds);
	JsonArrayToBrokenDate(jsonObject, "start_date", StartDate);
	JsonArrayToBrokenDate(jsonObject, "end_date", EndDate);
	CalendarOverride = JsonToUuid(jsonObject, "calendar_override");
	EraID = JsonToUuid(jsonObject, "id");

	return LoadSuccessful;
}

void TBEra::PopulateJson(QJsonObject& jsonObject) const
{
	jsonObject.insert("name", Name);
	jsonObject.insert("description", Description);
	jsonObject.insert("bounds_type", EnumToJson(BoundsType));
	BrokenDateToJsonArray(jsonObject, "start_date", StartDate);
	BrokenDateToJsonArray(jsonObject, "end_date", EndDate);
	jsonObject.insert("calendar_override", UuidToJson(CalendarOverride));
	jsonObject.insert("id", UuidToJson(EraID));
}