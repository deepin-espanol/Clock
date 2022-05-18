#ifndef STOPSDELEGATE_H
#define STOPSDELEGATE_H

#include "src/widgets/advancedclasses.h"

#include "src/models/stopsmodel.h"

/**
 * @brief Implementation the stops of the stopwatch, or 'laps'.
 */
class StopsDelegate : public AdvancedItemDelegate
{
public:
	explicit StopsDelegate(AdvancedListView *parent = nullptr);

	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	QSize sizeHint(const QStyleOptionViewItem &, const QModelIndex &) const override;

	QVariant isInDecoration(const QPoint &p, const QStyleOptionViewItem &opt) const override;

protected:
	bool isHover(const QPoint &p, const QModelIndex &index, const QStyleOptionViewItem &opt) const override;
	QVariant handlePress(const QPoint &p, const QModelIndex &index, const QStyleOptionViewItem &opt) override;
	QVariant handleRelease(const QPoint &p, const QModelIndex &index, const QStyleOptionViewItem &opt) override;
};

QColor colorOf(StopsModel::Colors color, const QPalette &palette);

#endif // STOPSDELEGATE_H
