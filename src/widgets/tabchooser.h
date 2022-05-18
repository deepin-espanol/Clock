#ifndef TABCHOOSER_H
#define TABCHOOSER_H

#include <DButtonBox>
#include <QMap>

DWIDGET_USE_NAMESPACE

/**
 * @brief Buttons used to change the widget when clicked according to their "attached" widget.
 */
class TabChooser : public DButtonBox
{
	Q_OBJECT
public:
	explicit TabChooser(QWidget *parent = nullptr);

	void add(QString name, QWidget *w, bool checked = false);

Q_SIGNALS:
	void clicked(QWidget *w);
};

#endif // TABCHOOSER_H
