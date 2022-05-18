#include "worldclockswidget.h"

#include "src/helpers.h"

#include <QTime>
#include <QPainter>
#include <DApplicationHelper>
#include <QSpacerItem>
#include <QVBoxLayout>
#include <QPalette>
#include <QPaintEvent>

BigClock::BigClock(QWidget *p)
	: QWidget(p),
	  m_clock(this, true)
{
	m_timer.setInterval(1000);
	connect(&m_timer, &QTimer::timeout, [this](){repaint();});
	m_timer.start();
}

void BigClock::paintEvent(QPaintEvent *e)
{
	QWidget::paintEvent(e);
	QPainter p(this);
	m_clock.renderFullClock(&p, QRect(QPoint((width()-clockSize.width())/2, (height()-clockSize.height())/2), clockSize), QTime::currentTime(),
							Dtk::Gui::DGuiApplicationHelper::instance()->themeType() == Dtk::Gui::DGuiApplicationHelper::ColorType::DarkType);
}

WorldClocksWidget::WorldClocksWidget(QWidget *parent)
	: QWidget(parent),
	  m_clock(new BigClock(this)),
	  m_list(new TimeZonesList(this)),
	  m_addBtn(new QPushButton(this)),
	  m_removeBtn(new QPushButton(this)),
	  chooser(new TimeZoneChooser(this))
{
	m_clock->setFixedSize(clockSize);
	m_addBtn->setText(tr("Add a timezone"));
	m_removeBtn->setText(tr("Edit"));

	QHBoxLayout *lay = new QHBoxLayout;
	QVBoxLayout *subL = new QVBoxLayout;

	subL->addWidget(m_clock);
	subL->addWidget(m_addBtn);
	subL->addWidget(m_removeBtn);
	subL->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));
	subL->setMargin(5);

	lay->addLayout(subL);
	lay->addWidget(m_list);
	lay->setSpacing(15);
	lay->setMargin(0);
	setLayout(lay);

	connect(m_addBtn, &QPushButton::clicked, this, [this]() {chooser->show();});
	connect(m_removeBtn, &QPushButton::clicked, this, [this]() {
		m_list->setRemovable(!m_list->isRemovable());
		m_removeBtn->setText(m_list->isRemovable() ? tr("Finish") : tr("Edit"));
	});
	connect(chooser, SELECT<const installer::ZoneInfo>::OVERLOAD_OF(&TimeZoneChooser::confirmed), [this](const installer::ZoneInfo zone) {
		QTimeZone tz(zone.timezone.toLocal8Bit());
		m_list->getModel().addInfo(zone);
		m_list->repaint();
	});
}

void WorldClocksWidget::paintEvent(QPaintEvent *e)
{
	QPainter p(this);
	p.fillRect(e->rect(), palette().color(QPalette::ColorGroup::Current, QPalette::ColorRole::Base));
}
