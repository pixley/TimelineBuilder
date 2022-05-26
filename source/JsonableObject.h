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
#include <utility>

#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QHash>
#include <QtCore/QString>
#include <QtCore/QUuid>
#include <QtCore/QtGlobal>
#include <QtCore/QtDebug>

#include "CommonTypes.h"
#include "CommonConcepts.h"
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
	template<typename KeyType, typename ValueType, MapType<KeyType, ValueType> T, typename TypeCheckMethodRef,
		typename GetMethodRef, typename KeyConversionMethod = std::function<KeyType(const QString&)>>
	void JsonObjectToMap(const QJsonObject& jsonObject, const QString& key, T& outMap, TypeCheckMethodRef typeCheckMethod,
		GetMethodRef getMethod, KeyConversionMethod keyConverter);

	// Specific implementations for objects, which don't fit the standard scheme.
	void JsonToObject(const QJsonObject& jsonObject, const QString& key, JsonableObject& outObject);
	template<JsonableType T>
	void JsonArrayToObjectList(const QJsonObject& jsonObject, const QString& key, QList<T>& outList);
	template<typename KeyType, JsonableType ObjectType, MapType<KeyType, ObjectType> T,
		typename KeyConversionMethod = std::function<KeyType(const QString&)>>
	void JsonObjectToObjectMap(const QJsonObject& jsonObject, const QString& key, T& outMap, KeyConversionMethod keyConverter);

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
	// For types that have direct conversions to QJsonValue, use the first overload of ListToJsonArray()
	template<typename ElemType>
	static void ListToJsonArray(QJsonObject& parentObject, const QString& key, const QList<ElemType>& inList);
	template<typename ListElemType, typename ArrayElemType, typename ConversionMethod = std::function<ArrayElemType(const ListElemType&)>>
	static void ListToJsonArray(QJsonObject& parentObject, const QString& key, const QList<ListElemType>& inList, ConversionMethod converter);
	template<JsonableType T>
	static void ObjectListToJsonArray(QJsonObject& parentObject, const QString& key, const QList<T>& inList);

	// Static methods to convert QMaps/QHashes to QJsonObjects
	template<typename KeyType, typename ValueType, MapType<KeyType, ValueType> T>
	static void MapToJsonObject(QJsonObject& parentObject, const QString& key, const T& inMap);

	template<typename KeyType, typename ValueType, MapType<KeyType, ValueType> T, typename ObjectKeyType,
		typename KeyConversionMethod = std::function<ObjectKeyType(const KeyType&)>>
		requires (!std::is_same_v<KeyType, ObjectKeyType>)
	static void MapToJsonObject(QJsonObject& parentObject, const QString& key, const T& inMap, KeyConversionMethod keyConverter);

	template<typename KeyType, typename ValueType, MapType<KeyType, ValueType> T, typename ObjectValueType,
		typename ValueConversionMethod = std::function<ObjectValueType(const ValueType&)>>
		requires (!std::is_same_v<ValueType, ObjectValueType>)
	static void MapToJsonObject(QJsonObject& parentObject, const QString& key, const T& inMap, ValueConversionMethod valueConverter);
	
	template<typename KeyType, typename ValueType, MapType<KeyType, ValueType> T,
		typename ObjectKeyType, typename ObjectValueType,
		typename KeyConversionMethod = std::function<ObjectKeyType(const KeyType&)>,
		typename ValueConversionMethod = std::function<ObjectValueType(const ValueType&)>>
		requires (!std::is_same_v<KeyType, ObjectKeyType> && !std::is_same_v<ValueType, ObjectValueType>)
	static void MapToJsonObject(QJsonObject& parentObject, const QString& key, const T& inMap,
		KeyConversionMethod keyConverter, ValueConversionMethod valueConverter);

	template<typename KeyType, JsonableType ValueType, MapType<KeyType, ValueType> T>
	static void ObjectMapToJsonObject(QJsonObject& parentObject, const QString& key, const T& inMap);

	template<typename KeyType, JsonableType ValueType, MapType<KeyType, ValueType> T, typename ObjectKeyType,
		typename KeyConversionMethod = std::function<ObjectKeyType(const KeyType&)>>
		requires (!std::is_same_v<KeyType, ObjectKeyType>)
	static void ObjectMapToJsonObject(QJsonObject& parentObject, const QString& key, const T& inMap, KeyConversionMethod keyConverter);
};

// For the sake of readability, the implementations for JsonableObject's templated methods
// and other boilerplate are in this file
#include "JsonableObject.inl"

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

// Note: maps are specific enough that alias macros are not provided here.

// Aliases for JsonableObject::ListToJsonArray for types that need conversion.
#define UuidListToJsonArray(jsonObject, key, inList) \
	ListToJsonArray<QUuid, QString, std::function<QString(QUuid)> >(jsonObject, key, inList, &JsonableObject::UuidToJson)
#define EnumListToJsonArray(jsonObject, key, inList, enumType) \
	ListToJsonArray<enumType, int64, std::function<int64(enumType)> >(jsonObject, key, inList, &JsonableObject::EnumToJson<enumType>)