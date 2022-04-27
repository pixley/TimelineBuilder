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

#include <QtGlobal>
#include <QtDebug>
#include <QString>

enum class ELogVerbosity : uint8
{
	Standard,
	Verbose,
	VeryVerbose
};

namespace TBLog
{
	/*
		These logging functions encode the line parameter as UTF-8 and output them through
		one of Qt's logging macros.  The log types have been renamed to match the Unreal
		Engine 4/5 pattern, because I like it better that way.
	*/
	void Log(const QString& line, ELogVerbosity verbosity = ELogVerbosity::Standard);
	void Debug(const QString& line);
	void Warning(const QString& line);
	void Error(const QString& line);
	void Fatal(const QString& line);
}