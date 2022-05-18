#include "alarmslist.h"

#include "src/helpers.h"

AlarmsList::AlarmsList(QWidget *p)
	: AdvancedListView(p),
	  m_editor(new TimeEditor(this)),
	  m_model(this),
	  m_delegate(this),
	  m_popup(m_editor, this)
{
	setFlow(QListView::Flow::LeftToRight);
	setWrapping(true);
	setResizeMode(QListView::ResizeMode::Adjust);
	setSpacing(0);
	setFrameStyle(QFrame::Plain);
	setLineWidth(0);

	setAdvancedModel(&m_model);
	setAdvancedDelegate(&m_delegate);
	setEditable(true);

	connect(&m_model, &AlarmsModel::needsPopup, this, &AlarmsList::showPopup);
	connect(m_editor, SELECT<int, QString>::OVERLOAD_OF(&TimeEditor::finished), this, &AlarmsList::submit);
	connect(m_editor, SELECT<AlarmObject *>::OVERLOAD_OF(&TimeEditor::finished), this, [this](AlarmObject *obj) {m_model.addAlarm(obj);});
	connect(m_editor, SELECT<>::OVERLOAD_OF(&TimeEditor::finished), &m_popup, &InWinPopup::remove);
}

void AlarmsList::showPopup(int row, int delta, QString text)
{
	m_editor->setMode(false);
	m_row = row;
	m_editor->setDelta(delta);
	m_editor->setText(text);
	m_popup.popup();
}

void AlarmsList::submit(int delta, QString text)
{
	m_model.changeData(m_row, delta, text);
	m_row = -1;
}

InWinPopup &AlarmsList::popup()
{
	return m_popup;
}

TimeEditor *AlarmsList::editor()
{
	return m_editor;
}

QString AlarmsList::nextAlarm()
{
	return m_model.getNextAlarm();
}

AlarmsModel &AlarmsList::getModel()
{
	return m_model;
}
