#include "mainwindow.h"

#include <QHBoxLayout>
#include <DTitlebar>

MainWindow::MainWindow(QWidget *p)
	: DMainWindow(p),
	  m_stack(new QStackedWidget(this)),
	  m_chooser(new TabChooser(titlebar())),
	  m_clocks(new WorldClocksWidget),
	  m_alarms(new AlarmsWidget),
	  m_timer(new TimerWidget),
	  m_swatch(new StopWatch)
{
	setCentralWidget(m_stack);

	QWidget *c = new QWidget(this);
	QHBoxLayout *l = new QHBoxLayout;
	l->setMargin(5);
	l->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));
	l->addWidget(m_chooser);
	l->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));
	c->setLayout(l);

	titlebar()->setCustomWidget(c, true);
	titlebar()->setIcon(QIcon::fromTheme("clock"));

	m_chooser->add(tr("Clocks"), m_clocks, true);
	m_chooser->add(tr("Alarms"), m_alarms);
	m_chooser->add(tr("Timer"), m_timer);
	m_chooser->add(tr("Stopwatch"), m_swatch);
	m_stack->addWidget(m_clocks);
	m_stack->addWidget(m_alarms);
	m_stack->addWidget(m_timer);
	m_stack->addWidget(m_swatch);

	connect(m_chooser, &TabChooser::clicked, m_stack, &QStackedWidget::setCurrentWidget);
}

void MainWindow::closeEvent(QCloseEvent *e)
{
	DMainWindow::closeEvent(e);
	Q_EMIT closed();
}
