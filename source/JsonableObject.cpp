/*
	Copyright (c) 2022 Tyler Pixley, all rights reserved.

	This file (JsonableObject.cpp) is part of TimelineBuilder.

	TimelineBuilder is free software: you can redistribute it and/or modify it under
	the terms of the GNU General Public License as published by the Free Software
	Foundation, either version 3 of the License, or (at your option) any later version.

	TimelineBuilder is distributed in the hope that it will be useful, but WITHOUT ANY
	WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
	PARTICULAR PURPOSE. See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along with
	TimelineBuilder. If not, see <https://www.gnu.org/licenses/>.
*/

#include "JsonableObject.h"

JsonableObject::JsonableObject() : LoadSuccessful(false)
{

}

bool JsonableObject::LoadFromJson(const QJsonObject& jsonObject)
{
	LoadSuccessful = true;
	return LoadSuccessful;
}

void JsonableObject::JsonToObject(const QJsonObject& jsonObject, const QString& key, JsonableObject& outObject)
{
	QJsonValue objectValue = jsonObject[key];
	if (objectValue.isUndefined() || !objectValue.isObject())
	{
		TBLog::Warning("Error parsing object value for key '%0'.", key);
		outObject.LoadSuccessful = false;
		LoadSuccessful = false;
	}
	else
	{
		outObject.LoadFromJson(objectValue.toObject());
		if (!outObject.IsValid())
		{
			TBLog::Warning("Error loading object for key '%0'.", key);
			LoadSuccessful = false;
		}
	}
}

/*
	Static type check methods for non-JSON types
	These are intended for use in relevant versions of JsonToVariable and JsonArrayToList
*/
bool JsonableObject::IsUuid(const QJsonValue& jsonValue)
{
	if (jsonValue.isString())
	{
		// It would technically be more efficient to only have to create the UUID object once and return that from
		// the JsonToX method, but that would break the pattern established by the template, and I'd rather not have
		// to write yet another variant, since I already have to do that for JsonToObject and JsonArrayToObjectList.
		QUuid testUuid = QUuid::fromString(jsonValue.toString());
		if (!testUuid.isNull())
		{
			// QUuid::fromString returning a non-null UUID means that the string was a valid UUID
			return true;
		}
	}

	return false;
}

QUuid JsonableObject::GetUuid(const QJsonValue& jsonValue)
{
	return QUuid::fromString(jsonValue.toString());
}

/*
	Static to-JSON methods
*/
void JsonableObject::ObjectToJson(QJsonObject& parentObject, const QString& key, const JsonableObject& object)
{
	QJsonObject newJsonObject;
	object.PopulateJson(newJsonObject);
	parentObject.insert(key, newJsonObject);
}

QString JsonableObject::UuidToJson(const QUuid& uuid)
{
	return uuid.toString(QUuid::WithoutBraces);
}