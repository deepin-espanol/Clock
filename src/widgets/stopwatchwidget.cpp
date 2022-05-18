#include "stopwatchwidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

#include <QPainter>
#include <QResizeEvent>

StopWatchWidget::StopWatchWidget(QWidget *parent)
	: QWidget(parent),
	  m_container(new QWidget(this)),
	  m_stop(new QPushButton(this)),
	  m_start(new QPushButton(this)),
	  m_timeLabel(new QLabel(this))
{
	QHBoxLayout *mainLayout = new QHBoxLayout;
	QVBoxLayout *centerLayout = new QVBoxLayout;
	QHBoxLayout *buttonsLayout = new QHBoxLayout;

	buttonsLayout->addWidget(m_start);
	buttonsLayout->addWidget(m_stop);

	centerLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));
	centerLayout->addWidget(m_timeLabel);
	centerLayout->addLayout(buttonsLayout);
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
	m_timeLabel->setText(formatted(0));

	m_stop->setVisible(false);

	m_start->setText(tr("Start"));
	m_stop->setText(tr("Lap"));
	m_timer.setInterval(10);

	setVisibility(false);

	connect(&m_timer, &QTimer::timeout, this, &StopWatchWidget::count);
	connect(m_start, &QPushButton::clicked, this, &StopWatchWidget::handlePress);
	connect(m_stop, &QPushButton::clicked, this, &StopWatchWidget::mark, Qt::DirectConnection);
}

void StopWatchWidget::start()
{
	Q_EMIT reset();
	m_delta = 0;
	m_currentScope = UpdateScope::MiliSecs;
	m_cpainter.reset();
	m_timeLabel->setText(formatted(0));
	setVisibility(true);
	m_timer.start();
}

void StopWatchWidget::stop()
{
	m_timer.stop();
	setVisibility(false);
}

void StopWatchWidget::count()
{
	m_delta++;
	m_timeLabel->setText(formatted(m_delta));

	//For performance reasons, when more and more time passes, more we slow down the repainting.
	switch (m_currentScope) {
	case UpdateScope::MiliSecs: {
		m_needsUpdate = true;
		repaint();
		if (m_delta > 100) {
			m_currentScope = UpdateScope::Secs;
		}
	}
	case UpdateScope::Secs: {
		if (m_delta % 100 == 0) {
			m_needsUpdate = true;
			repaint();
			if (m_delta > 100*60) {
				m_currentScope = UpdateScope::Mins;
			}
		}
	}
	case UpdateScope::Mins: {
		if (m_delta % 100*60 == 0) {
			m_needsUpdate = true;
			repaint();
			if (m_delta > 100*60*60) {
				m_currentScope = UpdateScope::Hrs;
			}
		}
	}
	case UpdateScope::Hrs: {
		if (m_delta % 100*60*60) {
			m_needsUpdate = true;
			repaint();
		}
	}
	}
}

void StopWatchWidget::mark()
{
	Q_EMIT deltaMark(m_delta - m_lmark);
	m_lmark = m_delta;
}

void StopWatchWidget::handlePress()
{
	if (m_visibility) {
		return stop();
	}
	return start();
}

void StopWatchWidget::setVisibility(bool visible)
{
	m_stop->setVisible(visible);
	m_start->setText(visible ? tr("Stop") : tr("Start"));
	m_visibility = visible;
	m_needsUpdate = true;
	repaint();
}

QString StopWatchWidget::formatted(unsigned int delta)
{
	uint d = 0, s = 0, m = 0, h = 0;

	if (delta != 0) {
		d = delta;
		s = d/100;
		m = s/60;
		h = m/60;

		m -= h*60;
		s -= h*60*60 + m*60;
		d -= (h*60*60 + m*60 + s)*100;
	}

	return tr("%1:%2:%3:%4").arg((h < 10) ? "0" + QString::number(h) : QString::number(h))
				.arg((m < 10)  ? "0" + QString::number(m) : QString::number(m))
				.arg((s < 10) ? "0" + QString::number(s) : QString::number(s))
				.arg((d < 10) ? "0" + QString::number(d) : QString::number(d));
}

void StopWatchWidget::resizeEvent(QResizeEvent *e)
{
	m_horizontal = e->size().width() > e->size().height();
	m_size = m_horizontal ? e->size().height() : e->size().width();
	m_container->setFixedSize(m_size-120, m_size-120);
	m_needsUpdate = true;
	return QWidget::resizeEvent(e);
}

void StopWatchWidget::paintEvent(QPaintEvent *e)
{
	QPainter painter(this);

	if (m_needsUpdate) {
		QImage img(size(), QImage::Format_ARGB32_Premultiplied);
		img.fill(Qt::transparent);
		QPainter p(&img);
		if (m_visibility) {
			m_cpainter.render(p, palette(), QRect(m_horizontal ? (width()-m_size)/2+20 : 20, m_horizontal ? 20 : (height()-m_size)/2+20, m_size-40, m_size-40));
		} else {
			m_cpainter.render(p, palette(), QRect(m_horizontal ? (width()-m_size)/2+20 : 20, m_horizontal ? 20 : (height()-m_size)/2+20, m_size-40, m_size-40), 0);
		}
		source = QPixmap::fromImage(img);
		m_needsUpdate = false;
	}

	painter.fillRect(e->rect(), palette().color(QPalette::ColorGroup::Current, QPalette::ColorRole::Background));
	painter.drawPixmap(e->rect(), source, e->rect());
}
