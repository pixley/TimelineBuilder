/*
	Copyright (c) 2022 Tyler Pixley, all rights reserved.

	This file (Logging.h) is part of TimelineBuilder.

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

#include <QtCore/QtGlobal>
#include <QtCore/QString>

#include <utility>
#include <type_traits>


// Note that in the 2-or-more arguments case of this function, we have to convert the arguments
// to QStrings, because the variadic overload of QString::arg() only accepts types that are
// implicitly convertable to QString, which does not include number types.
#define IMPLEMENT_LOG(LogFunctionName, QLogMacro) \
template<typename... Args> \
static void LogFunctionName(const char * line, Args&&... args) \
{ \
	LogFunctionName(QString(line), std::forward<Args>(args)...); \
} \
template<typename... Args> \
static void LogFunctionName(const QString& line, Args&&... args) \
{ \
	if constexpr (sizeof...(Args) == 0) \
	{ \
		QLogMacro(line.toUtf8()); \
	} \
	else if constexpr (sizeof...(Args) == 1) \
	{ \
		QLogMacro(line.arg(args...).toUtf8()); \
	} \
	else \
	{ \
		QLogMacro(line.arg(ToString<Args>(std::forward<Args>(args))...).toUtf8()); \
	} \
}

// Thank you, C++20, for making the template specialization not frustrating
template<typename T>
concept NumberType = std::is_arithmetic_v<std::remove_reference_t<T>>;

template<typename T>
concept StringType = std::is_same_v<std::remove_reference_t<std::remove_cv_t<T>>, QString>;

class TBLog
{
public:
	/*
		These logging functions encode the line parameter as UTF-8 and output them through
		one of Qt's logging macros.  The log types have been renamed to match the Unreal
		Engine 4/5 pattern, because I like it better that way.
	*/

	// void Log(const QString& line, Args&&... args)
	IMPLEMENT_LOG(Log, qInfo)
	// void Debug(const QString& line, Args&&... args)
	IMPLEMENT_LOG(Debug, qDebug)
	// void Warning(const QString& line, Args&&... args)
	IMPLEMENT_LOG(Warning, qWarning)
	// void Error(const QString& line, Args&&... args)
	IMPLEMENT_LOG(Error, qCritical)
	// void Fatal(const QString& line, Args&&... args)
	IMPLEMENT_LOG(Fatal, qFatal)

private:
	template<typename T>
	static QString ToString(const T& input)
	{
		return QString(input);
	}

	template<StringType T>
	static QString ToString(const T& input)
	{
		return input;
	}

	template<NumberType T>
	static QString ToString(const T input)
	{
		return QString::number(input);
	}
};