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
#include "CommonConcepts.h"

#include <QtCore/QtGlobal>
#include <QtCore/QString>

#include <utility>


// Note that in the 2-or-more arguments case of this function, we have to convert the arguments
// to QStrings, because the variadic overload of QString::arg() only accepts types that are
// implicitly convertable to QString, which does not include number types.
#define IMPLEMENT_LOG(LogFunctionName, QLogMacro) \
template<typename... Args> \
static void LogFunctionName(const char * message, Args&&... args) \
{ \
	LogFunctionName(QString(message), std::forward<Args>(args)...); \
} \
template<typename... Args> \
static void LogFunctionName(const QString& message, Args&&... args) \
{ \
	if constexpr (sizeof...(Args) == 0) \
	{ \
		QLogMacro(message.toUtf8()); \
	} \
	else if constexpr (sizeof...(Args) == 1) \
	{ \
		QLogMacro(message.arg(args...).toUtf8()); \
	} \
	else \
	{ \
		QLogMacro(message.arg(ToString<Args>(std::forward<Args>(args))...).toUtf8()); \
	} \
}

class TBLog
{
public:
	// This is a static method class only.  Never instantiate.
	TBLog() = delete;

	/*
		These logging functions encode the message parameter as UTF-8 and output them through
		one of Qt's logging macros.  The log types have been renamed to match the Unreal
		Engine 4/5 pattern, because I like it better that way.
	*/

	static void Initialize();
	static void Cleanup();

	// void Log(const QString& message, Args&&... args)
	IMPLEMENT_LOG(Log, qInfo)
	// void Debug(const QString& message, Args&&... args)
	IMPLEMENT_LOG(Debug, qDebug)
	// void Warning(const QString& message, Args&&... args)
	IMPLEMENT_LOG(Warning, qWarning)
	// void Error(const QString& message, Args&&... args)
	IMPLEMENT_LOG(Error, qCritical)
	// void Fatal(const QString& message, Args&&... args)
	IMPLEMENT_LOG(Fatal, qFatal)

private:
	// This should never be called directly.  It is instead given to qInstallMessageHandler.
	static void Handler(QtMsgType messageType, const QMessageLogContext& context, const QString& message);

	static class QFile* LogFile;

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