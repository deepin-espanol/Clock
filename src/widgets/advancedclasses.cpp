#include "advancedclasses.h"

#include <QResizeEvent>
#include <QMouseEvent>

AdvancedItemModel::AdvancedItemModel(QObject *p) : QAbstractItemModel(p)
{}

void AdvancedItemModel::handleStateChange()
{
	Q_EMIT dataChanged(createIndex(0, 0), createIndex(rowCount()-1, columnCount()-1));
}

AdvancedItemDelegate::AdvancedItemDelegate(AdvancedListView *p)
	: QStyledItemDelegate(p),
	  m_parent(p)
{}

void AdvancedItemDelegate::setEditEnabled(bool enable)
{
	m_enableEdit = enable;
}

void AdvancedItemDelegate::setRemovable(bool enable)
{
	m_enableRemove = enable;
}

void AdvancedItemDelegate::widthUpdate(int upd, int item, bool notAlone)
{
	m_width = upd;
	m_largest = item;
	m_enableSplit = ((m_width - m_largest) >= m_largest) && notAlone;

	//Determine the number of columns.
	if (m_enableSplit) {
		int i = 3;
		while (m_width - (i*(m_largest+10)) > m_largest) {
			i++;
		}
		columnCount = i-1;
	} else {
		columnCount = 1;
	}
}

int AdvancedListView::getLargest()
{
	if (!m_updated) {
		return m_oldWidth;
	}
	int i = 0;
	int w = -1;
	m_delegate->setLocked(true);
	while (i < m_model->rowCount()) {
		QSize s = sizeHintForIndex(m_model->index(i, 0));
		if (w < s.width()) {
			w = s.width();
		}
		i++;
	}
	m_delegate->setLocked(false);
	m_oldWidth = w;
	return w;
}

void AdvancedItemDelegate::setLocked(bool lock)
{
	m_locked = lock;
}

bool AdvancedItemDelegate::isHover(const QPoint &, const QModelIndex &, const QStyleOptionViewItem &) const
{
	return true;
}

QVariant AdvancedItemDelegate::isInDecoration(const QPoint &, const QStyleOptionViewItem &) const
{
	return false;
}

QVariant AdvancedItemDelegate::handlePress(const QPoint &p, const QModelIndex &index, const QStyleOptionViewItem &opt)
{
	if (isInDecoration(p, opt).toBool()) {
		m_currentRow = index.row();
	}
	return -1;
}

QVariant AdvancedItemDelegate::handleRelease(const QPoint &p, const QModelIndex &index, const QStyleOptionViewItem &opt)
{
	m_currentRow = -1;
	if (isInDecoration(p, opt).toBool()) {
		return index.row();
	}
	return -1;
}

AdvancedListView::AdvancedListView(QWidget *p) : QListView(p)
{
}

void AdvancedListView::setAdvancedModel(AdvancedItemModel *model)
{
	setModel(model);
	m_model = model;
	connect(m_model, &AdvancedItemModel::dataUpdated, [this]() {m_updated = true; QSize s = size(); resize(s - QSize(1, 1)); resize(s);});
	repaint();
}

void AdvancedListView::setAdvancedDelegate(AdvancedItemDelegate *delegate)
{
	setItemDelegate(delegate);
	m_delegate = delegate;
	m_delegate->setEditEnabled(m_enableEdit);
	m_delegate->setRemovable(m_enableRemove);
}

void AdvancedListView::refresh()
{
	//Will make it all get repaint.
	m_model->handleStateChange();
}

bool AdvancedListView::isEditEnabled()
{
	return m_enableEdit;
}

void AdvancedListView::setEditable(bool enable)
{
	m_enableEdit = enable;
	m_delegate->setEditEnabled(enable);
	m_model->handleStateChange();
	//repaint();
}

bool AdvancedListView::isRemovable()
{
	return m_enableRemove;
}

void AdvancedListView::setRemovable(bool enable)
{
	m_enableRemove = enable;
	m_delegate->setRemovable(enable);
	m_model->handleStateChange();
	//repaint();
}

bool AdvancedListView::hasHovers()
{
	return m_enableHovers;
}

void AdvancedListView::setHovers(bool enable)
{
	m_enableHovers = enable;
	setMouseTracking(enable);
}

void AdvancedListView::resizeEvent(QResizeEvent *e)
{
	m_delegate->widthUpdate(e->size().width() - 25, getLargest(), m_model->rowCount() != 1);
	QListView::resizeEvent(e);
}

void AdvancedListView::mouseMoveEvent(QMouseEvent *e)
{
	if (m_enableHovers) {
		const QModelIndex index = indexAt(e->pos());
		m_delegate->m_hoveredRow = m_delegate->isHover(mapPointFromRect(e->pos(), visualRect(index)), index, getViewOptions()) ? index.row() : -1;
	}
}

void AdvancedListView::mousePressEvent(QMouseEvent *e)
{
	if (m_enableEdit || m_enableRemove) {
		QModelIndex index = indexAt(e->pos());
		m_model->handlePress(m_delegate->handlePress(mapPointFromRect(e->pos(), visualRect(index)), index, getViewOptions()));
	}
	return QListView::mousePressEvent(e);
}

void AdvancedListView::mouseReleaseEvent(QMouseEvent *e)
{
	if (m_enableEdit || m_enableRemove) {
		QModelIndex index = indexAt(e->pos());
		m_model->handleRelease(m_delegate->handleRelease(mapPointFromRect(e->pos(), visualRect(index)), index, getViewOptions()));
	}
	return QListView::mouseReleaseEvent(e);
}

QStyleOptionViewItem AdvancedListView::getViewOptions()
{
	return viewOptions();
}
