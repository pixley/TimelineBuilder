/*
	Copyright (c) 2022 Tyler Pixley, all rights reserved.

	This file (Calendar.h) is part of TimelineBuilder.

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

#include "CommonTypes.h"
#include "JsonableObject.h"
#include "Time.h"

#include <QtCore/QString>
#include <QtCore/QList>

#include <memory>

// Forward-declaring
namespace pybind11
{
	class module_;
	class object;
}

class TBCalendarSystem : public JsonableObject
{
public:
	TBCalendarSystem();

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

	std::unique_ptr<pybind11::module_> CalendarScript;
	std::unique_ptr<pybind11::object> CalendarObject;

	// These values won't change during script execution, so we cache them right after initializing the script
	int32 CachedBrokenDateLength;
	QString CachedDateFormat;
	QString CachedTimespanFormat;
};