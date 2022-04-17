#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_TimelineBuilder.h"

class TimelineBuilder : public QMainWindow
{
    Q_OBJECT

public:
    TimelineBuilder(QWidget *parent = Q_NULLPTR);

private:
    Ui::TimelineBuilderClass ui;
};
