#include "dualwidgetcontainer.h"

#include <QResizeEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPalette>

DualWidgetContainer::DualWidgetContainer(QWidget *f, QWidget *s, QWidget *p)
	: QWidget(p),
	  first(f),
	  second(s)
{
	if (first) {
		first->setParent(this);
	}
	if (second) {
		second->setParent(this);
	}
}

DualWidgetContainer::DualWidgetContainer(QWidget *p) : DualWidgetContainer(nullptr, nullptr, p) {}

void DualWidgetContainer::setWidgets(QWidget *f, QWidget *s)
{
	if (f) {
		f->setParent(this);
	}
	if (s) {
		s->setParent(this);
	}
	first = f;
	second = s;
	makeLayout(size());
}

void DualWidgetContainer::setMarginsOfFirst(const int margin)
{
	m_fmargin = margin;
	makeLayout(size());
}

void DualWidgetContainer::setMarginsOfSecond(const int margin)
{
	m_smargin = margin;
	makeLayout(size());
}

void DualWidgetContainer::setSpacing(int spacing)
{
	m_spacing = spacing;
	makeLayout(size());
}

void DualWidgetContainer::setSmaller(bool onFirst)
{
	m_minimumOnFirst = onFirst;
	makeLayout(size());
}

void DualWidgetContainer::paintEvent(QPaintEvent *e)
{
	QPainter p(this);
	p.fillRect(e->rect(), palette().color(QPalette::ColorGroup::Current, QPalette::ColorRole::Base));
}

void DualWidgetContainer::resizeEvent(QResizeEvent *e)
{
	QWidget::resizeEvent(e);
	makeLayout(e->size());
}

VerticalDWC::VerticalDWC(QWidget *f, QWidget *s, QWidget *p) : DualWidgetContainer(f, s, p) {}

VerticalDWC::VerticalDWC(QWidget *p) : DualWidgetContainer(p) {}

void VerticalDWC::makeLayout(const QSize &s)
{
	if (first && second) {
		if (m_minimumOnFirst) {
			QRect result(m_fmargin, m_fmargin, s.width() - m_fmargin*2, first->minimumHeight());
			first->setGeometry(result);
			second->setGeometry(QRect(QPoint(m_smargin, result.y() + result.height() + m_spacing), QSize(s.width() - m_smargin*2, s.height() - m_smargin - m_spacing - first->minimumHeight())));
		} else {
			QRect result(m_fmargin, m_fmargin, s.width() - m_fmargin*2, s.height() - m_fmargin - m_spacing - m_smargin - second->minimumHeight());
			first->setGeometry(result);
			second->setGeometry(QRect(QPoint(m_smargin, result.y() + result.height() + m_spacing), QSize(s.width() - m_smargin*2, second->minimumHeight())));
		}
	} else if (first) {
		first->move(m_fmargin, m_fmargin);
		first->resize(s.width() - m_fmargin*2, s.height() - m_fmargin*2);
	} else if (second) {
		second->move(m_smargin, m_smargin);
		second->resize(s.width() - m_smargin*2, s.height() - m_smargin*2);
	}
}

