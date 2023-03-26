/*
	Copyright (c) 2022 Tyler Pixley, all rights reserved.

	This file (main.cpp) is part of TimelineBuilder.

	TimelineBuilder is free software: you can redistribute it and/or modify it under
	the terms of the GNU General Public License as published by the Free Software
	Foundation, either version 3 of the License, or (at your option) any later version.

	TimelineBuilder is distributed in the hope that it will be useful, but WITHOUT ANY
	WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
	PARTICULAR PURPOSE. See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along with
	TimelineBuilder. If not, see <https://www.gnu.org/licenses/>.
*/

/*
	Begin build guarding
	These preprocessor directives ensure easy detection of invalid build environments.
	This is before any includes, specifically the Qt6 ones, so we have to use ISO and platform macros.

	The following build configurations will never be supported:
	- C++ standards prior to C++20 (use of concepts)
	- Sub-64-bit architectures (heavy use of int64 and double)
	- Compilers that don't support "#pragma once" (include guard macros are ugly)
*/

#if __cplusplus < 202002L
#error "TimelineBuilder requires C++20 or later!"
#endif //__cplusplus

#ifdef _WIN64
#ifndef _M_AMD64
#error "TimelineBuilder only supports x86_64 builds on Windows!"
#endif //_M_AMD64
// Windows x64 builds supported.  All good here.
#elif defined(_WIN32)
#error "TimelineBuilder does not support 32-bit builds!"
#else
#error "This platform is not yet supported by TimelineBuilder!"
#endif //_WIN64, _WIN32

/*
	End build guarding
*/

#include "PyBind.h"		// This has to come first in order to not conflict with Qt includes
#include "pybind11/embed.h"
#include "Version.h"
#include "TimelineBuilder.h"
#include "UserFiles.h"
#include "Logging.h"
#include "Settings.h"
#include "TestSuite.h"

#include <QtWidgets/QApplication>

#include <iostream>
#ifdef Q_OS_WINDOWS
#include <Windows.h>
#endif //Q_OS_WINDOWS

namespace py = pybind11;

int cleanupAfter(int exitCode)
{
	// Any pre-exit logic we want to run goes here.
	TBSettings::Cleanup();
	TBLog::Cleanup();

	return exitCode;
}

int main(int argc, char *argv[])
{
#ifdef Q_OS_WINDOWS
	// Set the console to use UTF-8 and then allocate a buffer to allow multi-byte characters to print
	SetConsoleOutputCP(CP_UTF8);
	setvbuf(stdout, nullptr, _IOFBF, 4);
	setvbuf(stderr, nullptr, _IOFBF, 4);
#endif //Q_OS_WINDOWS

	// Initialize the Qt application
	QApplication app(argc, argv);
	QApplication::setApplicationName("TimelineBuilder");
	QApplication::setApplicationDisplayName("TimelineBuilder");
	QApplication::setApplicationVersion(QString("%0.%1.%2").arg(MAJOR_VERSION, MINOR_VERSION, PATCH_VERSION));
	QApplication::setOrganizationName("Tyler Pixley");
	QApplication::setOrganizationDomain("https://pixley.github.io");

	// Initialize user files
	// This must be done before any use of user files, such as logging or settings.
	// This needs to be done after Qt app initialization so that we can carry forward some app info
	TBUserFiles::Initialize(app);

	// Initialize logging
	TBLog::Initialize();

	// Initialize settings
	TBSettings::Initialize();

	// Do some Python initialization
	py::scoped_interpreter pythonInterpreter;
	try
	{
		// Add /scripts to the Python sys path so that calendar system scripts can be loaded
		// I will spare you from a rant about my irritation with Python making people do this in the first place.
		py::module_ sysModule = py::module_::import("sys");
		sysModule.attr("path").attr("insert")(0, "scripts");
	}
	catch (py::error_already_set& pythonException)
	{
		TBLog::Error("Error in Python sys module!  %0  Aborting...", pythonException.what());
		return cleanupAfter(-1);
	}

	{
		// Scoping here so that the test suite doesn't exist for the entire runtime of the program.
		TBTestSuite tests(app);
		if (tests.RunTests())
		{
			// If any tests run, then we don't want to launch as a windowed application.
			return cleanupAfter(0);
		}
	}

	TimelineBuilder mainWindow;
	mainWindow.show();
	try
	{
		return cleanupAfter(app.exec());
	}
	catch (...)
	{
		// Still want to clean up after any unhandled exception
		return cleanupAfter(-1);
	}
}
