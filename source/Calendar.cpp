#include "PyBind.h"		// This has to come first to not conflict with Qt defines.
#include "Calendar.h"

#include <vector>
#include <string>

#include <QtGlobal>

#define ScriptFunction(type, functionName, ...) \
	CallPythonFunction<type>(*CalendarScript, functionName, __FUNCTION__, __LINE__ __VA_OPT__(,) __VA_ARGS__)
#define VoidScriptFunction(functionName, ...) \
	CallVoidPythonFunction(*CalendarScript, functionName, __FUNCTION__, __LINE__ __VA_OPT__(,) __VA_ARGS__)

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
	CalendarScript(nullptr),
	CachedBrokenDateLength(0),
	CachedDateFormat(),
	CachedTimespanFormat()
{}

TBCalendarSystem::~TBCalendarSystem()
{
	if (CalendarScript != nullptr)
	{
		delete CalendarScript;
	}
}

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
		qWarning() << "Attempting to initialize calendar system script with an empty name!";
		return false;
	}

	CalendarScript = new py::module_();
	try
	{
		*CalendarScript = py::module_::import(ScriptName.toUtf8());
	}
	catch (py::error_already_set& pythonException)
	{
		qWarning("%s, line %i: Exception from module import: %s", __FUNCTION__, __LINE__, pythonException.what());
		return false;
	}

	VoidScriptFunction("init_calendar");
	CachedBrokenDateLength = ScriptFunction(int32, "get_broken_date_length");
	CachedDateFormat = ScriptFunction(std::string, "get_date_format").data();
	CachedTimespanFormat = ScriptFunction(std::string, "get_timespan_format").data();

	return true;
}

QString TBCalendarSystem::FormatDate(TBDate date) const
{
	return ScriptFunction(std::string, "format_date", date.GetDays()).data();
}

QString TBCalendarSystem::FormatDate(const TBBrokenDate& date) const
{
	std::vector<int64> dateVector(date.begin(), date.end());
	return ScriptFunction(std::string, "format_broken_date", dateVector).data();
}

QString TBCalendarSystem::FormatDateSpan(TBDate startDate, TBDate endDate) const
{
	return ScriptFunction(std::string, "format_broken_date", startDate.GetDays(), endDate.GetDays()).data();
}

QString TBCalendarSystem::FormatTimespan(const TBBrokenTimespan& span) const
{
	std::vector<int64> spanVector(span.begin(), span.end());
	return ScriptFunction(std::string, "format_timespan", spanVector).data();
}

void TBCalendarSystem::BreakDate(TBDate date, TBBrokenDate& outBrokenDate) const
{
	std::vector<int64> result = ScriptFunction(std::vector<int64>, "break_date", date.GetDays());
	outBrokenDate = TBBrokenTimespan(result.begin(), result.end());
}

void TBCalendarSystem::BreakDateSpan(TBDate startDate, TBDate endDate, TBBrokenTimespan& outBrokenSpan) const
{
	std::vector<int64> result = ScriptFunction(std::vector<int64>, "break_date_span", startDate.GetDays(), endDate.GetDays());
	outBrokenSpan = TBBrokenTimespan(result.begin(), result.end());
}

TBDate TBCalendarSystem::CombineDate(const TBBrokenDate& brokenDate) const
{
	std::vector<int64> dateVector(brokenDate.begin(), brokenDate.end());
	return ScriptFunction(int64, "combine_date", dateVector);
}

TBDate TBCalendarSystem::MoveDate(TBDate startDate, const TBBrokenTimespan& deltaTime) const
{
	std::vector<int64> deltaVector(deltaTime.begin(), deltaTime.end());
	return TBDate(ScriptFunction(int64, "combine_date", startDate.GetDays(), deltaVector));
}

bool TBCalendarSystem::ValidateBrokenDate(const TBBrokenDate& brokenDate) const
{
	std::vector<int64> dateVector(brokenDate.begin(), brokenDate.end());
	return ScriptFunction(bool, "validate_date", dateVector);
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