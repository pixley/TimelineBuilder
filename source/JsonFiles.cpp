/*
	Copyright (c) 2022 Tyler Pixley, all rights reserved.

	This file (JsonFiles.cpp) is part of TimelineBuilder.

	TimelineBuilder is free software: you can redistribute it and/or modify it under
	the terms of the GNU General Public License as published by the Free Software
	Foundation, either version 3 of the License, or (at your option) any later version.

	TimelineBuilder is distributed in the hope that it will be useful, but WITHOUT ANY
	WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
	PARTICULAR PURPOSE. See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along with
	TimelineBuilder. If not, see <https://www.gnu.org/licenses/>.
*/

#include "JsonFiles.h"
#include "Logging.h"

#include <QByteArray>

TBJsonFile::TBJsonFile() : file(), jsonDoc(), result(EJsonFileResult::NoFileSpecified)
{
}

TBJsonFile::TBJsonFile(const QString& filePath, QIODeviceBase::OpenMode openMode) : file(filePath), jsonDoc(), result()
{
	if (!file.open(openMode))
	{
		result = EJsonFileResult::FileNotFound;
	}
	else
	{
		QByteArray fileBytes = file.readAll();
		QJsonParseError error;
		jsonDoc = QJsonDocument::fromJson(fileBytes, &error);
		if (jsonDoc.isNull())
		{
			result = EJsonFileResult::FileNotJson;
			TBLog::Warning("Error parsing JSON file %0: %1", filePath, error.errorString());
		}
		else
		{
			result = EJsonFileResult::Success;
		}
	}
}

EJsonFileResult TBJsonFile::GetJsonDocument(QJsonDocument& outJsonDoc)
{
	if (result == EJsonFileResult::Success)
	{
		outJsonDoc = jsonDoc;
	}

	return result;
}