#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "tabchooser.h"
#include "alarmswidget.h"
#include "worldclockswidget.h"
#include "timerwidget.h"
#include "stopwatch.h"

#include <QStackedWidget>
#include <DMainWindow>

DWIDGET_USE_NAMESPACE

class MainWindow : public DMainWindow
{
	Q_OBJECT
public:
	explicit MainWindow(QWidget *p = nullptr);

Q_SIGNALS:
	void closed();

protected:
	void closeEvent(QCloseEvent *e) override;

private:
	QStackedWidget *m_stack;
	TabChooser *m_chooser;
	WorldClocksWidget *m_clocks;
	AlarmsWidget *m_alarms;
	TimerWidget *m_timer;
	StopWatch *m_swatch;
};

#endif // MAINWINDOW_H
