#ifndef CLOCK_H
#define CLOCK_H

#include <QSize>
#include <QPixmap>

//As defined in C++ code from dde-control-center, Clock module.
static const QSize clockSize = QSize(224, 224);
static const QSize pointSize = QSize(145, 15);

/**
 * @brief Class used to render a clock.
 */
class Clock
{
public:
	Clock(QWidget *scr, bool enableFull = false);

	/**
	 * @brief Render a flat clock without much overhead.
	 * @param p Source painter.
	 * @param palette Palette used for the colors.
	 * @param rect Rect in which the clock will be rendered.
	 * @param t Time to be drawn.
	 * @param night Changes colors on night/light time.
	 */
	void renderSimpleClock(QPainter *p, const QPalette &palette, const QRect &rect, const QTime t, const bool night) const;
	/**
	 * @brief Render a full clock with shades and more.
	 * @param p Source painter.
	 * @param rect Rect in which the clock will be rendered.
	 * @param t Time to be drawn.
	 * @param night Changes colors on night/light time.
	 */
	void renderFullClock(QPainter *p, const QRect &rect, const QTime t, const bool night) const;

private:
	QPixmap m_lightClock;
	QPixmap m_darkClock;

	QPixmap m_hour;
	QPixmap m_min;
	QPixmap m_sec;
};

#endif // CLOCK_H
