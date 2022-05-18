#include "timezonesdelegate.h"

#include "src/models/timezonesmodel.h"

#include <QIcon>
#include <QFont>
#include <QFontMetrics>
#include <QPainter>
#include <QPainterPath>
#include <QApplication>
#include <QListView>

TimeZonesDelegate::TimeZonesDelegate(AdvancedListView *p) : AdvancedItemDelegate(p), m_clock(p)
{}

void TimeZonesDelegate::paint(QPainter *painter, const QStyleOptionViewItem &opt, const QModelIndex &index) const
{
	painter->save();
	painter->setRenderHint(QPainter::Antialiasing);
	painter->setRenderHint(QPainter::SmoothPixmapTransform);

	QString title = index.data(TimeZonesModel::Roles::Name).toString();
	QString comment = index.data(TimeZonesModel::Roles::Diff).toString();
	QColor highlight = TZTitleColors[indexOfColor((TimeZonesModel::Colors)index.data(TimeZonesModel::Color).toInt())];
	QColor bg = TZBackgroundColors[indexOfBackground(index.data(TimeZonesModel::DayTime).toBool())];
	QColor text = TZTextColors[indexOfBackground(index.data(TimeZonesModel::DayTime).toBool())];
	QSize s = sizeHint(opt, index);
	QPoint p = opt.rect.topLeft();

	p += QPoint(10, 10);
	s -= QSize(10, 10);

	//Put the box.
	QPainterPath box;
	box.addRoundedRect(QRect(QPoint(0, 0) + p, s), 8, 8);
	painter->setClipPath(box);

	painter->fillPath(box, bg);

	//Setup for title
	QFont major = opt.font;
	major.setBold(true);
	major.setPixelSize(15);
	QFontMetrics mm(major);
	int theight = mm.height();

	//Generate title decoration
	QPainterPath titleBg;
	QRect decoRect = QRect(QPoint(5, 5) + p, QSize(theight+5, theight+5));

	//The system's locale MUST NOT be removed.
	if (m_enableRemove && !index.data(TimeZonesModel::Roles::IsSystem).toBool()) {
		if (index.row() == m_currentRow) {
			titleBg.addRoundedRect(decoRect, 5, 5);
		} else {
			titleBg.addRoundedRect(decoRect, decoRect.width()/2, decoRect.height()/2);
		}
		painter->fillPath(titleBg, Qt::red);

		//Put the bar
		QPen pen(Qt::white, 3);
		pen.setCapStyle(Qt::PenCapStyle::RoundCap);
		painter->setPen(pen);
		const int s = (decoRect.width()-10)/2;
		QPoint begin = decoRect.center() - QPoint(s, 0);
		painter->drawLine(begin, begin + QPoint(s*2, 0));
	} else {
		titleBg.addRoundedRect(decoRect, 5, 5);
		painter->fillPath(titleBg, highlight);
	}

	//Draw title
	painter->setFont(major);
	painter->setPen(QPen(text, 3));
	painter->drawText(QPoint(20 + theight, 10 + theight/3*2) + p, title);

	//Draw the comment
	int cy = theight+30;
	painter->setFont(opt.font);
	painter->drawText(QPoint(5, cy) + p, comment);

	//Clean time text
	QTime time = index.data(TimeZonesModel::Time).toTime();
	painter->setFont(major);
	painter->drawText(QPoint(5, cy + 5 + mm.height()/3*2) + p, time.toString("hh:mm"));

	//Put the clock
	m_clock.renderSimpleClock(painter, opt.palette, QRect(p.x() + s.width() - s.height() + 5, p.y() + 5, s.height() - 10, s.height()-10), time, index.data(TimeZonesModel::DayTime).toBool() != true);

	painter->restore();
}

QSize TimeZonesDelegate::sizeHint(const QStyleOptionViewItem &opt, const QModelIndex &index) const
{
	QSize local = calculateSizeHintOf(opt, index.data(TimeZonesModel::Roles::Name).toString(), index.data(TimeZonesModel::Roles::Diff).toString());
	if (m_locked) {
		return local;
	}
	if (m_enableSplit) {
		if (((m_width)/columnCount) > m_largest) {
			local.setWidth((m_width)/columnCount);
		} else {
			local.setWidth(m_largest);
		}
	} else {
		if (local.width() < (m_width)) {
			local.setWidth(m_width);
		}
	}
	return local;
}

QSize TimeZonesDelegate::calculateSizeHintOf(const QStyleOptionViewItem &opt, QString title, QString comment) const
{
	QFont major = opt.font;
	major.setBold(true);
	major.setPixelSize(15);
	QFontMetrics mm(major);

	int tw = mm.horizontalAdvance(title) + mm.height() + 20, cw = opt.fontMetrics.horizontalAdvance(comment);
	int max = ((tw > cw) ? tw : cw );

	int h = mm.height() + 10 + 30 + opt.fontMetrics.height();

	return QSize(15 + max + h, h+10);
}

QVariant TimeZonesDelegate::isInDecoration(const QPoint &p, const QStyleOptionViewItem &opt) const
{
	QFont major = opt.font;
	QFontMetrics mm(major);
	int lim = mm.height()+10;
	return (p.x() >= 5 && p.y() >= 5 && p.x() < lim && p.y() < lim);
}
