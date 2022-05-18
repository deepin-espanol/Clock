#ifndef TIMEEDITOR_H
#define TIMEEDITOR_H

#include "numberspinbox.h"

class QLineEdit;
class AlarmObject;

/**
 * @brief Editor for HH:mm:ss.
 */
class TimeEditor : public QWidget
{
	Q_OBJECT
public:
	explicit TimeEditor(QWidget *parent = nullptr);

	//If it is creating a new one, we don't use the same signals.
	void setMode(bool creation);

	Q_SLOT void setDelta(int d);
	Q_SLOT void setText(QString t);

Q_SIGNALS:
	void finished();
	void finished(int delta, QString text);
	void finished(AlarmObject *);

private:
	bool m_useCreation = false;

	NumberSpinBox *hBox;
	NumberSpinBox *mBox;
	QLineEdit *m_edit;
};

#endif // TIMEEDITOR_H
