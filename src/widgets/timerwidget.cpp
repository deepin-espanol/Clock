#include "timerwidget.h"

#include "src/helpers.h"

#include <DNotifySender>
#include <DApplication>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QResizeEvent>
#include <QPainter>

DCORE_USE_NAMESPACE
DWIDGET_USE_NAMESPACE

QLayout *generate(QLabel *l, QWidget *w)
{
	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(l);
	layout->addWidget(w);

	w->setMinimumSize(50, 160);
	l->setAlignment(Qt::AlignCenter);

	return layout;
}

TimerWidget::TimerWidget(QWidget *parent)
	: QWidget(parent),
	  m_container(new QWidget(this)),
	  m_timeLabel(new QLabel(this)),
	  m_stop(new QPushButton(tr("Stop"), this)),
	  m_start(new QPushButton(tr("Start"), this)),
	  m_lHrs(new QLabel(tr("Hours"), this)),
	  m_lMins(new QLabel(tr("Minutes"), this)),
	  m_lSecs(new QLabel(tr("Seconds"), this)),
	  m_secs(new NumberSpinBox(this)),
	  m_mins(new NumberSpinBox(this)),
	  m_hrs(new NumberSpinBox(this))
{
	QHBoxLayout *mainLayout = new QHBoxLayout;
	QVBoxLayout *centerLayout = new QVBoxLayout;
	QHBoxLayout *spinsLayout = new QHBoxLayout;

	spinsLayout->addLayout(generate(m_lHrs, m_hrs));
	spinsLayout->addLayout(generate(m_lMins, m_mins));
	spinsLayout->addLayout(generate(m_lSecs, m_secs));

	centerLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));
	centerLayout->addWidget(m_timeLabel);
	centerLayout->addLayout(spinsLayout);
	centerLayout->addWidget(m_start);
	centerLayout->addWidget(m_stop);
	centerLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));

	m_container->setLayout(centerLayout);

	mainLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));
	mainLayout->addWidget(m_container);
	mainLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));
	setLayout(mainLayout);

	QFont f = m_timeLabel->font();
	f.setPointSize(20);
	f.setBold(true);
	m_timeLabel->setFont(f);
	m_timeLabel->setAlignment(Qt::AlignCenter);

	m_hrs->setRange(0, 23);
	m_mins->setRange(0, 59);
	m_secs->setRange(0, 59);

	m_stop->setVisible(false);
	m_timeLabel->setVisible(false);

	connect(&t, &QTimer::timeout, this, &TimerWidget::count);
	connect(m_start, &QPushButton::clicked, this, &TimerWidget::start);
	connect(m_stop, &QPushButton::clicked, this, &TimerWidget::stop);

	t.setInterval(1000);
}

void TimerWidget::start()
{
	setVisibility(false);

	m_cpainter.reset();
	m_timeLabel->setText(formatted(0));
	m_delta = 0;
	m_percent = 0;
	m_limit = m_hrs->value()*3600 + m_mins->value()*60 + m_secs->value();

	if (m_limit < 1) {
		m_percent = 100;
		return stop();
	}

	t.start();
}

void TimerWidget::stop()
{
	t.stop();
	setVisibility(true);
	m_delta = 0;

	//Push notification
	DUtil::DNotifySender sender(tr("Your timer just ended!"));
	sender.appName(qApp->applicationName());
	sender.appIcon("clock");
	sender.appBody(tr("You've run out of time, the timer ended!"));
	sender.timeOut(10000);
	sender.call();
}

void TimerWidget::setVisibility(bool visible)
{
	m_hrs->setVisible(visible);
	m_mins->setVisible(visible);
	m_secs->setVisible(visible);
	m_lHrs->setVisible(visible);
	m_lMins->setVisible(visible);
	m_lSecs->setVisible(visible);

	m_timeLabel->setVisible(!visible);
	m_stop->setVisible(!visible);
	m_start->setVisible(visible);
}

void TimerWidget::count()
{
	m_delta += 1;
	m_percent = float(m_delta)/m_limit*100;


	m_timeLabel->setText(formatted(m_delta));

	if (m_limit < m_delta) {
		m_percent = 100;
		return stop();
	}

	repaint();
}

QString TimerWidget::formatted(unsigned int delta)
{
	const unsigned int hours = delta/3600;
	const unsigned int mins = (delta - hours*3600)/60;
	const unsigned int secs = delta - hours*3600 - mins*60;

	return tr("%1:%2:%3").arg((hours < 10) ? "0" + QString::number(hours) : QString::number(hours))
				.arg((mins < 10)  ? "0" + QString::number(mins) : QString::number(mins))
				.arg((secs < 10) ? "0" + QString::number(secs) : QString::number(secs));
}

void TimerWidget::resizeEvent(QResizeEvent *e)
{
	m_horizontal = e->size().width() > e->size().height();
	m_size = m_horizontal ? e->size().height() : e->size().width();
	m_container->setFixedSize(m_size-120, m_size-120);
}

void TimerWidget::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	m_cpainter.render(painter, palette(), QRect(m_horizontal ? (width()-m_size)/2+20 : 20, m_horizontal ? 20 : (height()-m_size)/2+20, m_size-40, m_size-40), m_percent);
}
