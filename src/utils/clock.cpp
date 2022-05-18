#include "clock.h"

#include <cmath>

#include <QIcon>
#include <QPainter>
#include <QWidget>
#include <QTime>
#include <QPainterPath>

/*
 * Parts of this code are extracted from the C++ code of dde-control-center, Clock module.
 */

//Generate the pixmap depending on the size of the widget.
QPixmap getPixmap(QWidget *src, const QString &name, const QSize size)
{
	const QIcon &icon = QIcon(name);
	const qreal ratio = src->devicePixelRatioF();
	QPixmap pixmap = icon.pixmap(size * ratio).scaled(size * ratio, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	QPainter p(&pixmap);
	p.setRenderHints(QPainter::Antialiasing);
	p.drawPixmap(0, 0, pixmap);
	pixmap.setDevicePixelRatio(ratio);
	return pixmap;
}

Clock::Clock(QWidget *src, bool enableFull)
{
	if (enableFull) {
		m_darkClock = getPixmap(src, ":/icons/dcc_clock_black.svg", clockSize);
		m_lightClock = getPixmap(src, ":/icons/dcc_clock_white.svg", clockSize);
	}

	m_hour = getPixmap(src, ":/icons/dcc_noun_hour.svg", pointSize);
	m_min = getPixmap(src, ":/icons/dcc_noun_minute.svg", pointSize);
	m_sec = getPixmap(src, ":/icons/dcc_noun_second.svg", pointSize);
}

void Clock::renderSimpleClock(QPainter *painter, const QPalette &palette, const QRect &r, const QTime time, const bool night) const
{
	painter->save();
	painter->setRenderHint(QPainter::RenderHint::Antialiasing);

	QPainterPath path;
	path.addRoundedRect(r, r.width()/2, r.height()/2);

	painter->fillPath(path, (night ? QColor("#282828") : Qt::white));

	painter->setPen(QPen(Qt::gray, 3));
	painter->drawPath(path);

	//Put the ticks
	QPointF src(r.x() + r.width()/2, r.y() + r.height()/2);

	const float min_angle = time.minute()/60*360;
	const float hour_angle = time.hour()/12*360;
	const int xs = r.width()/2 - 10;
	const int ys = r.height()/2 - 10;

	QPointF min_point(cos(min_angle) * xs, sin(min_angle) * ys);
	QPointF hour_point(cos(hour_angle)  * (xs-10), sin(hour_angle) * (ys-10));

	QPen hour_pen(palette.color(QPalette::ColorGroup::Current, QPalette::ColorRole::Highlight), 4);
	QPen min_pen(palette.color(QPalette::ColorGroup::Current, QPalette::ColorRole::Text), 2);

	min_pen.setCapStyle(Qt::PenCapStyle::RoundCap);
	hour_pen.setCapStyle(Qt::PenCapStyle::RoundCap);

	//Minutes
	painter->setPen(min_pen);
	painter->drawLine(src, min_point+src);

	//Hours
	painter->setPen(hour_pen);
	painter->drawLine(src, hour_point+src);

	painter->restore();
}

void Clock::renderFullClock(QPainter *painter, const QRect &r, QTime time, const bool night) const
{
	// draw plate
	painter->save();
	painter->translate(r.x() + r.width() / 2.0, r.y() + r.height() / 2.0);
	painter->drawPixmap(QPointF(-clockSize.width() / 2.0, -clockSize.height() / 2.0), (night ? m_darkClock : m_lightClock));
	painter->restore();

	int nHour = (time.hour() >= 12) ? (time.hour() - 12) : time.hour();
	int nStartAngle = 90;//The image from 0 start , but the clock need from -90 start

	// draw hour hand
	const qreal hourAngle = qreal(nHour * 30 + time.minute() * 30 / 60 + time.second() * 30 / 60 / 60 - nStartAngle);
	painter->save();
	painter->translate(r.x() + r.width() / 2.0, r.y() + r.height() / 2.0);
	painter->rotate(hourAngle);
	painter->drawPixmap(QPointF(-pointSize.width() / 2.0, -pointSize.height() / 2.0), m_hour);
	painter->restore();

	// draw minute hand
	const qreal minuteAngle = qreal(time.minute() * 6 + time.second() * 6 / 60 - nStartAngle);
	painter->save();
	painter->translate(r.x() + r.width() / 2.0, r.y() + r.height() / 2.0);
	painter->rotate(minuteAngle);
	painter->drawPixmap(QPointF(-pointSize.width() / 2.0, -pointSize.height() / 2.0), m_min);
	painter->restore();

	// draw second hand
	const qreal secondAngle = qreal(time.second() * 6 - nStartAngle);
	painter->save();
	painter->translate(r.x() + r.width() / 2.0, r.y() + r.height() / 2.0);
	painter->rotate(secondAngle);
	painter->drawPixmap(QPointF(-pointSize.width() / 2.0, -pointSize.height() / 2.0), m_sec);
	painter->restore();
}
