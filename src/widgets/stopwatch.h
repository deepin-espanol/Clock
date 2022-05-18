#ifndef STOPWATCH_H
#define STOPWATCH_H

#include "stopwatchwidget.h"
#include "src/lists/stopslist.h"

#include <QWidget>

class StopWatch : public QWidget
{
	Q_OBJECT
public:
	explicit StopWatch(QWidget *parent = nullptr);

private:
	StopWatchWidget *widget;
	StopsList m_list;
};

#endif // STOPWATCH_H
