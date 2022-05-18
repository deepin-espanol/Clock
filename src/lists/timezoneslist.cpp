#include "timezoneslist.h"

#include "src/helpers.h"

#include <QResizeEvent>
#include <QScrollBar>

TimeZonesList::TimeZonesList(QWidget *p)
	: AdvancedListView(p),
	  m_model(this),
	  m_delegate(this)
{
	setFlow(QListView::Flow::LeftToRight);
	setWrapping(true);
	setResizeMode(QListView::ResizeMode::Adjust);
	setSpacing(0);
	setFrameStyle(QFrame::Plain);
	setLineWidth(0);

	setAdvancedModel(&m_model);
	setAdvancedDelegate(&m_delegate);

	connect(&m_timer, &QTimer::timeout, this, &TimeZonesList::refresh);
	connect(&m_model, &AdvancedItemModel::dataUpdated, this, SELECT<>::OVERLOAD_OF(&TimeZonesList::repaint));

	m_timer.setInterval(500); //Update the 1/2 mins to ensure it is fresh enugh, doing it all the time uses more resources while it is not that important.
	m_timer.start();
}

TimeZonesList::~TimeZonesList()
{
	m_timer.stop();
}

TimeZonesModel &TimeZonesList::getModel()
{
	return m_model;
}
