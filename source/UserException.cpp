/*
	Copyright (c) 2022 Tyler Pixley, all rights reserved.

	This file (UserException.cpp) is part of TimelineBuilder.

	TimelineBuilder is free software: you can redistribute it and/or modify it under
	the terms of the GNU General Public License as published by the Free Software
	Foundation, either version 3 of the License, or (at your option) any later version.

	TimelineBuilder is distributed in the hope that it will be useful, but WITHOUT ANY
	WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
	PARTICULAR PURPOSE. See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along with
	TimelineBuilder. If not, see <https://www.gnu.org/licenses/>.
*/

#include "UserException.h"

#include <QtCore/QByteArray>

TBUserException::TBUserException(const QString& inWhat, const QString& inDisplay) :
	TBUserException(inWhat.toUtf8().constData(), inDisplay.toUtf8().constData())
{}

TBUserException::TBUserException(const char* inWhat, const char* inDisplay) :
	std::runtime_error(inWhat), display(nullptr)
{
	if (inDisplay != nullptr)
	{

	}
}

TBUserException::~TBUserException()
{
	if (display != nullptr)
	{
		delete display;
	}
}