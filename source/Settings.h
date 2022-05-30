/*
	Copyright (c) 2022 Tyler Pixley, all rights reserved.

	This file (Settings.h) is part of TimelineBuilder.

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

#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtCore/QMap>
#include <QtCore/QCommandLineParser>
#include <QtCore/QCommandLineOption>
#include <QtCore/QCoreApplication>

#include "CommonTypes.h"
#include "CommonConcepts.h"

// These names should match the INI file names in the user config
// Which should also match the default config files (minus "Default")
// Defaults for these need to be populated in TimelineBuilder.qrc.
enum class TBSettingsFile
{
	System,
	Gui,
	Platform
};

class TBSettings
{
private:
	static TBSettings* singleton;

	// Restrict these to private to enforce singleton-ness
	TBSettings() = delete;
	TBSettings(const QCoreApplication& app);	// Only allow construction with the app
	TBSettings(const TBSettings& other) = delete;
	TBSettings& operator=(const TBSettings& other) = delete;
	~TBSettings();

public:
	// Static initialization and access
	static void Initialize(const QCoreApplication& app);
	static TBSettings& Get() { return *singleton; }
	static void Cleanup();

	void SyncAllFiles();

	QVariant GetValue(TBSettingsFile file, const QString& section, const QString& key) const;
	void SetValue(TBSettingsFile file, const QString& section, const QString& key, const QVariant& value);

	template<VariantCompatible T>
	T GetValue(TBSettingsFile file, const QString& section, const QString& key) const;
	template<VariantCompatible T>
	void SetValue(TBSettingsFile file, const QString& section, const QString& key, const T& value);

private:
	QCommandLineParser Parser;
	QCommandLineOption DevConfigPath;

	// We are specifically using QMap here, rather than the TBMap alias, because this isn't large enough to warrant
	// a hash-map.
	// DefaultFiles deliberately has its value type const.  Default INIs shall only ever be manually edited on the dev side,
	// never inside the TimelineBuilder app.
	QMap<TBSettingsFile, const class QSettings*> DefaultFiles;
	QMap<TBSettingsFile, class QSettings*> SettingsFiles;
};

template<VariantCompatible T>
T TBSettings::GetValue(TBSettingsFile file, const QString& section, const QString& key) const
{
	return GetValue(file, section, key).value<T>();
}

template<VariantCompatible T>
void TBSettings::SetValue(TBSettingsFile file, const QString& section, const QString& key, const T& value)
{
	SetValue(file, section, key, QVariant(value));
}