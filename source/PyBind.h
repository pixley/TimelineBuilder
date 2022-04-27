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

#include "Logging.h"

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
		TBLog::Error(QString("Exception when calling Python function '%0' from %1 (Line %2): %3").arg(functionName, callingFunction).arg(callingLine).arg(pythonException.what()));
		// Throw again so that this can be handled at a higher level if necessary.
		throw pythonException;
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
		TBLog::Error(QString("Exception when calling Python function '%0' from %1 (Line %2): %3").arg(functionName, callingFunction).arg(callingLine).arg(pythonException.what()));
		// Throw again so that this can be handled at a higher level if necessary.
		throw pythonException;
	}
}