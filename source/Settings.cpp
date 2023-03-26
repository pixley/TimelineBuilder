/*
	Copyright (c) 2022 Tyler Pixley, all rights reserved.

	This file (Settings.cpp) is part of TimelineBuilder.

	TimelineBuilder is free software: you can redistribute it and/or modify it under
	the terms of the GNU General Public License as published by the Free Software
	Foundation, either version 3 of the License, or (at your option) any later version.

	TimelineBuilder is distributed in the hope that it will be useful, but WITHOUT ANY
	WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
	PARTICULAR PURPOSE. See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along with
	TimelineBuilder. If not, see <https://www.gnu.org/licenses/>.
*/

#include <QtCore/QSettings>
#include <QtCore/QDir>

#include <stdexcept>

#include "Settings.h"
#include "Logging.h"
#include "UserFiles.h"

QSettings* InitializeIniFile(const QString& FileName, const QDir& FileDir)
{
	return new QSettings(FileDir.filePath(FileName), QSettings::IniFormat);
}

#define INIT_INI_FILE(SettingsType, FileDir) \
SettingsFiles.insert(TBSettingsFile::SettingsType, InitializeIniFile("SettingsType##.ini", FileDir))
// The ":/config" works off Qt's resource system, where a leading colon says to use an in-built resource.
// Default INI files are packaged as resources.
#define INIT_DEFAULT_INI_FILE(SettingsType) \
DefaultFiles.insert(TBSettingsFile::SettingsType, InitializeIniFile("Default##SettingsType##.ini", QDir(":/config")))

TBSettings* TBSettings::singleton = nullptr;

TBSettings::TBSettings() :
	DefaultFiles(),
	SettingsFiles()
{
	// Establish the path to the config folder
	QDir configPath = TBUserFiles::GetBasePath();
	configPath.cd("config");

	// This needs to be done for each value in TBSettingsFile
	INIT_DEFAULT_INI_FILE(System);
	INIT_DEFAULT_INI_FILE(Gui);
	INIT_DEFAULT_INI_FILE(Platform);

	INIT_INI_FILE(System, configPath);
	INIT_INI_FILE(Gui, configPath);
	INIT_INI_FILE(Platform, configPath);
}

TBSettings::~TBSettings()
{
	// Clean up the QSettings objects we were working with
	for (const QSettings* defaultIni : DefaultFiles)
	{
		delete defaultIni;
	}

	for (QSettings* ini : SettingsFiles)
	{
		delete ini;
	}
}

void TBSettings::Initialize()
{
	if (singleton != nullptr)
	{
		throw std::runtime_error("TBSettings singleton already initialized!");
	}
	else
	{
		singleton = new TBSettings();
	}
}

void TBSettings::Cleanup()
{
	if (singleton != nullptr)
	{
		// Make sure all config files are up-to-date with changes made this session.
		singleton->SyncAllFiles();

		delete singleton;
		singleton = nullptr;
	}
}

void TBSettings::SyncAllFiles()
{
	for (QSettings* ini : SettingsFiles)
	{
		ini->sync();
	}
	// We don't need to do this for the default INIs because they don't get written to.
}

QVariant TBSettings::GetValue(TBSettingsFile file, const QString& section, const QString& key) const
{
	assert(SettingsFiles.contains(file) && DefaultFiles.contains(file));

	// QSettings handles sections by way of delimiting with a forward slash
	QString fullKey = QString("%0/%1").arg(section, key);
	const QSettings* ini = SettingsFiles[file];

	if (ini->contains(fullKey))
	{
		return ini->value(fullKey);
	}
	else
	{
		const QSettings* defaultIni = DefaultFiles[file];
		if (defaultIni->contains(fullKey))
		{
			return defaultIni->value(fullKey);
		}
		else
		{
			TBLog::Error("Could not find config value in section [%0] and key [%1]!", section, key);
			return QVariant();
		}
	}
}

void TBSettings::SetValue(TBSettingsFile file, const QString& section, const QString& key, const QVariant& value)
{
	assert(SettingsFiles.contains(file));

	// QSettings handles sections by way of delimiting with a forward slash
	QString fullKey = QString("%0/%1").arg(section, key);
	QSettings* ini = SettingsFiles[file];
	ini->setValue(fullKey, value);
}