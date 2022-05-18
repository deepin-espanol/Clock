#include "stopsdelegate.h"

#include "src/models/stopsmodel.h"

#include <QIcon>
#include <QFont>
#include <QFontMetrics>
#include <QPainter>
#include <QPainterPath>
#include <QPalette>

QColor colorOf(StopsModel::Colors color, const QPalette &palette)
{
	switch (color) {
	case StopsModel::Colors::Normal: {
		return palette.color(QPalette::ColorGroup::Current, QPalette::ColorRole::Text);
	}
	case StopsModel::Colors::Best: {
		return palette.color(QPalette::ColorGroup::Current, QPalette::ColorRole::Highlight);
	}
	case StopsModel::Colors::Worst: {
		return Qt::red;
	}
	}
	return Qt::transparent;
}

StopsDelegate::StopsDelegate(AdvancedListView *p) : AdvancedItemDelegate(p)
{}

void StopsDelegate::paint(QPainter *painter, const QStyleOptionViewItem &opt, const QModelIndex &index) const
{
	painter->save();
	painter->setRenderHint(QPainter::Antialiasing);
	painter->setRenderHint(QPainter::SmoothPixmapTransform);

	QColor fg = colorOf(index.data(StopsModel::Roles::Color).value<StopsModel::Colors>(), opt.palette);
	QString tag = index.data(StopsModel::Roles::Number).toString();
	QSize s = sizeHint(opt, index);
	QPoint p = opt.rect.topLeft();
	QTextOption topt;
	p+= QPoint(5, 5);
	s -= QSize(5, 10);
	topt.setAlignment(Qt::AlignCenter);

	//Put the box.
	QPainterPath box;
	box.addRoundedRect(QRect(QPoint(0, 0) + p, s), 8, 8);
	painter->setClipPath(box);
	painter->fillPath(box, (opt.state & QStyle::State_MouseOver
								? opt.palette.color(QPalette::ColorGroup::Current, QPalette::ColorRole::Button)
								: opt.palette.color(QPalette::ColorGroup::Current, QPalette::ColorRole::Base)));

	painter->setPen(fg);
	s -= QSize(20, 0);

	//Calculate the text size.
	const int ts = opt.fontMetrics.horizontalAdvance(tag) + 10;

	//Put the tag
	painter->drawText(QRect(p, QSize(ts, 30)), tag, topt);

	//Put the delta
	painter->drawText(QRect(p + QPoint(ts, 0), s - QSize(ts, 0)), index.data(StopsModel::Roles::Text).toString(), topt);

	if (m_enableRemove && opt.state & QStyle::State_MouseOver) {
		QRect r(p.x() + s.width() - 5, p.y() + 5, 20, 20);
		QPoint n = r.topLeft();
		QPainterPath path;
		path.addEllipse(r);
		painter->fillPath(path, Qt::red);

		QPen lpen(Qt::white, 2);
		lpen.setCapStyle(Qt::PenCapStyle::RoundCap);
		painter->setPen(lpen);
		painter->drawLine(QPoint(5, 5) +n, QPoint(15, 15) +n);
		painter->drawLine(QPoint(5, 15) +n, QPoint(15, 5) +n);
	}

	painter->restore();
}

QSize StopsDelegate::sizeHint(const QStyleOptionViewItem &, const QModelIndex &/*index*/) const
{
	return QSize(m_parent->size().width() - 10, 40);
}

bool StopsDelegate::isHover(const QPoint &p, const QModelIndex &index, const QStyleOptionViewItem &opt) const
{
	QSize s = sizeHint(opt, index);
	return (p.x() >= 5 && p.y() >= 5 && p.x() <= s.width()-10 && p.y() <= s.height()-10);
}

QVariant StopsDelegate::isInDecoration(const QPoint &p, const QStyleOptionViewItem &opt) const
{
	QSize s = sizeHint(opt, QModelIndex());
	if (p.y() >= 0 && p.y() <= 0 && p.x() >= s.width() && p.x() <= s.height()) {
		return true;
	}
	return false;
}

QVariant StopsDelegate::handlePress(const QPoint &, const QModelIndex &index, const QStyleOptionViewItem &)
{
	m_currentRow = index.row();
	return -1;
}

QVariant StopsDelegate::handleRelease(const QPoint &p, const QModelIndex &index, const QStyleOptionViewItem &opt)
{
	return (isInDecoration(p, opt).toBool() ? index.row() : -1);
}
