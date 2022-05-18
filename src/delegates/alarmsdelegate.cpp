#include "alarmsdelegate.h"

#include "src/models/alarmsmodel.h"
#include "src/helpers.h"

#include <QIcon>
#include <QFont>
#include <QFontMetrics>
#include <QPainter>
#include <QPainterPath>
#include <QApplication>
#include <QListView>
#include <QTime>
#include <QLocale>
#include <DApplicationHelper>

#define IWIDTH 320

AlarmsDelegate::AlarmsDelegate(AdvancedListView *p) : AdvancedItemDelegate(p), m_clock(p)
{}

void AlarmsDelegate::paint(QPainter *painter, const QStyleOptionViewItem &opt, const QModelIndex &index) const
{
	painter->save();
	painter->setRenderHint(QPainter::Antialiasing);
	painter->setRenderHint(QPainter::SmoothPixmapTransform);

	const QString title = index.data(AlarmsModel::Roles::Name).toString();
	const bool enabled = index.data(AlarmsModel::Roles::Enabled).toBool();
	const QTime time = index.data(AlarmsModel::Roles::Time).toTime();
	const int days = index.data(AlarmsModel::Roles::Days).toInt();

	QSize s = sizeHint(opt, index);
	QPoint p = opt.rect.topLeft();

	//Setup for title
	QFont major = opt.font;
	major.setBold(true);
	major.setPixelSize(17);
	QFontMetrics mm(major);
	const int theight = mm.height();

	//Setup hints
	p += QPoint(10, 10);
	s -= QSize(10, 10);

	//Put the box.
	QPainterPath box;
	box.addRoundedRect(QRect(p, s), 8, 8);
	painter->setClipPath(box);
	painter->fillPath(box, opt.palette.color(QPalette::ColorGroup::Current, QPalette::ColorRole::Background));

	p += QPoint(0, theight+10);

	//Draw title (name)
	QTextOption topt;
	topt.setAlignment(Qt::AlignLeft | Qt::AlignTop);
	topt.setWrapMode(QTextOption::WrapMode::WordWrap);
	const QSize ts(s.width() - 20 - s.height(), theight);

	painter->setFont(major);
	painter->setPen(QPen(opt.palette.color(QPalette::ColorGroup::Current, QPalette::ColorRole::BrightText), 3));
	painter->drawText(QRect(QPoint(5, 5) + p, ts), title, topt);

	//Draw the time
	const int cy = theight;
	painter->setFont(major);
	painter->drawText(QRect(QPoint(5, cy) + p, ts), time.toString("hh:mm"), topt);

	//Draw the days enabled
	topt.setAlignment(Qt::AlignCenter);
	QLocale locale = QLocale::system();
	int i = 0;
	int px = 5;
	while (i < 7) {
		const QString text = locale.dayName(i+1, QLocale::NarrowFormat);
		const QSize ds = QSize(opt.fontMetrics.horizontalAdvance(text)+10, opt.fontMetrics.height()+10);
		const QRect targeted(QPoint(px, cy + mm.height() + 5) + p, ds);

		box.clear();
		box.addRoundedRect(targeted, opt.fontMetrics.height()/2+5, opt.fontMetrics.height()/2+5);

		if (CHECK_BIT(days, i)) {
			painter->fillPath(box, opt.palette.color(QPalette::ColorGroup::Current, QPalette::ColorRole::Highlight));
			painter->setPen(opt.palette.color(QPalette::ColorGroup::Current, QPalette::ColorRole::HighlightedText));
		} else {
			QPen rpen(opt.palette.color(QPalette::ColorGroup::Current, QPalette::ColorRole::Highlight), 2);
			painter->setPen(rpen);
			painter->drawPath(box);
			painter->setPen(opt.palette.color(QPalette::ColorGroup::Current, QPalette::ColorRole::Text));
		}

		painter->setFont(opt.font);
		painter->drawText(targeted, text, topt);

		px += ds.width()+5;
		i++;
	}

	//Put the clock
	s -= QSize(0, 40);
	m_clock.renderSimpleClock(painter, opt.palette, QRect(p.x() + s.width() - s.height() + 5, p.y() + 5, s.height() - 10, s.height()-10), time, Dtk::Gui::DGuiApplicationHelper::instance()->themeType() == Dtk::Gui::DGuiApplicationHelper::DarkType);

	//Generate title decoration
	p -= QPoint(0, theight+10);
	s += QSize(10, 40);

	//The remove button
	if (m_enableRemove) {
		box.clear();
		QRect decoRect = QRect(QPoint(5, 5) + p, QSize(theight+5, theight+5));

		if (index.row() == m_currentRow) {
			box.addRoundedRect(decoRect, 5, 5);
		} else {
			box.addRoundedRect(decoRect, decoRect.width()/2, decoRect.height()/2);
		}
		painter->fillPath(box, Qt::red);

		//Put the bar
		QPen pen(Qt::white, 3);
		pen.setCapStyle(Qt::PenCapStyle::RoundCap);
		painter->setPen(pen);
		const int s = (decoRect.width()-10)/2;
		QPoint begin = decoRect.center() - QPoint(s, 0);
		painter->drawLine(begin, begin + QPoint(s*2, 0));
	} else if (m_enableEdit) {
		box.clear();
		QRect decoRect = QRect(QPoint(5, 5) + p, QSize(theight+5, theight+5));
		box.addRoundedRect(decoRect, decoRect.width()/2, decoRect.height()/2);

		QColor c = opt.palette.color(QPalette::ColorGroup::Current, QPalette::ColorRole::Base);
		if (index.row() == m_currentRow && m_isInIcon) {
			c = opt.palette.color(QPalette::ColorGroup::Current, QPalette::ColorRole::Highlight);
		}
		painter->fillPath(box, c);

		//Put the stylo
		QPen pen(opt.palette.color(QPalette::ColorGroup::Current, QPalette::ColorRole::ButtonText), 1);
		pen.setCapStyle(Qt::PenCapStyle::RoundCap);
		painter->setPen(pen);

		const float r = float(theight)/14;
		const QPoint t = decoRect.topLeft();

		box.clear();
		box.addPolygon(QPolygon({QPoint(12, 2)*r+t, QPoint(14, 4)*r+t, QPoint(13, 5)*r+t, QPoint(11, 3)*r+t, QPoint(12, 2)*r+t}));
		box.addPolygon(QPolygon({QPoint(10, 4)*r+t, QPoint(12, 6)*r+t, QPoint(5, 13)*r+t, QPoint(3, 11)*r+t, QPoint(10, 4)*r+t}));
		box.addPolygon(QPolygon({QPoint(2, 12)*r+t, QPoint(2, 14)*r+t, QPoint(4, 14)*r+t, QPoint(2, 12)*r+t}));
		painter->fillPath(box, opt.palette.color(QPalette::ColorGroup::Current, QPalette::ColorRole::ButtonText));
	}

	//The enable/disable button
	if (m_enableEdit) {
		box.clear();
		QPoint result = p + QPoint(s.width()-theight*3-5, 5);

		box.addRoundedRect(QRect(result, QSize(theight*3 -15, theight+5)), 7, 7);
		painter->fillPath(box, opt.palette.color(QPalette::ColorGroup::Current, QPalette::ColorRole::Button));

		box.clear();
		QColor c = opt.palette.color(QPalette::ColorGroup::Current, QPalette::ColorRole::Highlight);
		if (enabled) {
			result.setX(s.width() - theight*2 + p.x());
		} else {
			c = opt.palette.color(QPalette::ColorGroup::Current, QPalette::ColorRole::ButtonText);
		}
		box.addRoundedRect(QRect(result, QSize(theight+5, theight+5)), 7, 7);
		painter->fillPath(box, c);
	}

	painter->restore();
}

QSize AlarmsDelegate::sizeHint(const QStyleOptionViewItem &opt, const QModelIndex &) const
{

	QFont major = opt.font;
	major.setBold(true);
	major.setPixelSize(15);
	const QFontMetrics mm(major);
	const int v = 20 + 10 + mm.height() + 30 + mm.height() + 25;

	const QLocale locale = QLocale::system();
	int i = 0;
	int px = 5 + v - 40;
	while (i < 7) {
		px += opt.fontMetrics.horizontalAdvance(locale.dayName(i+1, QLocale::NarrowFormat))+10+5;
		i++;
	}
	QSize local = QSize(px, v);

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

bool AlarmsDelegate::isInIcon(const QPoint &p, const QStyleOptionViewItem &opt) const
{
	const int theight = opt.fontMetrics.height()+10;
	return (p.x() >= 5 && p.y() >= 5 && p.x() < theight && p.y() < theight);
}

QVariant AlarmsDelegate::isInDecoration(const QPoint &p, const QStyleOptionViewItem &opt) const
{
	const int theight = opt.fontMetrics.height()+5;
	if (m_enableRemove && isInIcon(p, opt)) {
		return QVariant::fromValue(AlarmEvent {Roles::Remove, -1, 0});
	}
	if (m_enableEdit) {
		//Look for the edit button
		if (!m_enableRemove && isInIcon(p, opt)) {
			return QVariant::fromValue(AlarmEvent {Roles::Edit, -1, 0});
		}

		//Look for the switch button
		QSize s = sizeHint(opt, QModelIndex());
		if (p.x() >= (s.width() - theight*3 - 5) && p.y() >= 5 && p.x() < (s.width()-5) && p.y() < theight+5) {
			return QVariant::fromValue(AlarmEvent {Roles::Switch, -1, 0});
		}

		//Look for the days
		QLocale locale = QLocale::system();
		int i = 0;
		int px = 5;
		while (i < 7) {
			const QSize ds = QSize(opt.fontMetrics.horizontalAdvance(locale.dayName(i+1, QLocale::NarrowFormat))+10, opt.fontMetrics.height()+10);
			const QRect t(QPoint(px, theight*2 + 10 + opt.fontMetrics.height() + 5), ds);

			if (p.x() >= t.x() && p.y() >= t.y() && p.x() < (t.x()+t.width()) && p.y() < (t.y()+t.height())) {
				return QVariant::fromValue(AlarmEvent {Roles::Day, -1, i});
			}

			px += ds.width()+5;
			i++;
		}
	}
	return QVariant::fromValue(AlarmEvent {None, -1, 0});
}

QVariant AlarmsDelegate::handleRelease(const QPoint &p, const QModelIndex &index, const QStyleOptionViewItem &opt)
{
	m_currentRow = -1;
	m_isInIcon = false;
	AlarmEvent e = isInDecoration(p - QPoint(10, 10), opt).value<AlarmEvent>();
	if (e.type != Roles::None) {
		e.row = index.row();
		return QVariant::fromValue(e);
	}
	return QVariant::fromValue(AlarmEvent {Roles::None, -1, 0});
}

QVariant AlarmsDelegate::handlePress(const QPoint &p, const QModelIndex &index, const QStyleOptionViewItem &opt)
{
	m_currentRow = index.row();
	m_isInIcon = isInIcon(p, opt);
	return QVariant();
}
