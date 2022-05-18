#ifndef DUALWIDGETCONTAINER_H
#define DUALWIDGETCONTAINER_H

#include <QWidget>

/**
 * @brief Layout widget with different options for 2 widgets, one having its minimum height/width.
 */
class DualWidgetContainer : public QWidget
{
	Q_OBJECT
public:
	explicit DualWidgetContainer(QWidget *f, QWidget *s, QWidget *p = nullptr);
	DualWidgetContainer(QWidget *p = nullptr);

	void setWidgets(QWidget *first, QWidget *second);

	void setMarginsOfFirst(const int margin);
	void setMarginsOfSecond(const int margin);
	void setSpacing(int spacing);
	void setSmaller(bool onFirst);

protected:
	void paintEvent(QPaintEvent *e) override;
	void resizeEvent(QResizeEvent *e) override;
	virtual void makeLayout(const QSize &s) = 0;

	QWidget *first;
	QWidget *second;

	int m_fmargin = 5;
	int m_smargin = 5;
	int m_spacing = 0;
	bool m_minimumOnFirst = false;
};

/**
 * @brief Vertical implementation.
 */
class VerticalDWC : public DualWidgetContainer
{
	Q_OBJECT
public:
	explicit VerticalDWC(QWidget *f, QWidget *s, QWidget *parent = nullptr);
	VerticalDWC(QWidget *p);

protected:
	void makeLayout(const QSize &s) override;
};

#endif // DUALWIDGETCONTAINER_H
