#ifndef ALARMSWIDGET_H
#define ALARMSWIDGET_H

#include "src/lists/alarmslist.h"

#include <QWidget>
#include <QTimer>

class AlarmsWidget : public QWidget
{
	Q_OBJECT
public:
	explicit AlarmsWidget(QWidget *parent = nullptr);

private:
	AlarmsList m_list;
	//Used to update to the latest alarm.
	QTimer m_timer;
};

#endif // ALARMSWIDGET_H
