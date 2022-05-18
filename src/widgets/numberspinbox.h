#ifndef NUMBERSPINBOX_H
#define NUMBERSPINBOX_H

#include <QWidget>

/**
 * @brief Input class, like a QSpinBox but a bit more clean.
 */
class NumberSpinBox : public QWidget
{
	Q_OBJECT
public:
	explicit NumberSpinBox(QWidget *parent = nullptr);

	int value();
	int step();

	void setValue(int v);
	void setStep(int s);
	void setRange(int min, int max);

	void increment(int steps = 1);
	void decrement(int steps = 1);

Q_SIGNALS:
	void valueChanged(int v);

protected:
	void paintEvent(QPaintEvent *e) override;
	void mousePressEvent(QMouseEvent *e) override;
	void mouseReleaseEvent(QMouseEvent *e) override;
	void resizeEvent(QResizeEvent *e) override;
	void wheelEvent(QWheelEvent *e) override;

	enum Areas {
		None   = 0,
		Top    = 1,
		Middle = 2,
		Bottom = 4
	};

	int areas(const QPoint &p) const;
	int areas(const QRect &r, bool force = false) const;

private:
	int m_max = 1;
	int m_min = 0;
	int m_val = 0;
	int m_step = 1;

	bool m_top = false;
	bool m_bottom = false;

	bool m_clicked = false;
	Areas m_area = Areas::None;
	int m_height = 1;
	int m_s = 1;
	bool m_useVertical = true;
};

#endif // NUMBERSPINBOX_H
