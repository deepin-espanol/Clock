#ifndef ALARMSDELEGATE_H
#define ALARMSDELEGATE_H

#include "src/utils/clock.h"
#include "src/widgets/advancedclasses.h"

/**
 * @brief Implementation of the alarms' UI delegate.
 */
class AlarmsDelegate : public AdvancedItemDelegate
{
public:
	explicit AlarmsDelegate(AdvancedListView *parent = nullptr);

	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	QSize sizeHint(const QStyleOptionViewItem &, const QModelIndex &) const override;

	/**
	 * @brief Storage roles describing the available data.
	 */
	enum Roles {
		None = 0,
		Remove,
		Switch,
		Day,
		Edit
	};

	/**
	 * @brief Used to know if the point is in the icon of the decoration.
	 */
	bool isInIcon(const QPoint &p, const QStyleOptionViewItem &opt) const;

protected:
	QVariant handleRelease(const QPoint &p, const QModelIndex &index, const QStyleOptionViewItem &opt) override;
	QVariant handlePress(const QPoint &p, const QModelIndex &index, const QStyleOptionViewItem &opt) override;
	QVariant isInDecoration(const QPoint &p, const QStyleOptionViewItem &opt) const override;

	bool m_isInIcon = false;

private:
	//Used to render clocks' UI.
	Clock m_clock;
};

/**
 * @brief Structure containing data about an Alarm.
 */
struct AlarmEvent {
	AlarmsDelegate::Roles type;
	int row;
	int additional;
};

Q_DECLARE_METATYPE(AlarmEvent)

#endif // ALARMSDELEGATE_H
