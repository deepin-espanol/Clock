#ifndef ALARMSMODEL_H
#define ALARMSMODEL_H

#include "src/widgets/advancedclasses.h"
#include "src/utils/configoperator.h"

/**
 * @brief Enum for days of the week meant to be used in flags.
 */
enum Days {
	Monday    = 1,
	Tuesday   = 2,
	Wednesday = 4,
	Thursday  = 8,
	Friday    = 16,
	Saturday  = 32,
	Sunday    = 64
};

/**
 * @brief Alarm object containing data about an Alarm.
 */
struct AlarmObject
{
	inline AlarmObject(int del = 0, QString n = "", int d = 0, bool en = true) {
		delta = del;
		name = n;
		days = d;
		enable = en;
	}
	int delta;
	QString name;
	int days = 0;
	bool enable = true;
};

/**
 * @brief Implementation of the model used for the Alarms.
 */
class AlarmsModel : public AdvancedItemModel, public ConfigContainer
{
	Q_OBJECT
public:
	explicit AlarmsModel(QObject *parent = nullptr);
	~AlarmsModel();

	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
	QModelIndex parent(const QModelIndex &) const override;

	void addAlarm(AlarmObject *obj);
	//Used when updating from an editor.
	void changeData(int row, int delta, QString text);

	QString getNextAlarm();

	void clear() override;
	void handlePress(QVariant v) override;
	void handleRelease(QVariant v) override;

	enum Roles {
		Time = Qt::UserRole +1,
		Enabled,
		Days,
		Name
	};

	//Miscellaneous funcions used as facilities.
	enum EditBehaviour {
		Take = Roles::Name +1,
		Retrieve
	};

	template<typename T>
	inline
	AlarmObject *getAlarmby(T value, T AlarmObject::*ptr, EditBehaviour behave = EditBehaviour::Retrieve) {
		int i = m_als.length()/2, end = m_als.length(), beg = 0;
		if (end == 0) {
			return nullptr;
		}
		while (true) {
			if (i == end && end == beg) {
				//Last chance to have the target.
				if ((m_als[i]->*ptr) == value) {
					return ((behave == EditBehaviour::Retrieve) ? m_als[i] : m_als.takeAt(i));
				}
				return nullptr;
			} else {
				const T t = m_als[i]->*ptr;
				if (t < value) {
					beg = i;
					i = beg+(end-beg)/2;
				} else if (value > t) {
					end = i;
					i = end-(end-beg)/2;
				} else {
					return (behave == EditBehaviour::Retrieve ? m_als[i] : m_als.takeAt(i));
				}
			}
		}
	};

	inline
	int getAlarmInsertbyDelta(AlarmObject *target) {
		int i = m_als.length()/2, end = m_als.length(), beg = 0;
		if (end == 0) {
			return 0;
		}
		while (true) {
			if (i == beg && end == (beg+1)) {
				//Last chance to have the target.
				if (m_als[i]->delta == target->delta) {
					return i;
				}
				return i+1;
			} else {
				const double t = m_als[i]->delta;
				if (t < target->delta) {
					beg = i;
					i = beg+(end-beg)/2;
				} else if (target->delta > t) {
					end = i;
					i = end-(end-beg)/2;
				} else {
					return i;
				}
			}
		}
	};

	template<typename T>
	inline
	bool alarmContained(T value, T AlarmObject::*ptr) {
		int i = 0;
		while (i < m_als.length()) {
			if ((m_als[i]->*ptr) == value) {
				return true;
			}
			i++;
		}
		return false;
	}

	void sort(int = 0, Qt::SortOrder = Qt::SortOrder::AscendingOrder) override;

	//Time to text.
	QString deltaText(int i) const;

	//Know if system time corresponds to an alarm when calling the function.
	QStringList alarmsOnSystemTime() const;

	void read(QDataStream &stream) override;
	void write(QDataStream &stream) override;
	quint64 dataSize(QString name = "") override;

Q_SIGNALS:
	/**
	 * @brief Emited when the edit button of the item is clicked.
	 */
	void needsPopup(int row, int delta, QString text);

private:
	QList<AlarmObject *> m_als;
	int m_largest = -1;
};

#endif // ALARMSMODEL_H
