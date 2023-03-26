/*
	Copyright (c) 2022 Tyler Pixley, all rights reserved.

	This file (PyBind.h) is part of TimelineBuilder.

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

#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#include "Logging.h"

#include <QtCore/QString>

#include <utility>
#include <stdexcept>

namespace py = pybind11;

// Repackage exceptions for display and logging.
#define CATCH_PY_EXCEPTIONS \
catch (py::error_already_set& pythonException) \
{ \
	TBLog::Error("Exception when calling Python function '%0' from %1 (Line %2): %3", functionName, callingFunction, callingLine, pythonException.what()); \
	throw pythonException; \
} \
catch (std::runtime_error& otherException) \
{ \
	TBLog::Error("Exception inside pybind11 from %0 (Line %1): %2", callingFunction, callingLine, otherException.what()); \
	throw otherException; \
}

template<typename T, typename... Args>
T CallPythonFunction(py::module_& pythonModule, const char* functionName, const char* callingFunction, int callingLine, Args&&... args)
{
	try
	{
		py::object result = pythonModule.attr(functionName)(std::forward<Args>(args)...);
		return result.cast<T>();
	}
	CATCH_PY_EXCEPTIONS
}

template<typename... Args>
void CallVoidPythonFunction(py::module_& pythonModule, const char* functionName, const char* callingFunction, int callingLine, Args&&... args)
{
	try
	{
		pythonModule.attr(functionName)(std::forward<Args>(args)...);
	}
	CATCH_PY_EXCEPTIONS
}

template<typename T, typename... Args>
T CallPythonMethod(py::object& pythonObject, const char* functionName, const char* callingFunction, int callingLine, Args&&... args)
{
	try
	{
		py::object result = pythonObject.attr(functionName)(std::forward<Args>(args)...);
		return result.cast<T>();
	}
	CATCH_PY_EXCEPTIONS
}

template<typename... Args>
void CallVoidPythonMethod(py::object& pythonObject, const char* functionName, const char* callingFunction, int callingLine, Args&&... args)
{
	try
	{
		pythonObject.attr(functionName)(std::forward<Args>(args)...);
	}
	CATCH_PY_EXCEPTIONS
}