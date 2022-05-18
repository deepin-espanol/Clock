#ifndef ALARMSLIST_H
#define ALARMSLIST_H

#include "src/models/alarmsmodel.h"
#include "src/delegates/alarmsdelegate.h"
#include "src/widgets/inwinpopup.h"
#include "src/widgets/timeeditor.h"
#include "src/widgets/advancedclasses.h"

/**
 * @brief Widget implementing a list of alarms.
 */
class AlarmsList : public AdvancedListView
{
	Q_OBJECT
public:
	explicit AlarmsList(QWidget *p = nullptr);

	/**
	 * @brief Popup widget used to show the editor.
	 * @return The popup.
	 */
	InWinPopup &popup();
	/**
	 * @brief Get the editor used in the popup.
	 * @return
	 */
	TimeEditor *editor();
	/**
	 * @brief Get the internal model of the list.
	 * @return Internal model.
	 */
	AlarmsModel &getModel();

	QString nextAlarm();

protected:
	void showPopup(int row, int delta, QString text);
	void submit(int delta, QString t);

private:
	TimeEditor *m_editor;

	AlarmsModel m_model;
	AlarmsDelegate m_delegate;
	InWinPopup m_popup;

	int m_row;
};

#endif // ALARMSLIST_H
