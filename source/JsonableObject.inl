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

/*
	Boilerplate macros for use in the type aliases.
*/
// Types for references to the various typecheck and get methods of QJsonValues or other functions with "const QJsonValue&" as their sole param
typedef std::function<bool(const QJsonValue&)> IsTypeRef;
#define DECLARE_JSON_GET_REF_TYPE(type, typeSubstring) \
typedef std::function<type(const QJsonValue&)> Get##typeSubstring##Ref;

DECLARE_JSON_GET_REF_TYPE(bool, Bool)
DECLARE_JSON_GET_REF_TYPE(float64, Double)
DECLARE_JSON_GET_REF_TYPE(QString, String)
DECLARE_JSON_GET_REF_TYPE(QJsonObject, Object)
DECLARE_JSON_GET_REF_TYPE(QJsonArray, Array)
DECLARE_JSON_GET_REF_TYPE(QUuid, Uuid)
DECLARE_JSON_GET_REF_TYPE(int32, Int)
DECLARE_JSON_GET_REF_TYPE(int64, Integer)

// Overloaded methods and default parameters aren't handled especially elegantly when it comes to this templating,
// so we need to wrap the offending methods in lambdas to use in the JsonToX aliases.
#define JSON_LAMBDA_WRAPPER(returnType, getMethod) \
	[](const QJsonValue& jsonValue) -> returnType { return jsonValue.getMethod(); }

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
template<typename T, typename TypeCheckMethodRef, typename GetMethodRef>
T JsonableObject::JsonToVariable(const QJsonObject& jsonObject, const QString& key, TypeCheckMethodRef typeCheckMethod, GetMethodRef getMethod, T defaultValue)
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

template<typename T, typename TypeCheckMethodRef, typename GetMethodRef>
void JsonableObject::JsonArrayToList(const QJsonObject& jsonObject, const QString& key, QList<T>& outList, TypeCheckMethodRef typeCheckMethod, GetMethodRef getMethod)
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

template<typename ValueType, MapType<QString, ValueType> T, typename TypeCheckMethodRef, typename GetMethodRef>
	void JsonableObject::JsonObjectToMap(const QJsonObject& jsonObject, const QString& key, T& outMap,
		TypeCheckMethodRef typeCheckMethod, GetMethodRef getMethod)
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

template<typename KeyType, typename ValueType, MapType<KeyType, ValueType> T, typename TypeCheckMethodRef,
	typename GetMethodRef, typename KeyConversionMethod>
void JsonableObject::JsonObjectToMap(const QJsonObject& jsonObject, const QString& key, T& outMap,
	TypeCheckMethodRef typeCheckMethod, GetMethodRef getMethod, KeyConversionMethod keyConverter)
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

template<JsonableType T>
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

template<JsonableType ObjectType, MapType<QString, ObjectType> T>
void JsonableObject::JsonObjectToObjectMap(const QJsonObject& jsonObject, const QString& key, T& outMap)
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

template<typename KeyType, JsonableType ObjectType, MapType<KeyType, ObjectType> T, typename KeyConversionMethod>
void JsonableObject::JsonObjectToObjectMap(const QJsonObject& jsonObject, const QString& key, T& outMap,
	KeyConversionMethod keyConverter)
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
				KeyType convertedKey = keyConverter(keyValPair.key);
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
template<typename ListElemType, typename ArrayElemType, typename ConversionMethod>
void JsonableObject::ListToJsonArray(QJsonObject& parentObject, const QString& key, const QList<ListElemType>& inList, ConversionMethod converter)
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

template<JsonableType T>
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

template<typename ValueType, MapType<QString, ValueType> T>
void JsonableObject::MapToJsonObject(QJsonObject& parentObject, const QString& key, const T& inMap)
{
	QJsonObject outObject;
	for (const T::ConstIterator& keyValPair : inMap)
	{
		outObject[keyValPair.key] = keyValPair.value;
	}

	parentObject.insert(key, outObject);
}

template<typename KeyType, typename ValueType, MapType<KeyType, ValueType> T, typename ObjectKeyType, typename KeyConversionMethod>
	requires (!std::is_same_v<KeyType, ObjectKeyType>)
void JsonableObject::MapToJsonObject(QJsonObject& parentObject, const QString& key, const T& inMap, KeyConversionMethod keyConverter)
{
	QJsonObject outObject;
	for (const T::ConstIterator& keyValPair : inMap)
	{
		outObject[keyConverter(keyValPair.key)] = keyValPair.value;
	}

	parentObject.insert(key, outObject);
}

template<typename ValueType, MapType<QString, ValueType> T, typename ObjectValueType, typename ValueConversionMethod>
	requires (!std::is_same_v<ValueType, ObjectValueType>)
void JsonableObject::MapToJsonObject(QJsonObject& parentObject, const QString& key, const T& inMap, ValueConversionMethod valueConverter)
{
	QJsonObject outObject;
	for (const T::ConstIterator& keyValPair : inMap)
	{
		outObject[keyValPair.key] = valueConverter(keyValPair.value);
	}

	parentObject.insert(key, outObject);
}

template<typename KeyType, typename ValueType, MapType<KeyType, ValueType> T, typename ObjectKeyType,
	typename ObjectValueType, typename KeyConversionMethod, typename ValueConversionMethod>
	requires (!std::is_same_v<KeyType, ObjectKeyType> && !std::is_same_v<ValueType, ObjectValueType>)
void JsonableObject::MapToJsonObject(QJsonObject& parentObject, const QString& key, const T& inMap,
	KeyConversionMethod keyConverter, ValueConversionMethod valueConverter)
{
	QJsonObject outObject;
	for (const T::ConstIterator& keyValPair : inMap)
	{
		outObject[keyConverter(keyValPair.key)] = valueConverter(keyValPair.value);
	}

	parentObject.insert(key, outObject);
}

template<JsonableType ValueType, MapType<QString, ValueType> T>
void JsonableObject::ObjectMapToJsonObject(QJsonObject& parentObject, const QString& key, const T& inMap)
{
	QJsonObject outObject;
	for (const T::ConstIterator& keyValPair : inMap)
	{
		QJsonObject valObject;
		keyValPair.value.PopulateJson(valObject);
		outObject[keyValPair.key] = valObject;
	}

	parentObject.insert(key, outObject);
}

template<typename KeyType, JsonableType ValueType, MapType<KeyType, ValueType> T, typename ObjectKeyType, typename KeyConversionMethod>
	requires (!std::is_same_v<KeyType, ObjectKeyType>)
void JsonableObject::ObjectMapToJsonObject(QJsonObject& parentObject, const QString& key,
	const T& inMap, KeyConversionMethod keyConverter)
{
	QJsonObject outObject;
	for (const T::ConstIterator& keyValPair : inMap)
	{
		QJsonObject valObject;
		keyValPair.value.PopulateJson(valObject);
		outObject[keyConverter(keyValPair.key)] = valObject;
	}

	parentObject.insert(key, outObject);
}