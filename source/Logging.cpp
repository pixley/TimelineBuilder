/*
	Copyright (c) 2023 Tyler Pixley, all rights reserved.

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

#include <iostream>
#include <Qt>
#include <QApplication>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QDateTime>
#include <QtCore/QFileInfo>
#include "Logging.h"
#include "UserFiles.h"
#include "UserException.h"

#ifdef Q_OS_UNIX
#include <unistd.h>
#endif //Q_OS_UNIX

#ifdef Q_OS_WINDOWS
extern Q_CORE_EXPORT int qt_ntfs_permission_lookup;
#endif //Q_OS_WINDOWS

QFile* TBLog::LogFile = nullptr;

// Get the default Qt message handler.  Fun hack I found on StackOverflow.
static const QtMessageHandler DefaultMessageHandlerFunction = qInstallMessageHandler(0);

void TBLog::Initialize()
{
	// Set custom log message formatting and install our custom handler.
	// See <https://doc.qt.io/qt-6/qtglobal.html#qSetMessagePattern> for documentation on how this formatting works.
	// The resulting log strings have the format "[#time in ISO-8601#][#type#] #message#"
	qSetMessagePattern("[%{time}][%{if-info}Log%{endif}%{if-debug}Debug%{endif}%{if-warning}Warning%{endif}%{if-critical}Error%{endif}%{if-fatal}Fatal%{endif}] %{message}");
	qInstallMessageHandler(&TBLog::Handler);

	// Setup path to log file.
	QDir logDir = TBUserFiles::GetBasePath();
	logDir.cd("logs");
	const QString logFileName("TimelineBuilder.log");

	// If we already have a log file, rename it so that it may serve as an archive.
	if (logDir.exists(logFileName))
	{
		// The %0 arg will be replaced with the file's creation time in ISO-8601 format.
		QString logArchiveName = "TimelineBuilder-%0-archive.log";
		QFileInfo oldLogFileInfo(logDir, logFileName);

#ifdef Q_OS_WINDOWS
		// Qt doesn't do permission lookup on NTFS file systems by default due to performance reasons.
		// Incrementing this flag enables it.  Once the QFileInfo::permission() call is complete, we
		// decrement it again.
		qt_ntfs_permission_lookup++;
#endif //Q_OS_WINDOWS

#ifdef Q_OS_UNIX
		// Qt currently doesn't handle file permission checking elegantly for Unix-likes.
		// Instead of QFileInfo::permission() checking the user's permissions for the file,
		// it checks the file's owner's permissions for the file.  As such, we have to operate
		// under the assumption that the log file has proper permissions if the current user
		// is the file's owner.
		const bool oldFilePermissions = oldLogFileInfo.ownerId() == getuid();
#else
		const bool oldFilePermissions = oldLogFileInfo.permission(QFile::WriteUser);
#endif //Q_OS_UNIX

#ifdef Q_OS_WINDOWS
		qt_ntfs_permission_lookup--;
#endif //Q_OS_WINDOWS

		if (!oldFilePermissions)
		{
			throw TBUserException("File permission error", "The current user does not have permission to rename the log file!");
		}

		// Arg %0 replacement
		const QDateTime creationTime = oldLogFileInfo.birthTime();
		logArchiveName = logArchiveName.arg(creationTime.toString(Qt::ISODate));
		const bool renameSuccess = logDir.rename(logFileName, logArchiveName);
		if (!renameSuccess)
		{
			throw TBUserException("File rename error", "Renaming old log file failed!");
		}
	}

	// Create a new log file for writing.
	LogFile = new QFile(logDir.filePath(logFileName));
	const bool success = LogFile->open(QIODeviceBase::WriteOnly | QIODeviceBase::NewOnly | QIODeviceBase::Append | QIODeviceBase::Text);
	if (!success)
	{
		Cleanup();
		throw TBUserException("File open error", "Could not create new log file!");
	}
}

void TBLog::Cleanup()
{
	if (LogFile != nullptr)
	{
		// Closing the file occurs during QFile's destructor, so we don't need to do it prior to delete.
		delete LogFile;
		LogFile = nullptr;
	}
}

void TBLog::Handler(QtMsgType messageType, const QMessageLogContext& context, const QString& message)
{
	// This custom handler only exists to pipe the logging to a file in addition to Qt's default places.
	if (LogFile != nullptr)
	{
		// Format and prepare log message for writing to file.
		QString printText = qFormatLogMessage(messageType, context, message);
		QByteArray utf8Text = printText.toUtf8();

		LogFile->write(utf8Text);

		if (messageType == QtFatalMsg)
		{
			// We're about to core dump.  Let's make sure we've written what we need!
			LogFile->flush();
		}
	}

	// Allow Qt's default message handler to print to console/debugger/whatever.
	(*DefaultMessageHandlerFunction)(messageType, context, message);
}