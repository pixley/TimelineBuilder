/*
	Copyright (c) 2022 Tyler Pixley, all rights reserved.

	This file (TBTimeline.cpp) is part of TimelineBuilder.

	TimelineBuilder is free software: you can redistribute it and/or modify it under
	the terms of the GNU General Public License as published by the Free Software
	Foundation, either version 3 of the License, or (at your option) any later version.

	TimelineBuilder is distributed in the hope that it will be useful, but WITHOUT ANY
	WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
	PARTICULAR PURPOSE. See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along with
	TimelineBuilder. If not, see <https://www.gnu.org/licenses/>.
*/

#include "TBTimeline.h"
#include "CommonTypes.h"
#include "TBEra.h"
#include "TBEvent.h"

#include <QtCore/QUuid>
#include <QtCore/QString>

#define JsonObjectToEraMap(jsonObject, key, eraMap) \
JsonObjectToObjectMap<QUuid, TBEra>(jsonObject, key, eraMap, &JsonableObject::StringToUuid)

#define JsonObjectToEventMap(jsonObject, key, eventMap) \
JsonObjectToObjectMap<QUuid, TBEvent>(jsonObject, key, eventMap, &JsonableObject::StringToUuid)

#define EraMapToJsonObject(jsonObject, key, eraMap) \
ObjectMapToJsonObject<QUuid, TBEra>(jsonObject, key, eraMap, &JsonableObject::UuidToString)

#define EventMapToJsonObject(jsonObject, key, eventMap) \
ObjectMapToJsonObject<QUuid, TBEvent>(jsonObject, key, eventMap, &JsonableObject::UuidToString)

TBTimeline::TBTimeline() :
	JsonableObject(),
	Settings(),
	PresentDate(0),
	Eras(),
	Events()
{

}

bool TBTimeline::LoadFromJson(const QJsonObject& jsonObject)
{
	JsonableObject::LoadFromJson(jsonObject);

	Settings.MinYear = JsonToInt64(jsonObject, "min_year");
	Settings.MaxYear = JsonToInt64(jsonObject, "max_year");
	PresentDate = JsonToInt64(jsonObject, "present_date");

	JsonObjectToEraMap(jsonObject, "eras", Eras);
	JsonObjectToEventMap(jsonObject, "events", Events);

	return LoadSuccessful;
}

void TBTimeline::PopulateJson(QJsonObject& jsonObject) const
{
	jsonObject.insert("min_year", Settings.MinYear);
	jsonObject.insert("max_year", Settings.MaxYear);
	jsonObject.insert("present_date", PresentDate.GetDays());

	EraMapToJsonObject(jsonObject, "eras", Eras);
	EventMapToJsonObject(jsonObject, "events", Events);
}