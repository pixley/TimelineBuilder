#pragma once

#include "pybind11/pybind11.h"

#include <QtGlobal>
#include <QtDebug>
#include <QString>

#include <utility>

namespace py = pybind11;

template<typename T, typename... Args>
T CallPythonFunction(py::module_& pythonModule, const char* functionName, const char* callingFunction, int callingLine, Args&&... args)
{
	try
	{
		py::object result = pythonModule.attr(functionName)(std::forward<Args>(args)...);
		return result.cast<T>();
	}
	catch (py::error_already_set& pythonException)
	{
		qCritical() << QString("Exception when calling Python function '%0' from %1 (Line %2): %3").arg(functionName, callingFunction).arg(callingLine).arg(pythonException.what());
		throw;
	}
}

template<typename... Args>
void CallVoidPythonFunction(py::module_& pythonModule, const char* functionName, const char* callingFunction, int callingLine, Args&&... args)
{
	try
	{
		pythonModule.attr(functionName)(std::forward<Args>(args)...);
	}
	catch (py::error_already_set& pythonException)
	{
		qCritical() << QString("Exception when calling Python function '%0' from %1 (Line %2): %3").arg(functionName, callingFunction).arg(callingLine).arg(pythonException.what());
		throw;
	}
}