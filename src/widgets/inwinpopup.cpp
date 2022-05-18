#include "inwinpopup.h"

#include <QVBoxLayout>
#include <QEvent>
#include <QResizeEvent>

InWinPopup::InWinPopup(QWidget *c, QWidget *p)
	: DBlurEffectWidget(p)
{
	QHBoxLayout *horizontalLayout = new QHBoxLayout;
	QVBoxLayout *verticalLayout = new QVBoxLayout;

	horizontalLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));
	horizontalLayout->addLayout(verticalLayout);
	horizontalLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));

	verticalLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));
	verticalLayout->addWidget(c);
	verticalLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));

	middle = verticalLayout;
	m_central = c;
	setLayout(horizontalLayout);
	setVisible(false);
	setBlendMode(DBlurEffectWidget::BlendMode::InWindowBlend);

	if (p) {
		p->installEventFilter(this);
	}
}

InWinPopup::InWinPopup(QWidget *p)
	: InWinPopup(nullptr, p)
{
}

void InWinPopup::popup()
{
	setVisible(true);
}

void InWinPopup::remove()
{
	setVisible(false);
	Q_EMIT ended();
}

void InWinPopup::setCentralWidget(QWidget *w)
{
	middle->removeWidget(m_central);
	middle->insertWidget(1, w);
	m_central = w;
}

void InWinPopup::changeParent(QWidget *n)
{
	if (parentWidget()) {
		parentWidget()->removeEventFilter(this);
	}
	setParent(n);
	if (n) {
		n->installEventFilter(this);
	}
}

bool InWinPopup::eventFilter(QObject *t, QEvent *ev)
{
	if (t == parentWidget() && ev->type() == QEvent::Type::Resize) {
		if (QResizeEvent *e = dynamic_cast<QResizeEvent *>(ev)) {
			resize(e->size());
		}
	}
	return DBlurEffectWidget::eventFilter(t, ev);
}
