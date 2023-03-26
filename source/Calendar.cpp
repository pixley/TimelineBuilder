/*
	Copyright (c) 2022 Tyler Pixley, all rights reserved.

	This file (Calendar.cpp) is part of TimelineBuilder.

	TimelineBuilder is free software: you can redistribute it and/or modify it under
	the terms of the GNU General Public License as published by the Free Software
	Foundation, either version 3 of the License, or (at your option) any later version.

	TimelineBuilder is distributed in the hope that it will be useful, but WITHOUT ANY
	WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
	PARTICULAR PURPOSE. See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along with
	TimelineBuilder. If not, see <https://www.gnu.org/licenses/>.
*/

#include "PyBind.h"		// This has to come first to not conflict with Qt defines.
#include "Calendar.h"
#include "Logging.h"

#include <vector>
#include <string>

// These are not safe to call until the CalendarScript is initialized.
#define ScriptFunction(type, functionName, ...) \
	CallPythonFunction<type>(*CalendarScript, functionName, __FUNCTION__, __LINE__ __VA_OPT__(,) __VA_ARGS__)
#define VoidScriptFunction(functionName, ...) \
	CallVoidPythonFunction(*CalendarScript, functionName, __FUNCTION__, __LINE__ __VA_OPT__(,) __VA_ARGS__)
// These are not safe to call until the CalendarObject is initialized.
#define ScriptMethod(type, methodName, ...) \
	CallPythonMethod<type>(*CalendarObject, methodName, __FUNCTION__, __LINE__ __VA_OPT__(,) __VA_ARGS__)
#define VoidScriptMethod(functionName, ...) \
	CallVoidPythonMethod(*CalendarObject, methodName, __FUNCTION__, __LINE__ __VA_OPT__(,) __VA_ARGS__)

/*
	Helper functions for converting between Python arrays and QLists
*/
template<typename T>
void PythonObjectToQList(const py::object& inObject, QList<T>& outList)
{
	std::vector<T> intermediate = inObject.cast<std::vector<T>>();
	outList = QList<T>(intermediate.begin(), intermediate.end());
}

/*
	TBCalendarSystem
*/
TBCalendarSystem::TBCalendarSystem() : JsonableObject(),
	Name(),
	Description(),
	ScriptName(),
	CalendarScript(),
	CalendarObject(),
	CachedBrokenDateLength(0),
	CachedDateFormat(),
	CachedTimespanFormat()
{}

bool TBCalendarSystem::LoadFromJson(const QJsonObject& jsonObject)
{
	JsonableObject::LoadFromJson(jsonObject);

	Name = JsonToString(jsonObject, "name");
	Description = JsonToString(jsonObject, "description");
	ScriptName = JsonToString(jsonObject, "script_name");

	return LoadSuccessful;
}

void TBCalendarSystem::PopulateJson(QJsonObject& jsonObject) const
{
	jsonObject.insert("name", Name);
	jsonObject.insert("description", Description);
	jsonObject.insert("script_path", ScriptName);
}

bool TBCalendarSystem::InitializeScript()
{
	if (ScriptName.isEmpty())
	{
		TBLog::Warning("Attempting to initialize calendar system script with an empty name!");
		return false;
	}

	CalendarScript = std::make_unique<py::module_>();
	CalendarObject = std::make_unique<py::object>();

	try
	{
		*CalendarScript = py::module_::import(ScriptName.toUtf8());
	}
	catch (py::error_already_set& pythonException)
	{
		TBLog::Error("%0, line %1: Exception from module import: %2", __FUNCTION__, __LINE__, pythonException.what());
		return false;
	}

	*CalendarObject = ScriptFunction(py::object, "init_calendar");

	CachedBrokenDateLength = ScriptMethod(int32, "get_broken_date_length");
	CachedDateFormat = ScriptMethod(std::string, "get_date_format").data();
	CachedTimespanFormat = ScriptMethod(std::string, "get_timespan_format").data();

	return true;
}

QString TBCalendarSystem::FormatDate(TBDate date) const
{
	return ScriptMethod(std::string, "format_date", date.GetDays()).data();
}

QString TBCalendarSystem::FormatDate(const TBBrokenDate& date) const
{
	std::vector<int64> dateVector(date.begin(), date.end());
	return ScriptMethod(std::string, "format_broken_date", dateVector).data();
}

QString TBCalendarSystem::FormatDateSpan(TBDate startDate, TBDate endDate) const
{
	return ScriptMethod(std::string, "format_date_span", startDate.GetDays(), endDate.GetDays()).data();
}

QString TBCalendarSystem::FormatTimespan(const TBBrokenTimespan& span) const
{
	std::vector<int64> spanVector(span.begin(), span.end());
	return ScriptMethod(std::string, "format_timespan", spanVector).data();
}

void TBCalendarSystem::BreakDate(TBDate date, TBBrokenDate& outBrokenDate) const
{
	std::vector<int64> result = ScriptMethod(std::vector<int64>, "break_date", date.GetDays());
	outBrokenDate = TBBrokenTimespan(result.begin(), result.end());
}

void TBCalendarSystem::BreakDateSpan(TBDate startDate, TBDate endDate, TBBrokenTimespan& outBrokenSpan) const
{
	std::vector<int64> result = ScriptMethod(std::vector<int64>, "break_date_span", startDate.GetDays(), endDate.GetDays());
	outBrokenSpan = TBBrokenTimespan(result.begin(), result.end());
}

TBDate TBCalendarSystem::CombineDate(const TBBrokenDate& brokenDate) const
{
	std::vector<int64> dateVector(brokenDate.begin(), brokenDate.end());
	return ScriptMethod(int64, "combine_date", dateVector);
}

TBDate TBCalendarSystem::MoveDate(TBDate startDate, const TBBrokenTimespan& deltaTime) const
{
	std::vector<int64> deltaVector(deltaTime.begin(), deltaTime.end());
	return TBDate(ScriptMethod(int64, "move_date", startDate.GetDays(), deltaVector));
}

bool TBCalendarSystem::ValidateBrokenDate(const TBBrokenDate& brokenDate) const
{
	std::vector<int64> dateVector(brokenDate.begin(), brokenDate.end());
	return ScriptMethod(bool, "validate_date", dateVector);
}

int32 TBCalendarSystem::GetBrokenDateLength() const
{
	return CachedBrokenDateLength;
}

QString TBCalendarSystem::GetDateFormat() const
{
	return CachedDateFormat;
}

QString TBCalendarSystem::GetTimespanFormat() const
{
	return CachedTimespanFormat;
}