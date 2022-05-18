#include "numberspinbox.h"

#include "src/helpers.h"

#include <QMouseEvent>
#include <QResizeEvent>
#include <QWheelEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPalette>

#include <iostream>

NumberSpinBox::NumberSpinBox(QWidget *parent) : QWidget(parent)
{
	setMinimumSize(20, 50);
}

int NumberSpinBox::value()
{
	return m_val;
}

int NumberSpinBox::step()
{
	return m_step;
}

void NumberSpinBox::setValue(int v)
{
	m_val = v;
}

void NumberSpinBox::setStep(int s)
{
	m_step = s;
}

void NumberSpinBox::setRange(int min, int max)
{
	m_min = min;
	m_max = max;
	if (m_val < min) {
		m_val = min;
	} else if (m_max < max) {
		m_val = max;
	}
	repaint();
}

int NumberSpinBox::areas(const QPoint &p) const
{
	int i = 0, py = 0, flag = 0;
	while (i < 4) {
		const int y = py + m_height;
		if (p.y() >= py && p.y() <= y) {
			setBit((unsigned int &)flag, i);
		}
		py += m_height+5;
		i++;
	}
	return flag;
}

int NumberSpinBox::areas(const QRect &r, bool force) const
{
	int v = orInt(orInt(areas(r.topLeft()), areas(r.topRight())), orInt(areas(r.bottomLeft()), areas(r.bottomRight())));
	if (force && CHECK_BIT(v, 0) && CHECK_BIT(v, 2)) {
		setBit((unsigned int &)v, 1);
	}
	return v;
}

void NumberSpinBox::increment(int steps)
{
	m_val += m_step*steps;
	while (m_max < m_val) {
		m_val = m_val - m_max + m_min - 1;
	}
	Q_EMIT valueChanged(m_val);
}

void NumberSpinBox::decrement(int steps)
{
	m_val -= m_step*steps;
	while (m_val < m_min) {
		m_val += (m_max - m_min) + 1;
	}
	Q_EMIT valueChanged(m_val);
}

void NumberSpinBox::mousePressEvent(QMouseEvent *e)
{
	m_clicked = true;
	m_area = (Areas)areas(e->pos());
	e->accept();
	repaint();
}

void NumberSpinBox::mouseReleaseEvent(QMouseEvent *e)
{
	m_clicked = false;
	m_area = Areas::None;

	switch (areas(e->pos())) {
	case Areas::Bottom: {
		decrement();
		break;
	}
	case Areas::Top: {
		increment();
		break;
	}
	default: break;
	}

	e->accept();
	repaint();
}

void NumberSpinBox::wheelEvent(QWheelEvent *e)
{
	e->accept();
	m_clicked = false;
	const int deg = e->angleDelta().y()/16;
	if (deg < 0) {
		decrement(0-deg);
	} else {
		increment(deg);
	}
	repaint();
}

void NumberSpinBox::resizeEvent(QResizeEvent *e)
{
	const int h = e->size().height() - 10;
	if (h < 1) {
		m_height = 1;
	} else {
		m_height = h/3;
	}
	m_useVertical = m_height <= e->size().width();
	m_s = m_useVertical ? m_height : e->size().width();
	e->accept();
	return QWidget::resizeEvent(e);
}

void NumberSpinBox::paintEvent(QPaintEvent *e)
{
	QPainter p(this);

	flex_switch(areas(e->rect(), true), 3, {
	std::bind([&p, this]() {
		const QColor c = m_clicked && m_area == Areas::Top
					? palette().color(QPalette::ColorGroup::Current, QPalette::ColorRole::Highlight)
					: palette().color(QPalette::ColorGroup::Current, QPalette::ColorRole::Button);
		const QColor t = m_clicked && m_area == Areas::Top
					? palette().color(QPalette::ColorGroup::Current, QPalette::ColorRole::HighlightedText)
					: palette().color(QPalette::ColorGroup::Current, QPalette::ColorRole::ButtonText);

		//Draw the rect
		QPainterPath path;
		path.setFillRule(Qt::FillRule::WindingFill);
		path.addRoundedRect(QRect(0, 0, width()-1, m_height/2+10), 8, 8);
		path.addRect(0, m_height/2, width()-1, m_height/2);
		p.fillPath(path, c);

		//Draw the arrow
		path.clear();
		const QPoint d(m_useVertical ? (width()-m_height)/2 : 0, m_useVertical ? 0 : (width()-m_height)/2);
		const float r = m_height/6;
		path.addPolygon(QPolygon({QPoint(1, 4)*r+d, QPoint(3, 2)*r+d, QPoint(5, 4)*r+d}));

		QPen apen(t, 3);
		apen.setCapStyle(Qt::PenCapStyle::RoundCap);
		p.setPen(apen);
		p.drawPath(path);
	}),
	std::bind([&e, &p, this](){
		QPainterPath path;
		QRect r(0, m_height+5, width()-1, m_height);

		//Put the BG
		p.fillRect(r.intersected(e->rect()), palette().color(QPalette::ColorGroup::Current, QPalette::ColorRole::Base));

		//Paint the text
		QTextOption topt;
		topt.setAlignment(Qt::AlignCenter);
		QFont f = font();
		f.setPointSize(20);
		p.setFont(f);
		p.setPen(QPen(palette().color(QPalette::ColorGroup::Current, QPalette::ColorRole::Text)));
		p.drawText(r, QString::number(m_val), topt);
	}),
	std::bind([&p, this](){
		const QColor c = m_clicked && m_area == Areas::Bottom
					? palette().color(QPalette::ColorGroup::Current, QPalette::ColorRole::Highlight)
					: palette().color(QPalette::ColorGroup::Current, QPalette::ColorRole::Button);
		const QColor t = m_clicked && m_area == Areas::Bottom
					? palette().color(QPalette::ColorGroup::Current, QPalette::ColorRole::HighlightedText)
					: palette().color(QPalette::ColorGroup::Current, QPalette::ColorRole::ButtonText);
		const int cy = m_height*2+10;

		//Draw the rect
		QPainterPath path;
		path.setFillRule(Qt::FillRule::WindingFill);
		path.addRoundedRect(QRect(0, cy + m_height/2 - 10, width()-1, m_height/2+10), 8, 8);
		path.addRect(0, cy, width()-1, m_height/2+10);
		p.fillPath(path, c);

		//Draw the arrow
		path.clear();
		const QPoint d = QPoint(m_useVertical ? (width()-m_height)/2 : 0, m_useVertical ? 0 : (width()-m_height)/2) + QPoint(0, cy);
		const float r = m_height/6;
		path.addPolygon(QPolygon({QPoint(1, 2)*r+d, QPoint(3, 4)*r+d, QPoint(5, 2)*r+d}));

		QPen apen(t, 3);
		apen.setCapStyle(Qt::PenCapStyle::RoundCap);
		p.setPen(apen);
		p.drawPath(path);
	})
	});

	e->accept();
}
