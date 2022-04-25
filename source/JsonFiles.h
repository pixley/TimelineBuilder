#pragma once

#include "CommonTypes.h"

#include <QFile>
#include <QJsonDocument>

enum class EJsonFileResult : uint8
{
	Success,
	NoFileSpecified,
	FileNotFound,
	FileNotJson
};

class TBJsonFile
{
public:
	TBJsonFile();
	TBJsonFile(const QString& filePath, QIODeviceBase::OpenMode openMode);
	// Only need auto-destructor at present because QFile's destructor handles closing the file.

	EJsonFileResult GetJsonDocument(QJsonDocument& outJsonDoc);

private:
	QFile file;
	QJsonDocument jsonDoc;
	EJsonFileResult result;
};