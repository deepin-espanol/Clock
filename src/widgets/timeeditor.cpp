#include "timeeditor.h"

#include "src/models/alarmsmodel.h"

#include <DPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <DLabel>
#include <QLineEdit>

DWIDGET_USE_NAMESPACE

TimeEditor::TimeEditor(QWidget *parent) : QWidget(parent)
{
	QVBoxLayout *mainLayout = new QVBoxLayout;
	QHBoxLayout *subLayout = new QHBoxLayout;
	QVBoxLayout *aLayout = new QVBoxLayout;
	QVBoxLayout *bLayout = new QVBoxLayout;

	m_edit = new QLineEdit(this);
	QPushButton *ok = new QPushButton(this);
	hBox = new NumberSpinBox(this);
	mBox = new NumberSpinBox(this);
	DLabel *hLabel = new DLabel;
	DLabel *mLabel = new DLabel;

	hLabel->setText(tr("Hours"));
	mLabel->setText(tr("Mins"));
	hLabel->setAlignment(Qt::AlignCenter);
	mLabel->setAlignment(Qt::AlignCenter);
	hLabel->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
	mLabel->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));

	hBox->setRange(0, 23);
	mBox->setRange(0, 59);

	aLayout->addWidget(hLabel);
	aLayout->addWidget(hBox);
	bLayout->addWidget(mLabel);
	bLayout->addWidget(mBox);

	subLayout->addLayout(aLayout);
	subLayout->addLayout(bLayout);

	ok->setText(tr("Confirm"));

	mainLayout->addWidget(m_edit);
	mainLayout->addLayout(subLayout);
	mainLayout->addWidget(ok);
	setLayout(mainLayout);
	setMinimumSize(200, 300);

	connect(ok, &QPushButton::clicked, this, [this]() {
		Q_EMIT finished();
		if (m_useCreation) {
			Q_EMIT finished(new AlarmObject {hBox->value()*60 + mBox->value(), m_edit->text(), 0, true});
		} else {
			Q_EMIT finished(hBox->value()*60 + mBox->value(), m_edit->text());
		}
	});
}

void TimeEditor::setMode(bool creation)
{
	m_useCreation = creation;
}

void TimeEditor::setDelta(int d)
{
	int hours = d/60;
	hBox->setValue(hours);
	mBox->setValue(d-hours*60);
}

void TimeEditor::setText(QString t)
{
	m_edit->setText(t);
}
