/*
	Copyright (c) 2023 Tyler Pixley, all rights reserved.

	This file (UserFiles.cpp) is part of TimelineBuilder.

	TimelineBuilder is free software: you can redistribute it and/or modify it under
	the terms of the GNU General Public License as published by the Free Software
	Foundation, either version 3 of the License, or (at your option) any later version.

	TimelineBuilder is distributed in the hope that it will be useful, but WITHOUT ANY
	WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
	PARTICULAR PURPOSE. See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along with
	TimelineBuilder. If not, see <https://www.gnu.org/licenses/>.
*/

#include <QtCore/QCommandLineParser>
#include <QtCore/QCommandLineOption>
#include "UserFiles.h"

QDir* TBUserFiles::CachedBasePath = nullptr;

void TBUserFiles::Initialize(const QCoreApplication& app)
{
	QCommandLineOption devPathOption("devuserfilepath", "If this option is present, write user files to the project directory rather than AppData (or the equivalent for your platform).");
	QCommandLineParser parser;
	parser.addOption(devPathOption);
	parser.process(app);

	CachedBasePath = new QDir();

	// Initialize the new cached path.
	if (parser.isSet(devPathOption))
	{
		// In development mode, user files are in the "saved" folder in the project directory
		*CachedBasePath = QDir::current();
		if (!CachedBasePath->exists("saved"))
		{
			CachedBasePath->mkdir("saved");
		}
		CachedBasePath->cd("saved");
	}
	else
	{
		// Start from the user's home directory and navigate to the appropriate place to store app user files
		*CachedBasePath = QDir::home();

#ifdef Q_OS_WINDOWS
		// Navigate to the TimelineBuilder folder in %APPDATALOCAL%
		CachedBasePath->cd("AppData/Local");

		if (!CachedBasePath->exists("TimelineBuilder"))
		{
			CachedBasePath->mkdir("TimelineBuilder");
		}
		CachedBasePath->cd("TimelineBuilder");
#else
		static_assert(false, "Current platform does not have logic for user files!");
#endif
	}
}

void TBUserFiles::Cleanup()
{
	if (CachedBasePath != nullptr)
	{
		delete CachedBasePath;
		CachedBasePath = nullptr;
	}
}

QDir TBUserFiles::GetBasePath()
{
	assert(CachedBasePath != nullptr);

	return *CachedBasePath;
}