#include "stopwatch.h"

#include "dualwidgetcontainer.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>

StopWatch::StopWatch(QWidget *parent)
	: QWidget(parent),
	  widget(new StopWatchWidget(this)),
	  m_list(this)
{
	VerticalDWC *dwc = new VerticalDWC(this);
	QPushButton *btn = new QPushButton(tr("Clear"), this);
	QHBoxLayout *lay = new QHBoxLayout;

	btn->setMinimumHeight(30);
	dwc->setMarginsOfFirst(0);
	dwc->setMarginsOfSecond(5);
	dwc->setSpacing(5);
	dwc->setFixedWidth(200);
	dwc->setWidgets(&m_list, btn);

	lay->addWidget(widget);
	lay->addWidget(dwc);
	lay->setMargin(0);
	lay->setSpacing(0);
	setLayout(lay);

	connect(widget, &StopWatchWidget::deltaMark, &m_list.getModel(), &StopsModel::addData, Qt::DirectConnection);
	connect(widget, &StopWatchWidget::reset, &m_list.getModel(), &StopsModel::clear);
	connect(btn , &QPushButton::clicked, &m_list.getModel(), &StopsModel::clear);
}
