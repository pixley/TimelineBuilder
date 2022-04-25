#include "PyBind.h"		// This has to come first in order to not conflict with Qt includes
#include "pybind11/embed.h"
#include "Version.h"
#include "TimelineBuilder.h"

#include <QtWidgets/QApplication>
#include <QtGlobal>
#include <QtDebug>

#ifdef _DEBUG
#include "TestSuite.h"
#endif

namespace py = pybind11;

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	QApplication::setApplicationName("TimelineBuilder");
	QApplication::setApplicationDisplayName("TimelineBuilder");
	QApplication::setApplicationVersion(QString("%0.%1.%2").arg(MAJOR_VERSION).arg(MINOR_VERSION).arg(PATCH_VERSION));
	QApplication::setOrganizationName("Tyler Pixley");
	QApplication::setOrganizationDomain("https://pixley.github.io");

	// Do some Python initialization
	py::scoped_interpreter pythonInterpreter;
	try
	{
		py::module_ sysModule = py::module_::import("sys");
		sysModule.attr("path").attr("insert")(0, "scripts");
	}
	catch (py::error_already_set& pythonException)
	{
		qCritical() << QString("Error loading Python sys module!  %0  Aborting...").arg(pythonException.what());
		return -1;
	}

	{
		// Scoping here so that the test suite doesn't exist for the entire runtime of the program.
		TBTestSuite tests(app);
		if (tests.RunTests())
		{
			return 0;
		}
	}

	TimelineBuilder mainWindow;
	mainWindow.show();
	return app.exec();
}
