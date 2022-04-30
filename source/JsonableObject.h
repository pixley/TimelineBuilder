/*
	Copyright (c) 2022 Tyler Pixley, all rights reserved.

	This file (JsonableObject.h) is part of TimelineBuilder.

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

#include <functional>

#include <QJsonObject>
#include <QJsonArray>
#include <QList>
#include <QString>
#include <QUuid>
#include <QtGlobal>
#include <QtDebug>
#include "CommonTypes.h"
#include "Logging.h"

// Virtual base class
class JsonableObject
{
public:
	JsonableObject();

	// When overriding LoadFromJson, always return LoadSuccessful.
	virtual bool LoadFromJson(const QJsonObject& jsonObject);
	virtual void PopulateJson(QJsonObject& jsonObject) const = 0;

	bool IsValid() const { return LoadSuccessful; }

protected:
	bool LoadSuccessful;

	// Base methods.  Generally shouldn't be used.  See the alias macros below.
	template<typename T, typename TypeCheckMethodRef, typename GetMethodRef>
	T JsonToVariable(const QJsonObject& jsonObject, const QString& key, TypeCheckMethodRef typeCheckMethod, GetMethodRef getMethod, T defaultValue);
	template<typename T, typename TypeCheckMethodRef, typename GetMethodRef>
	void JsonArrayToList(const QJsonObject& jsonObject, const QString& key, QList<T>& outList, TypeCheckMethodRef typeCheckMethod, GetMethodRef getMethod);

	// Specific implementations for objects, which don't fit the standard scheme.
	void JsonToObject(const QJsonObject& jsonObject, const QString& key, JsonableObject& outObject);
	template<typename T>
	void JsonArrayToObjectList(const QJsonObject& jsonObject, const QString& key, QList<T>& outList);

	// Static methods to help specific conversions from JSON
	static bool IsUuid(const QJsonValue& jsonValue);
	static QUuid GetUuid(const QJsonValue& jsonValue);
	template<typename E>
	static bool IsEnum(const QJsonValue& jsonValue);
	template<typename E>
	static E GetEnum(const QJsonValue& jsonValue);

	// Static mentods to help specific conversions to JSON
	static void ObjectToJson(QJsonObject& parentObject, const QString& key, const JsonableObject& object);
	static QString UuidToJson(const QUuid& uuid);
	template<typename E>
	static int64 EnumToJson(E enumValue);

	// Static methods to convert QLists to QJsonArrays
	// For types that have direct conversions to QJsonValue, only the ListElemType needs to be specified
	template<typename ListElemType, typename ArrayElemType = ListElemType, typename ConversionMethod = std::function<void> >
	static void ListToJsonArray(QJsonObject& parentObject, const QString& key, const QList<ListElemType>& inList, ConversionMethod converter = nullptr);
	template<typename T>
	static void ObjectListToJsonArray(QJsonObject& parentObject, const QString& key, const QList<T>& inList);
};

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

// Aliases for different return types for JsonableObject::JsonToVariable()
// Why are we using lambdas in some places?  Because we apparently can't resolve which version of overloaded functions like
// QJsonValue::toString we want even though only one of the overloads matches the signature that the parameter type encodes.  Ugh.
#define JsonToBool(jsonObject, key) \
	JsonToVariable<bool, IsTypeRef, GetBoolRef>(jsonObject, key, &QJsonValue::isBool, JSON_LAMBDA_WRAPPER(bool, toBool), false)
#define JsonToFloat64(jsonObject, key) \
	JsonToVariable<float64, IsTypeRef, GetDoubleRef>(jsonObject, key, &QJsonValue::isDouble, JSON_LAMBDA_WRAPPER(float64, toDouble), 0.0)
#define JsonToFloat32(jsonObject, key) static_cast<float32>(JsonToFloat64(jsonObject, variable, key))
#define JsonToInt32(jsonObject, key) \
	JsonToVariable<int32, IsTypeRef, GetIntRef>(jsonObject, key, &QJsonValue::isDouble, JSON_LAMBDA_WRAPPER(int32, toInt), 0)
#define JsonToInt64(jsonObject, key) \
	JsonToVariable<int64, IsTypeRef, GetIntegerRef>(jsonObject, key, &QJsonValue::isDouble, JSON_LAMBDA_WRAPPER(int64, toInteger), 0)
#define JsonToString(jsonObject, key) \
	JsonToVariable<QString, IsTypeRef, GetStringRef>(jsonObject, key, &QJsonValue::isString, JSON_LAMBDA_WRAPPER(QString, toString), "")
#define JsonToUuid(jsonObject, key) \
	JsonToVariable<QUuid, IsTypeRef, GetUuidRef>(jsonObject, key, &JsonableObject::IsUuid, &JsonableObject::GetUuid, QUuid())
// JsonToEnum() only works with enum classes declared with the ENUM_CLASS() macro in CommonTypes.h
#define JsonToEnum(jsonObject, key, enumType) \
	JsonToVariable<enumType, IsTypeRef, std::function<enumType(const QJsonValue&)> >(jsonObject, key, &JsonableObject::IsEnum<enumType>, &JsonableObject::GetEnum<enumType>, enumType::_INVALIDVALUE_)
// Note: JsonToObject() is implemented as a separate method in JsonableObject, rather than being an alias.

// Aliases for different QList types for JsonableObject::JsonArrayToList()
#define JsonArrayToBoolList(jsonObject, key, outList) \
	JsonArrayToList<bool, IsTypeRef, GetBoolRef>(jsonObject, key, outList, &QJsonValue::isBool, JSON_LAMBDA_WRAPPER(bool, toBool))
#define JsonArrayToFloat64List(jsonObject, key, outList) \
	JsonArrayToList<float64, IsTypeRef, GetDoubleRef>(jsonObject, key, outList, &QJsonValue::isDouble, JSON_LAMBDA_WRAPPER(float64, toDouble))
#define JsonArrayToFloat32List(jsonObject, key, outList) \
	JsonArrayToList<float32, IsTypeRef, GetDoubleRef>(jsonObject, key, outList, &QJsonValue::isDouble, JSON_LAMBDA_WRAPPER(float32, toDouble))
#define JsonArrayToInt32List(jsonObject, key, outList) \
	JsonArrayToList<int32, IsTypeRef, GetIntRef>(jsonObject, key, outList, &QJsonValue::isDouble, JSON_LAMBDA_WRAPPER(int32, toInt))
#define JsonArrayToInt64List(jsonObject, key, outList) \
	JsonArrayToList<int64, IsTypeRef, GetIntegerRef>(jsonObject, key, outList, &QJsonValue::isDouble, JSON_LAMBDA_WRAPPER(int64, toInteger))
#define JsonArrayToStringList(jsonObject, key, outList) \
	JsonArrayToList<QString, IsTypeRef, GetStringRef>(jsonObject, key, outList, &QJsonValue::isString, JSON_LAMBDA_WRAPPER(QString, toString))
#define JsonArrayToUuidList(jsonObject, key, outList) \
	JsonArrayToList<QUuid, IsTypeRef, GetUuidRef>(jsonObject, key, outList, &JsonableObject::IsUuid, &JsonableObject::GetUuid)
#define JsonArrayToEnumList(jsonObject, key, outList, enumType) \
	JsonArrayToList<enumType, IsTypeRef, std::function<enumType(const QJsonValue&)> >(jsonObject, key, outList, &JsonableObject::IsEnum<enumType>, &JsonableObject::GetEnum<enumType>)

template<typename T>
void JsonableObject::JsonArrayToObjectList(const QJsonObject& jsonObject, const QString& key, QList<T>& outList)
{
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
				outList.emplaceBack(arrayElem.toObject());
				if (!outList.back().IsValid())
				{
					TBLog::Warning("Error loading object within array for key '%0'.", key);
					LoadSuccessful = false;
					break;
				}
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

// For use with types that have direct QJsonValue conversions or have custom conversions as listed in the macros below.
template<typename ListElemType, typename ArrayElemType, typename ConversionMethod>
void JsonableObject::ListToJsonArray(QJsonObject& parentObject, const QString& key, const QList<ListElemType>& inList, ConversionMethod converter)
{
	// QJsonArray does not have a reserve/preallocate method.  Otherwise, I would put one here.
	QJsonArray outArray;
	for (const ListElemType& element : inList)
	{
		ArrayElemType convertedElement;
		if (converter == nullptr)
		{
			convertedElement = converter(element);
		}
		else
		{
			convertedElement = element;
		}

		outArray.push_back(convertedElement);
	}

	parentObject.insert(key, outArray);
}

// Aliases for JsonableObject::ListToJsonArray for types that need conversion.
#define UuidListToJsonArray(inList, outArray) \
	ListToJsonArray<QUuid, QString, std::function<QString(QUuid)> >(inList, outArray, &JsonableObject::UuidToJson)
#define EnumListToJsonArray(inList, outArray, enumType) \
	ListToJsonArray<enumType, int64, std::function<int64(enumType)> >(inList, outArray, &JsonableObject::EnumToJson<enumType>)

// Use only with vectors of pointers to JsonableObject-inheriting objects.
template<typename T>
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