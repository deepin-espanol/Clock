#ifndef WORLDCLOCKSWIDGET_H
#define WORLDCLOCKSWIDGET_H

#include "src/external/timezonechooser.h"

#include "src/utils/clock.h"
#include "src/lists/timezoneslist.h"

#include <QTimer>
#include <QWidget>

/**
 * @brief A big clock showing the local time.
 */
class BigClock : public QWidget
{
	Q_OBJECT
public:
	explicit BigClock(QWidget *parent = nullptr);

protected:
	void paintEvent(QPaintEvent *e) override;

private:
	//Used to render the clock.
	Clock m_clock;
	//Used to update the UI.
	QTimer m_timer;
};

using namespace dcc::datetime;

class WorldClocksWidget : public QWidget
{
	Q_OBJECT
public:
	explicit WorldClocksWidget(QWidget *parent = nullptr);

protected:
	void paintEvent(QPaintEvent *e) override;

private:
	BigClock *m_clock;
	TimeZonesList *m_list;
	QPushButton *m_addBtn;
	QPushButton *m_removeBtn;
	TimeZoneChooser *chooser;
};

#endif // WORLDCLOCKSWIDGET_H
