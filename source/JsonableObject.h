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
#include <type_traits>

#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QUuid>

#include "CommonTypes.h"
#include "CommonConcepts.h"

// Overloaded methods and default parameters aren't handled especially elegantly when it comes to this templating,
// so we need to wrap the offending methods in functions to use in the JsonToX aliases.
#define DECLARE_GET_PASSTHROUGH(returnType, passthroughName, getMethod) \
static returnType passthroughName(const QJsonValue& jsonValue) { return jsonValue.getMethod(); }

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

	// Static methods to help specific conversions from JSON
	static bool IsUuid(const QJsonValue& jsonValue);
	static QUuid GetUuid(const QJsonValue& jsonValue);
	template<typename E>
	static bool IsEnum(const QJsonValue& jsonValue);
	template<typename E>
	static E GetEnum(const QJsonValue& jsonValue);

	DECLARE_GET_PASSTHROUGH(bool, GetBool, toBool)
	DECLARE_GET_PASSTHROUGH(float64, GetFloat64, toDouble)
	DECLARE_GET_PASSTHROUGH(float32, GetFloat32, toDouble)
	DECLARE_GET_PASSTHROUGH(int32, GetInt32, toInt)
	DECLARE_GET_PASSTHROUGH(int64, GetInt64, toInteger)
	DECLARE_GET_PASSTHROUGH(QString, GetString, toString)

	// Need to wrap string-to-UUID as well
	static QUuid StringToUuid(const QString& inString) { return QUuid::fromString(inString); }
	static QString UuidToString(const QUuid& inUuid) { return inUuid.toString(QUuid::WithoutBraces); }

	// Base methods.  Generally shouldn't be used.  See the alias macros below.
	template<typename T>
	T JsonToVariable(const QJsonObject& jsonObject, const QString& key, std::function<bool(const QJsonValue&)> typeCheckMethod,
		std::function<T(const QJsonValue&)> getMethod, T defaultValue);
	
	template<typename T>
	void JsonArrayToList(const QJsonObject& jsonObject, const QString& key, QList<T>& outList,
		std::function<bool(const QJsonValue&)> typeCheckMethod, std::function<T(const QJsonValue&)> getMethod);
	
	template<typename ValueType>
	void JsonObjectToMap(const QJsonObject& jsonObject, const QString& key, TBMap<QString, ValueType>& outMap,
		std::function<bool(const QJsonValue&)> typeCheckMethod, std::function<ValueType(const QJsonValue&)> getMethod);
	
	template<NotStringType KeyType, typename ValueType>
	void JsonObjectToMap(const QJsonObject& jsonObject, const QString& key, TBMap<KeyType, ValueType>& outMap,
		std::function<bool(const QJsonValue&)> typeCheckMethod, std::function<ValueType(const QJsonValue&)> getMethod,
		std::function<KeyType(const QString&)> keyConverter);

	// Specific implementations for objects, which don't fit the standard scheme.
	void JsonToObject(const QJsonObject& jsonObject, const QString& key, JsonableObject& outObject);
	
	template<IsA<JsonableObject> T>
	void JsonArrayToObjectList(const QJsonObject& jsonObject, const QString& key, QList<T>& outList);
	
	template<IsA<JsonableObject> ObjectType>
	void JsonObjectToObjectMap(const QJsonObject& jsonObject, const QString& key, TBMap<QString, ObjectType>& outMap);
	
	template<NotStringType KeyType, IsA<JsonableObject> ObjectType>
	void JsonObjectToObjectMap(const QJsonObject& jsonObject, const QString& key,
		TBMap<KeyType, ObjectType>& outMap, std::function<KeyType(const QString&)> keyConverter);

	// Static mentods to help specific conversions to JSON
	static void ObjectToJson(QJsonObject& parentObject, const QString& key, const JsonableObject& object);
	
	static QString UuidToJson(const QUuid& uuid);
	
	template<typename E>
	static int64 EnumToJson(E enumValue);

	// Static methods to convert QLists to QJsonArrays
	// For types that have direct conversions to QJsonValue, use the first overload of ListToJsonArray()
	template<typename ElemType>
	static void ListToJsonArray(QJsonObject& parentObject, const QString& key, const QList<ElemType>& inList);
	
	template<typename ListElemType, typename ArrayElemType>
	static void ListToJsonArray(QJsonObject& parentObject, const QString& key, const QList<ListElemType>& inList,
		std::function<ArrayElemType(const ListElemType&)> converter);
	
	template<IsA<JsonableObject> T>
	static void ObjectListToJsonArray(QJsonObject& parentObject, const QString& key, const QList<T>& inList);

	// Static methods to convert QMaps/QHashes to QJsonObjects
	template<typename ValueType>
	static void MapToJsonObject(QJsonObject& parentObject, const QString& key, const TBMap<QString, ValueType>& inMap);

	template<NotStringType KeyType, typename ValueType>
	static void MapToJsonObject(QJsonObject& parentObject, const QString& key,
		const TBMap<KeyType, ValueType>& inMap, std::function<QString(const KeyType&)> keyConverter);

	template<typename ValueType, typename ObjectValueType>
		requires (!std::is_same_v<ValueType, ObjectValueType>)
	static void MapToJsonObject(QJsonObject& parentObject, const QString& key,
		const TBMap<QString, ValueType>& inMap, std::function<ObjectValueType(const ValueType&)> valueConverter);
	
	template<NotStringType KeyType, typename ValueType, typename ObjectValueType>
		requires (!std::is_same_v<ValueType, ObjectValueType>)
	static void MapToJsonObject(QJsonObject& parentObject, const QString& key, const TBMap<KeyType, ValueType>& inMap,
		std::function<QString(const KeyType&)> keyConverter, std::function<ObjectValueType(const ValueType&)> valueConverter);

	template<IsA<JsonableObject> ValueType>
	static void ObjectMapToJsonObject(QJsonObject& parentObject, const QString& key, const TBMap<QString, ValueType>& inMap);

	template<NotStringType KeyType, IsA<JsonableObject> ValueType>
	static void ObjectMapToJsonObject(QJsonObject& parentObject, const QString& key,
		const TBMap<KeyType, ValueType>& inMap, std::function<QString(const KeyType&)> keyConverter);
};

// Aliases for different return types for JsonableObject::JsonToVariable()
// Why are we using lambdas in some places?  Because we apparently can't resolve which version of overloaded functions like
// QJsonValue::toString we want even though only one of the overloads matches the signature that the parameter type encodes.  Ugh.
#define JsonToBool(jsonObject, key) \
	JsonToVariable<bool>(jsonObject, key, &QJsonValue::isBool, &JsonableObject::GetBool, false)
#define JsonToFloat64(jsonObject, key) \
	JsonToVariable<float64>(jsonObject, key, &QJsonValue::isDouble, &JsonableObject::GetFloat64, 0.0)
#define JsonToFloat32(jsonObject, key) static_cast<float32>(JsonToFloat64(jsonObject, variable, key))
#define JsonToInt32(jsonObject, key) \
	JsonToVariable<int32>(jsonObject, key, &QJsonValue::isDouble, &JsonableObject::GetInt32, 0)
#define JsonToInt64(jsonObject, key) \
	JsonToVariable<int64>(jsonObject, key, &QJsonValue::isDouble, &JsonableObject::GetInt64, 0)
#define JsonToString(jsonObject, key) \
	JsonToVariable<QString>(jsonObject, key, &QJsonValue::isString, &JsonableObject::GetString, "")
#define JsonToUuid(jsonObject, key) \
	JsonToVariable<QUuid>(jsonObject, key, &JsonableObject::IsUuid, &JsonableObject::GetUuid, QUuid())
// JsonToEnum() only works with enum classes declared with the ENUM_CLASS() macro in CommonTypes.h
#define JsonToEnum(jsonObject, key, enumType) \
	JsonToVariable<enumType>(jsonObject, key, &JsonableObject::IsEnum<enumType>, &JsonableObject::GetEnum<enumType>, enumType::_INVALIDVALUE_)
// Note: JsonToObject() is implemented as a separate method in JsonableObject, rather than being an alias.

// Aliases for different QList types for JsonableObject::JsonArrayToList()
#define JsonArrayToBoolList(jsonObject, key, outList) \
	JsonArrayToList<bool>(jsonObject, key, outList, &QJsonValue::isBool, &JsonableObject::GetBool)
#define JsonArrayToFloat64List(jsonObject, key, outList) \
	JsonArrayToList<float64>(jsonObject, key, outList, &QJsonValue::isDouble, &JsonableObject::GetFloat64)
#define JsonArrayToFloat32List(jsonObject, key, outList) \
	JsonArrayToList<float32>(jsonObject, key, outList, &QJsonValue::isDouble, &JsonableObject::GetFloat32)
#define JsonArrayToInt32List(jsonObject, key, outList) \
	JsonArrayToList<int32>(jsonObject, key, outList, &QJsonValue::isDouble, &JsonableObject::GetInt32))
#define JsonArrayToInt64List(jsonObject, key, outList) \
	JsonArrayToList<int64>(jsonObject, key, outList, &QJsonValue::isDouble, &JsonableObject::GetInt64)
#define JsonArrayToStringList(jsonObject, key, outList) \
	JsonArrayToList<QString>(jsonObject, key, outList, &QJsonValue::isString,  &JsonableObject::GetString)
#define JsonArrayToUuidList(jsonObject, key, outList) \
	JsonArrayToList<QUuid>(jsonObject, key, outList, &JsonableObject::IsUuid, &JsonableObject::GetUuid)
#define JsonArrayToEnumList(jsonObject, key, outList, enumType) \
	JsonArrayToList<enumType>(jsonObject, key, outList, &JsonableObject::IsEnum<enumType>, &JsonableObject::GetEnum<enumType>)

// Aliases for JsonableObject::ListToJsonArray for types that need conversion.
#define UuidListToJsonArray(jsonObject, key, inList) \
	ListToJsonArray<QUuid, QString>(jsonObject, key, inList, &JsonableObject::UuidToJson)
#define EnumListToJsonArray(jsonObject, key, inList, enumType) \
	ListToJsonArray<enumType, int64>(jsonObject, key, inList, &JsonableObject::EnumToJson<enumType>)

// Note: maps are specific enough that alias macros are not provided here.

// For the sake of readability, the implementations for JsonableObject's templated methods
// and other boilerplate are in this file
#include "JsonableObject.inl"