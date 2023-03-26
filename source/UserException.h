/*
	Copyright (c) 2022 Tyler Pixley, all rights reserved.

	This file (UserException.h) is part of TimelineBuilder.

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

#include <stdexcept>

#include <QtCore/QString>

class TBUserException : public std::runtime_error
{
public:
	TBUserException(const QString& inWhat, const QString& inDisplay);
	TBUserException(const char* inWhat, const char* inDisplay);
	virtual ~TBUserException();

	// Alias because I prefer capitalized method names and you can't stop me.
	const char* What() const noexcept { return what(); }
	const char* UserString() const noexcept { return display; }

private:
	// Holds the string for the error pop-up to be presented to the user.
	const char* display;
};