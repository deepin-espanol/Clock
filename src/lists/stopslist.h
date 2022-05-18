#ifndef STOPSLIST_H
#define STOPSLIST_H

#include "src/widgets/advancedclasses.h"

#include "src/models/stopsmodel.h"
#include "src/delegates/stopsdelegate.h"

/**
 * @brief Widget implementing a list of stops, or 'laps'.
 */
class StopsList : public AdvancedListView
{
	Q_OBJECT
public:
	explicit StopsList(QWidget *p = nullptr);

	/**
	 * @brief Get the internal model of the list.
	 * @return The internal StopsModel.
	 */
	StopsModel &getModel();

private:
	StopsModel m_model;
	StopsDelegate m_delegate;

	int m_row;
};

#endif // STOPSLIST_H
