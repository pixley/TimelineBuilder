#include "TimelineBuilder.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TimelineBuilder w;
    w.show();
    return a.exec();
}
