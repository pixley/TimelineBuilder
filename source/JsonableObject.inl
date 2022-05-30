/*
	Copyright (c) 2022 Tyler Pixley, all rights reserved.

	This file (JsonableObject.inl) is part of TimelineBuilder.

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
#include "Logging.h"

#include <utility>

/*
	Boilerplate macros for use in the type aliases.
*/


/*
	Templated type-check and get-from-JSON methods
*/
template<typename E>
bool JsonableObject::IsEnum(const QJsonValue& jsonValue)
{
	if (jsonValue.isDouble())
	{
		E castEnum = static_cast<E>(jsonValue.toInteger());
		if (EnumValueIsValid<E>(castEnum))
		{
			return true;
		}
	}

	return false;
}

template<typename E>
E JsonableObject::GetEnum(const QJsonValue& jsonValue)
{
	return static_cast<E>(jsonValue.toInteger());
}

/*
	From-JSON conversion methods and alias macros
*/
// TypeCheckMethodRef and GetMethodRef should be function references/pointers, such as std::function<>, as shown below
template<typename T>
T JsonableObject::JsonToVariable(const QJsonObject& jsonObject, const QString& key,
	std::function<bool(const QJsonValue&)> typeCheckMethod, std::function<T(const QJsonValue&)> getMethod, T defaultValue)
{
	if (!jsonObject.contains(key))
	{
		LoadSuccessful = false;
		TBLog::Warning("No value for key '%0'", key);
		return defaultValue;
	}

	QJsonValue jsonValue = jsonObject[key];
	if (jsonValue.isUndefined() || !typeCheckMethod(jsonValue))
	{
		LoadSuccessful = false;
		TBLog::Warning("Error parsing value for key '%0'.", key);
		return defaultValue;
	}
	else
	{
		return getMethod(jsonValue);
	}
}

template<typename T>
void JsonableObject::JsonArrayToList(const QJsonObject& jsonObject, const QString& key, QList<T>& outList,
	std::function<bool(const QJsonValue&)> typeCheckMethod, std::function<T(const QJsonValue&)> getMethod)
{
	if (!jsonObject.contains(key))
	{
		LoadSuccessful = false;
		TBLog::Warning("No value for key '%0'", key);
	}

	QJsonValue listValue = jsonObject[key];
	if (listValue.isUndefined() || !listValue.isArray())
	{
		TBLog::Warning("Error parsing array value for key '%0'.", key);
		LoadSuccessful = false;
	}
	else
	{
		QJsonArray jsonArray = listValue.toArray();
		outList.reserve(jsonArray.count());
		for (const QJsonValue& arrayElem : jsonArray)
		{
			if (!arrayElem.isUndefined() && typeCheckMethod(arrayElem))
			{
				outList.emplaceBack(getMethod(arrayElem));
			}
			else
			{
				TBLog::Warning("Error parsing array element for key '%0'.", key);
				LoadSuccessful = false;
				break;
			}
		}
	}
}

template<typename ValueType>
	void JsonableObject::JsonObjectToMap(const QJsonObject& jsonObject, const QString& key, TBMap<QString, ValueType>& outMap,
		std::function<bool(const QJsonValue&)> typeCheckMethod, std::function<ValueType(const QJsonValue&)> getMethod)
{
	if (!jsonObject.contains(key))
	{
		LoadSuccessful = false;
		TBLog::Warning("No value for key '%0'", key);
	}

	QJsonValue mapValue = jsonObject[key];
	if (mapValue.isUndefined() || !mapValue.isObject())
	{
		TBLog::Warning("Error parsing map value for key '%0'", key);
		LoadSuccessful = false;
	}
	else
	{
		QJsonObject mapObject = mapValue.toObject();
		outMap.reserve(mapObject.length);
		for (QJsonObject::ConstIterator keyValPair : outMap)
		{
			const QJsonValue& value = keyValPair.value;
			if (!value.isUndefined() && typeCheckMethod(value))
			{
				outMap.emplace(keyValPair.key, getMethod(value));
			}
			else
			{
				TBLog::Warning("Error parsing map element for key '%0'", key);
				LoadSuccessful = false;
				break;
			}
		}
	}
}

template<NotStringType KeyType, typename ValueType>
void JsonableObject::JsonObjectToMap(const QJsonObject& jsonObject, const QString& key, TBMap<KeyType, ValueType>& outMap,
	std::function<bool(const QJsonValue&)> typeCheckMethod, std::function<ValueType(const QJsonValue&)> getMethod,
	std::function<KeyType(const QString&)> keyConverter)
{
	if (!jsonObject.contains(key))
	{
		LoadSuccessful = false;
		TBLog::Warning("No value for key '%0'", key);
	}

	QJsonValue mapValue = jsonObject[key];
	if (mapValue.isUndefined() || !mapValue.isObject())
	{
		TBLog::Warning("Error parsing map value for key '%0'", key);
		LoadSuccessful = false;
	}
	else
	{
		QJsonObject mapObject = mapValue.toObject();
		outMap.reserve(mapObject.length);
		for (QJsonObject::ConstIterator keyValPair : outMap)
		{
			const QJsonValue& value = keyValPair.value;
			if (!value.isUndefined() && typeCheckMethod(value))
			{
				outMap.emplace(keyConverter(keyValPair.key), getMethod(value));
			}
			else
			{
				TBLog::Warning("Error parsing map element for key '%0'", key);
				LoadSuccessful = false;
				break;
			}
		}
	}
}

template<IsA<JsonableObject> T>
void JsonableObject::JsonArrayToObjectList(const QJsonObject& jsonObject, const QString& key, QList<T>& outList)
{
	if (!jsonObject.contains(key))
	{
		LoadSuccessful = false;
		TBLog::Warning("No value for key '%0'", key);
	}

	QJsonValue listValue = jsonObject[key];
	if (listValue.isUndefined() || !listValue.isArray())
	{
		TBLog::Warning("Error parsing array value for key '%0'.", key);
		LoadSuccessful = false;
	}
	else
	{
		QJsonArray jsonArray = listValue.toArray();
		outList.reserve(jsonArray.count());
		for (const QJsonValue& arrayElem : jsonArray)
		{
			if (arrayElem.isUndefined() || !arrayElem.isObject())
			{
				LoadSuccessful = false;
				TBLog::Warning("Error parsing array element for key '%0'.", key);
				break;
			}
			else
			{
				T& newObject = outList.emplaceBack();
				newObject.LoadFromJson(arrayElem.toObject());
				if (!newObject.IsValid())
				{
					TBLog::Warning("Error loading object within array for key '%0'.", key);
					LoadSuccessful = false;
					break;
				}
			}
		}
	}
}

template<IsA<JsonableObject> ObjectType>
void JsonableObject::JsonObjectToObjectMap(const QJsonObject& jsonObject, const QString& key, TBMap<QString, ObjectType>& outMap)
{
	if (!jsonObject.contains(key))
	{
		LoadSuccessful = false;
		TBLog::Warning("No value for key '%0'", key);
	}

	QJsonValue mapValue = jsonObject[key];
	if (mapValue.isUndefined() || !mapValue.isObject())
	{
		TBLog::Warning("Error parsing map value for key '%0'", key);
		LoadSuccessful = false;
	}
	else
	{
		QJsonObject mapObject = mapValue.toObject();
		outMap.reserve(mapObject.length);
		for (QJsonObject::ConstIterator keyValPair : outMap)
		{
			const QJsonValue& value = keyValPair.value;
			if (!value.isUndefined() && value.isObject())
			{
				outMap.emplace(keyValPair.key);
				ObjectType& newObject = outMap[keyValPair.key];
				newObject.LoadFromJson(value.toObject());
				if (!newObject.IsValid())
				{
					TBLog::Warning("Error loading object within map for key '%0'.", key);
					LoadSuccessful = false;
					break;
				}
			}
			else
			{
				TBLog::Warning("Error parsing map element for key '%0'", key);
				LoadSuccessful = false;
				break;
			}
		}
	}
}

template<NotStringType KeyType, IsA<JsonableObject> ObjectType>
void JsonableObject::JsonObjectToObjectMap(const QJsonObject& jsonObject, const QString& key,
	TBMap<KeyType, ObjectType>& outMap, std::function<KeyType(const QString&)> keyConverter)
{
	if (!jsonObject.contains(key))
	{
		LoadSuccessful = false;
		TBLog::Warning("No value for key '%0'", key);
	}

	QJsonValue mapValue = jsonObject[key];
	if (mapValue.isUndefined() || !mapValue.isObject())
	{
		TBLog::Warning("Error parsing map value for key '%0'", key);
		LoadSuccessful = false;
	}
	else
	{
		QJsonObject mapObject = mapValue.toObject();
		outMap.reserve(mapObject.length());
		for (QJsonObject::ConstIterator keyValPair = mapObject.begin(); keyValPair != mapObject.end(); keyValPair++)
		{
			const QJsonValue& value = keyValPair.value();
			if (!value.isUndefined() && value.isObject())
			{
				KeyType convertedKey = keyConverter(keyValPair.key());
				outMap.emplace(convertedKey);
				ObjectType& newObject = outMap[convertedKey];
				newObject.LoadFromJson(value.toObject());
				if (!newObject.IsValid())
				{
					TBLog::Warning("Error loading object within map for key '%0'.", key);
					LoadSuccessful = false;
					break;
				}
			}
			else
			{
				TBLog::Warning("Error parsing map element for key '%0'", key);
				LoadSuccessful = false;
				break;
			}
		}
	}
}

/*
	Templated to-JSON conversion methods
*/

template<typename E>
int64 JsonableObject::EnumToJson(E enumValue)
{
	return static_cast<int64>(enumValue);
}

/*
	List-to-JsonArray methods and alias macros
*/

// For use with types that have direct QJsonValue conversions
template<typename ElemType>
void JsonableObject::ListToJsonArray(QJsonObject& parentObject, const QString& key, const QList<ElemType>& inList)
{
	// QJsonArray does not have a reserve/preallocate method.  Otherwise, I would put one here.
	QJsonArray outArray;
	for (const ElemType& element : inList)
	{
		outArray.push_back(element);
	}

	parentObject.insert(key, outArray);
}

// For use with that have custom conversions, such as listed in the macros below.
template<typename ListElemType, typename ArrayElemType>
void JsonableObject::ListToJsonArray(QJsonObject& parentObject, const QString& key, const QList<ListElemType>& inList,
	std::function<ArrayElemType(const ListElemType&)> converter)
{
	// QJsonArray does not have a reserve/preallocate method.  Otherwise, I would put one here.
	QJsonArray outArray;
	for (const ListElemType& element : inList)
	{
		ArrayElemType convertedElement = converter(element);
		outArray.push_back(convertedElement);
	}

	parentObject.insert(key, outArray);
}

template<IsA<JsonableObject> T>
void JsonableObject::ObjectListToJsonArray(QJsonObject& parentObject, const QString& key, const QList<T>& inList)
{
	// QJsonArray does not have a reserve/preallocate method.  Otherwise, I would put one here.
	QJsonArray outArray;
	for (const T& element : inList)
	{
		QJsonObject jsonObject;
		element.PopulateJson(jsonObject);
		outArray.push_back(jsonObject);
	}

	parentObject.insert(key, outArray);
}

template<typename ValueType>
void JsonableObject::MapToJsonObject(QJsonObject& parentObject, const QString& key, const TBMap<QString, ValueType>& inMap)
{
	QJsonObject outObject;
	// I generally dislike the use of "auto", but MSVC really didn't like the whole "declare a templated type
	// within a template" thing with TBMap<>::ConstIterator.  Declaring the iterator as "auto" works around this.
	for (auto keyValPair = inMap.begin(); keyValPair != inMap.end(); keyValPair++)
	{
		outObject[keyValPair.key()] = keyValPair.value();
	}

	parentObject.insert(key, outObject);
}

template<NotStringType KeyType, typename ValueType>
void JsonableObject::MapToJsonObject(QJsonObject& parentObject, const QString& key,
	const TBMap<KeyType, ValueType>& inMap, std::function<QString(const KeyType&)> keyConverter)
{
	QJsonObject outObject;
	// I generally dislike the use of "auto", but MSVC really didn't like the whole "declare a templated type
	// within a template" thing with TBMap<>::ConstIterator.  Declaring the iterator as "auto" works around this.
	for (auto keyValPair = inMap.begin(); keyValPair != inMap.end(); keyValPair++)
	{
		outObject[keyConverter(keyValPair.key())] = keyValPair.value();
	}

	parentObject.insert(key, outObject);
}

template<typename ValueType, typename ObjectValueType>
	requires (!std::is_same_v<ValueType, ObjectValueType>)
void JsonableObject::MapToJsonObject(QJsonObject& parentObject, const QString& key,
	const TBMap<QString, ValueType>& inMap, std::function<ObjectValueType(const ValueType&)> valueConverter)
{
	QJsonObject outObject;
	// I generally dislike the use of "auto", but MSVC really didn't like the whole "declare a templated type
	// within a template" thing with TBMap<>::ConstIterator.  Declaring the iterator as "auto" works around this.
	for (auto keyValPair = inMap.begin(); keyValPair != inMap.end(); keyValPair++)
	{
		outObject[keyValPair.key()] = valueConverter(keyValPair.value());
	}

	parentObject.insert(key, outObject);
}

template<NotStringType KeyType, typename ValueType, typename ObjectValueType>
	requires (!std::is_same_v<ValueType, ObjectValueType>)
void JsonableObject::MapToJsonObject(QJsonObject& parentObject, const QString& key, const TBMap<KeyType, ValueType>& inMap,
	std::function<QString(const KeyType&)> keyConverter, std::function<ObjectValueType(const ValueType&)> valueConverter)
{
	QJsonObject outObject;
	// I generally dislike the use of "auto", but MSVC really didn't like the whole "declare a templated type
	// within a template" thing with TBMap<>::ConstIterator.  Declaring the iterator as "auto" works around this.
	for (auto keyValPair = inMap.begin(); keyValPair != inMap.end(); keyValPair++)
	{
		outObject[keyConverter(keyValPair.key())] = valueConverter(keyValPair.value());
	}

	parentObject.insert(key, outObject);
}

template<IsA<JsonableObject> ValueType>
void JsonableObject::ObjectMapToJsonObject(QJsonObject& parentObject, const QString& key, const TBMap<QString, ValueType>& inMap)
{
	QJsonObject outObject;
	// I generally dislike the use of "auto", but MSVC really didn't like the whole "declare a templated type
	// within a template" thing with TBMap<>::ConstIterator.  Declaring the iterator as "auto" works around this.
	for (auto keyValPair = inMap.begin(); keyValPair != inMap.end(); keyValPair++)
	{
		QJsonObject valObject;
		keyValPair.value().PopulateJson(valObject);
		outObject[keyValPair.key()] = valObject;
	}

	parentObject.insert(key, outObject);
}

template<NotStringType KeyType, IsA<JsonableObject> ValueType>
void JsonableObject::ObjectMapToJsonObject(QJsonObject& parentObject, const QString& key,
	const TBMap<KeyType, ValueType>& inMap, std::function<QString(const KeyType&)> keyConverter)
{
	QJsonObject outObject;
	// I generally dislike the use of "auto", but MSVC really didn't like the whole "declare a templated type
	// within a template" thing with TBMap<>::ConstIterator.  Declaring the iterator as "auto" works around this.
	for (auto keyValPair = inMap.begin(); keyValPair != inMap.end(); keyValPair++)
	{
		QJsonObject valObject;
		keyValPair.value().PopulateJson(valObject);
		outObject[keyConverter(keyValPair.key())] = valObject;
	}

	parentObject.insert(key, outObject);
}