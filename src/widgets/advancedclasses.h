#ifndef ADVANCEDCLASSES_H
#define ADVANCEDCLASSES_H

#include <QListView>
#include <QStyledItemDelegate>

class AdvancedListView;
class AdvancedItemDelegate;

class AdvancedItemModel : public QAbstractItemModel
{
	Q_OBJECT
public:
	explicit AdvancedItemModel(QObject *p = nullptr);

	virtual void clear() = 0;
	virtual void handlePress(QVariant v) = 0;
	virtual void handleRelease(QVariant v) = 0;
	virtual void handleStateChange();

Q_SIGNALS:
	void dataUpdated();

private:
	friend class AdvancedListView;
};

class AdvancedItemDelegate : public QStyledItemDelegate
{
public:
	explicit AdvancedItemDelegate(AdvancedListView *p = nullptr);

	Q_INVOKABLE void widthUpdate(int upd, int item, bool notAlone);

	virtual void setLocked(bool lock);
	virtual void setEditEnabled(bool enable);
	virtual void setRemovable(bool enable);

protected:
	bool m_enableEdit = false;
	bool m_enableRemove = false;

	bool m_enableSplit = false;
	int m_currentRow = -1;
	int m_hoveredRow = -1;
	bool m_locked = false;
	int m_width = 0;
	int m_largest = 0;
	int columnCount = 1;

	AdvancedListView *m_parent;

	virtual bool isHover(const QPoint &p, const QModelIndex &index, const QStyleOptionViewItem &opt) const;
	virtual QVariant isInDecoration(const QPoint &p, const QStyleOptionViewItem &opt) const;
	virtual QVariant handlePress(const QPoint &p, const QModelIndex &index, const QStyleOptionViewItem &opt);
	virtual QVariant handleRelease(const QPoint &p, const QModelIndex &index, const QStyleOptionViewItem &opt);

private:
	friend class AdvancedListView;
};

class AdvancedListView : public QListView
{
	Q_OBJECT
public:
	explicit AdvancedListView(QWidget *p = nullptr);

	virtual void setEditable(bool enable);
	virtual bool isEditEnabled();
	virtual void setRemovable(bool enable);
	virtual bool isRemovable();
	virtual void setHovers(bool enable);
	virtual bool hasHovers();

	void setAdvancedModel(AdvancedItemModel *model);
	void setAdvancedDelegate(AdvancedItemDelegate *delegate);
	void refresh();

	QStyleOptionViewItem getViewOptions();
	int getLargest();

protected:
	void mouseMoveEvent(QMouseEvent *e) override;
	void mousePressEvent(QMouseEvent *e) override;
	void mouseReleaseEvent(QMouseEvent *e) override;
	void resizeEvent(QResizeEvent *e) override;

	bool m_enableEdit = false;
	bool m_enableRemove = false;
	bool m_enableHovers = false;

private:
	AdvancedItemModel *m_model = nullptr;
	AdvancedItemDelegate *m_delegate = nullptr;

	bool m_updated = false;
	int m_oldWidth = -1;
};

inline QPoint mapPointFromRect(const QPoint &p, const QRect &r)
{
	return p - r.topLeft();
}


#endif // ADVANCEDCLASSES_H
