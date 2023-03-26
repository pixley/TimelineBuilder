/*
	Copyright (c) 2023 Tyler Pixley, all rights reserved.

	This file (UserFiles.h) is part of TimelineBuilder.

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

#include <QtCore/QDir>
#include <QtCore/QCoreApplication>

class TBUserFiles
{
public:
	// This is a static method class only.  Never instantiate.
	TBUserFiles() = delete;

	static void Initialize(const QCoreApplication& app);
	static void Cleanup();

	static QDir GetBasePath();

private:
	static QDir* CachedBasePath;
};