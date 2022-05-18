#include "alarmswidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <DPushButton>
#include <QLabel>
#include <DNotifySender>

DWIDGET_USE_NAMESPACE
using namespace Dtk::Core::DUtil;

AlarmsWidget::AlarmsWidget(QWidget *parent)
	: QWidget(parent),
	  m_list(this)
{
	QWidget *bar = new QWidget(this);
	QHBoxLayout *barLayout = new QHBoxLayout;
	DPushButton *btn = new DPushButton(this);
	DPushButton *add = new DPushButton(this);
	QLabel *label = new QLabel(this);

	btn->setText(tr("Edit"));
	add->setText(tr("Add"));
	label->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
	label->setAlignment(Qt::AlignCenter);

	barLayout->addWidget(add);
	barLayout->addWidget(label);
	barLayout->addWidget(btn);
	barLayout->setMargin(5);
	bar->setLayout(barLayout);
	bar->setMaximumHeight(40);

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(&m_list);
	mainLayout->addWidget(bar);
	mainLayout->setMargin(0);
	mainLayout->setSpacing(0);

	m_list.popup().changeParent(this);
	setLayout(mainLayout);

	connect(btn, &QPushButton::clicked, this, [btn, this]() {
		m_list.setRemovable(!m_list.isRemovable());
		btn->setText(m_list.isRemovable() ? tr("Finished") : tr("Edit"));
	});
	connect(add, &DPushButton::clicked, this, [this]() {
		m_list.editor()->setMode(true);
		m_list.editor()->setDelta(0);
		m_list.editor()->setText(tr("New Alarm"));
		m_list.popup().popup();
	});
	connect(&m_timer, &QTimer::timeout, this, [label, this]() {
		label->setText(m_list.nextAlarm());
		QStringList l = m_list.getModel().alarmsOnSystemTime();
		foreach(const QString &n, l) {
			DNotifySender msg(n);
			msg.appName(tr("Clock"));
			msg.appBody(tr("Now is time, %1 is only one right now!").arg(n));
			msg.appIcon("clock");
			msg.call();
		}
	});
	connect(&m_list.getModel(), &AlarmsModel::dataUpdated, this, [label, this](){
		label->setText(m_list.nextAlarm());
	});

	m_timer.setInterval(5900);
	m_timer.start();
}
