/*
	Copyright (c) 2022 Tyler Pixley, all rights reserved.

	This file (Logging.cpp) is part of TimelineBuilder.

	TimelineBuilder is free software: you can redistribute it and/or modify it under
	the terms of the GNU General Public License as published by the Free Software
	Foundation, either version 3 of the License, or (at your option) any later version.

	TimelineBuilder is distributed in the hope that it will be useful, but WITHOUT ANY
	WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
	PARTICULAR PURPOSE. See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along with
	TimelineBuilder. If not, see <https://www.gnu.org/licenses/>.
*/

#include "Logging.h"

namespace TBLog
{
	void Log(const QString& line, ELogVerbosity verbosity)
	{

	}

	void Debug(const QString& line)
	{
		qDebug() << line.toUtf8();
	}

	void Warning(const QString& line)
	{
		qWarning() << line.toUtf8();
	}

	void Error(const QString& line)
	{
		qCritical() << line.toUtf8();
	}

	void Fatal(const QString& line)
	{
		qFatal(line.toUtf8());
	}
}