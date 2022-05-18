#ifndef STOPWATCHWIDGET_H
#define STOPWATCHWIDGET_H

#include "src/utils/circlepainter.h"

#include <QWidget>
#include <QTimer>

class QPushButton;
class QLabel;

class StopWatchWidget : public QWidget
{
	Q_OBJECT
public:
	explicit StopWatchWidget(QWidget *parent = nullptr);

	Q_SLOT void mark();

Q_SIGNALS:
	//Triggered when "Lap" is pushed.
	void deltaMark(uint delta);
	//Reset the stopwatch.
	void reset();

protected:
	void resizeEvent(QResizeEvent *e) override;
	void paintEvent(QPaintEvent *e) override;

	void start(); //Start button clicked
	void stop();  //Stop button clicked
	void count(); //Triggered
	void handlePress();

	void setVisibility(bool visible);
	//Put the input delta into a friendly text.
	QString formatted(unsigned int delta);

	enum UpdateScope {
		MiliSecs = 1,
		Secs,
		Mins,
		Hrs
	};

private:
	//Used to render the circle.
	CirclePainter m_cpainter;
	//Used to update the image.
	QTimer m_timer;

	QWidget *m_container;
	QPushButton *m_stop;
	QPushButton *m_start;
	QLabel *m_timeLabel;

	//Time counter.
	uint m_delta = 0;
	//These help to make slow down animation while time goes up.
	uint m_lmark = 0;
	UpdateScope m_currentScope;

	int m_size = 0;
	bool m_horizontal = false;
	bool m_visibility = true;
	bool m_needsUpdate = true;

	QPixmap source;
};

#endif // STOPWATCHWIDGET_H
