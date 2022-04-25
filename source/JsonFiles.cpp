#include "JsonFiles.h"

#include <QByteArray>
#include <QtGlobal>
#include <QtDebug>

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
			qWarning() << QString("Error parsing JSON file %0: %1").arg(filePath, error.errorString());
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