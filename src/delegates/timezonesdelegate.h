#ifndef TIMEZONESDELEGATE_H
#define TIMEZONESDELEGATE_H

#include "src/utils/clock.h"
#include "src/widgets/advancedclasses.h"

#include <QColor>

//Colors used depending on the theme.
static const QColor TZBackgroundColors[2] = {Qt::black, QColor("#f0f0f0")};
static const QColor TZTextColors[2] = {QColor("#eee"), QColor("404040")};
static const QColor TZTitleColors[6] = {QColor("#0430dd"), QColor("#fff891"), QColor("#b9fcff"), QColor("#21d5ff"), QColor("#ffbba6"), QColor("#4843ff")};

/**
 * @brief Implementation of the UI of the clocks depending on the TZ.
 */
class TimeZonesDelegate : public AdvancedItemDelegate
{
public:
	explicit TimeZonesDelegate(AdvancedListView *parent = nullptr);

	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	QSize sizeHint(const QStyleOptionViewItem &, const QModelIndex &) const override;
	QVariant isInDecoration(const QPoint &p, const QStyleOptionViewItem &opt) const override;

	/**
	 * @brief Facility to calculate the item size while having many columns.
	 */
	QSize calculateSizeHintOf(const QStyleOptionViewItem &, QString title, QString comment) const;

private:
	//Used to render the clock in the items.
	Clock m_clock;
};

#endif // TIMEZONESDELEGATE_H
