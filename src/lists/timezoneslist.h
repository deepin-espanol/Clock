#ifndef TIMEZONESLIST_H
#define TIMEZONESLIST_H

#include "src/models/timezonesmodel.h"
#include "src/delegates/timezonesdelegate.h"

#include "src/widgets/advancedclasses.h"

#include <QTimer>

/**
 * @brief Widget implementing a list of timezones.
 */
class TimeZonesList : public AdvancedListView
{
	Q_OBJECT
public:
	explicit TimeZonesList(QWidget *p = nullptr);
	~TimeZonesList();

	/**
	 * @brief Get the internal model of the list.
	 * @return Internal model.
	 */
	TimeZonesModel &getModel();

private:
	TimeZonesModel m_model;
	TimeZonesDelegate m_delegate;
	//Used to update UI clocks.
	QTimer m_timer;
};

#endif // TIMEZONESLIST_H
