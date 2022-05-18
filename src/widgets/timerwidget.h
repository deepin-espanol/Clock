#ifndef TIMERWIDGET_H
#define TIMERWIDGET_H

#include "src/utils/circlepainter.h"
#include "numberspinbox.h"

#include <QPushButton>
#include <QLabel>
#include <QTimer>

class TimerWidget : public QWidget
{
	Q_OBJECT
public:
	explicit TimerWidget(QWidget *parent = nullptr);

protected:
	void resizeEvent(QResizeEvent *e) override;
	void paintEvent(QPaintEvent *e) override;

	void start();
	void stop();
	void count();

	void setVisibility(bool visible);
	//Delta into text.
	QString formatted(unsigned int delta);

private:
	//Used to render the circle.
	CirclePainter m_cpainter;
	//Used to update the UI.
	QTimer t;
	int m_size = 0;
	bool m_horizontal = false;

	unsigned int m_limit = 0;
	unsigned int m_delta = 0;
	float m_percent = 0;

	QWidget *m_container;

	QLabel *m_timeLabel;
	QPushButton *m_stop;
	QPushButton *m_start;

	QLabel *m_lHrs;
	QLabel *m_lMins;
	QLabel *m_lSecs;
	NumberSpinBox *m_secs;
	NumberSpinBox *m_mins;
	NumberSpinBox *m_hrs;
};

#endif // TIMERWIDGET_H
