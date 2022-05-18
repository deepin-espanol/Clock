#include "stopslist.h"

StopsList::StopsList(QWidget *p)
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
	setRemovable(true);
	setHovers(true);
}

StopsModel &StopsList::getModel()
{
	return m_model;
}
