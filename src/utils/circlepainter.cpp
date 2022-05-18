#include "circlepainter.h"

#include <QPainter>
#include <QPainterPath>

#include <QPalette>

#include <math.h>

CirclePainter::CirclePainter()
{
}

void CirclePainter::render(QPainter &painter, const QPalette &palette, const QRect &r, const float percent)
{
	painter.save();
	painter.setRenderHint(QPainter::Antialiasing, true);

	const int delta = 3.6*percent;

	QPen p(palette.color(QPalette::ColorGroup::Current, QPalette::ColorRole::Button), 5);
	p.setCapStyle(Qt::PenCapStyle::RoundCap);

	//Draw the non highlighted part first.
	painter.setPen(p);
	painter.drawArc(p.width()/2+r.x(), p.width()/2+r.y(), r.width()-p.width(), r.height()-p.width(), (delta+90)*16, (360 - delta)*16);

	//Draw the highlighted part.
	p.setColor(palette.color(QPalette::ColorGroup::Current, QPalette::ColorRole::Highlight));
	painter.setPen(p);
	painter.drawArc(p.width()/2+r.x(), p.width()/2+r.y(), r.width()-p.width(), r.height()-p.width(), (/*m_angle+*/90/*+delta*/)*16, delta*16);

	painter.restore();
}

void CirclePainter::render(QPainter &painter, const QPalette &palette, const QRect &r)
{
	painter.save();
	painter.setRenderHint(QPainter::Antialiasing, true);

	QPen p(palette.color(QPalette::ColorGroup::Current, QPalette::ColorRole::Button), 5);
	p.setCapStyle(Qt::PenCapStyle::RoundCap);

	if (360*16 < m_angle) {
		m_angle = 0;
	}

	//Draw the non highlighted part first.
	painter.setPen(p);
	painter.drawArc(p.width()/2+r.x(), p.width()/2+r.y(), r.width()-p.width(), r.height()-p.width(), 0, 360*16);

	//Draw the highlighted part.
	p.setColor(palette.color(QPalette::ColorGroup::Current, QPalette::ColorRole::Highlight));
	painter.setPen(p);
	painter.drawArc(p.width()/2+r.x(), p.width()/2+r.y(), r.width()-p.width(), r.height()-p.width(), m_angle + 90*16, m_size*16);

	painter.restore();
	m_angle += 5.76*2;
	m_size += (m_angle < 180*16) ? 0.1 : -0.1;
}

void CirclePainter::reset()
{
	m_angle = 0;
	m_size = 6;
}
