#pragma once

#include "CommonTypes.h"
#include "JsonableObject.h"
#include "Time.h"

#include <QString>
#include <QList>

// Forward-declaring
namespace pybind11
{
	class module_;
}

class TBCalendarSystem : public JsonableObject
{
public:
	TBCalendarSystem();
	virtual ~TBCalendarSystem();

	virtual bool LoadFromJson(const QJsonObject& jsonObject) override;
	virtual void PopulateJson(QJsonObject& jsonObject) const override;

	QString GetName() const { return Name; }
	QString GetDescription() const { return Description; }

	bool InitializeScript();

	QString FormatDate(TBDate date) const;
	QString FormatDate(const TBBrokenDate& date) const;
	QString FormatDateSpan(TBDate startDate, TBDate endDate) const;
	QString FormatTimespan(const TBBrokenTimespan& span) const;
	void BreakDate(TBDate date, TBBrokenDate& outBrokenDate) const;
	void BreakDateSpan(TBDate startDate, TBDate endDate, TBBrokenTimespan& outBrokenSpan) const;
	TBDate CombineDate(const TBBrokenDate& brokenDate) const;
	TBDate MoveDate(TBDate startDate, const TBBrokenTimespan& deltaTime) const;
	bool ValidateBrokenDate(const TBBrokenDate& testDate) const;

	int32 GetBrokenDateLength() const;
	QString GetDateFormat() const;
	QString GetTimespanFormat() const;

private:
	QString Name;
	QString ScriptName;
	QString Description;
	pybind11::module_* CalendarScript;

	// These values won't change during script execution, so we cache them right after initializing the script
	int32 CachedBrokenDateLength;
	QString CachedDateFormat;
	QString CachedTimespanFormat;
};