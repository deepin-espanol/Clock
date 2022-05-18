#include "tabchooser.h"

TabChooser::TabChooser(QWidget *p) : DButtonBox(p)
{
}

void TabChooser::add(QString name, QWidget *w, bool checked)
{
	DButtonBoxButton *btn = new DButtonBoxButton(name);
	btn->setChecked(checked);

	connect(btn, &DButtonBoxButton::clicked, this, [w, this](){
		Q_EMIT clicked(w);
	});

	QList<DButtonBoxButton *> l;
	QList<QAbstractButton *> list = buttonList();
	int i = 0;
	while (i < list.length()) {
		l << (DButtonBoxButton *)list[i];
		i++;
	}
	l << btn;
	setButtonList(l, true);
}
