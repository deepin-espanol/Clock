#ifndef INWINPOPUP_H
#define INWINPOPUP_H

#include <DBlurEffectWidget>

class QVBoxLayout;

DWIDGET_USE_NAMESPACE

/**
 * @brief Implementation of a "popup" showing up into the parent widget.
 */
class InWinPopup : public DBlurEffectWidget
{
	Q_OBJECT
public:
	explicit InWinPopup(QWidget *center, QWidget *parent);
	InWinPopup(QWidget *p);

	void popup();
	void remove();
	void setCentralWidget(QWidget *w);
	void changeParent(QWidget *n);

Q_SIGNALS:
	void ended();

protected:
	bool eventFilter(QObject *t, QEvent *ev) override;

private:
	QWidget *m_central;
	QVBoxLayout *middle;
};

#endif // INWINPOPUP_H
