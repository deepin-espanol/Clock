#ifndef TIMEZONESMODEL_H
#define TIMEZONESMODEL_H

#include <QTimeZone>

#include "src/external/timezone.h"
#include "src/widgets/advancedclasses.h"
#include "src/utils/configoperator.h"

struct TZObject
{
	QTimeZone tz;
	QString name;
	installer::ZoneInfo info;
	bool isDefault = false;
};

struct LargestTZObject;

class TimeZonesModel : public AdvancedItemModel, public ConfigContainer
{
	Q_OBJECT
public:
	explicit TimeZonesModel(QObject *parent = nullptr);
	~TimeZonesModel();

	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
	QModelIndex parent(const QModelIndex &) const override;

	void addInfo(const QString loca);
	void addInfo(const QTimeZone &info, bool system = false);
	void addInfo(installer::ZoneInfo info);
	void removeInfo(const QString loca);
	void removeInfo(installer::ZoneInfo info);

	void clear() override;
	void handlePress(QVariant v) override;
	void handleRelease(QVariant v) override;

	//Different enums used to retrieve data from the model.
	enum Roles {
		Name = Qt::DisplayRole,
		Diff = Qt::UserRole +1,
		Offset,
		DayTime,
		Time,
		NearBegin,
		NearEnd,
		IsSystem,
		Color
	};

	enum Colors {
		Night = Roles::Color +1,
		Raising,
		Midday,
		Day,
		Nightfall,
		Midnight
	};

	//Miscellaneous funcions used as facilities.
	enum EditBehaviour {
		Take = Colors::Midnight +1,
		Retrieve
	};

	template<typename T>
	inline
	TZObject *getTZbyMutable(TZObject *target, T TZObject::*ptr, EditBehaviour behave = EditBehaviour::Retrieve) {
		int i = m_tzs.length()/2, end = m_tzs.length(), beg = 0;
		if (end == 0) {
			return nullptr;
		}
		while (true) {
			if (i == end && end == beg) {
				//Last chance to have the target.
				if ((m_tzs[i]->*ptr) == (target->*ptr)) {
					return (behave == EditBehaviour::Retrieve ? m_tzs[i] : m_tzs.takeAt(i));
				}
				return nullptr;
			} else {
				const T t = m_tzs[i]->*ptr, s = target->*ptr;
				if (t < s) {
					beg = i;
					i = beg+(end-beg)/2;
				} else if (s > t) {
					end = i;
					i = end-(end-beg)/2;
				} else {
					return (behave == EditBehaviour::Retrieve ? m_tzs[i] : m_tzs.takeAt(i));
				}
			}
		}
	};

	template<typename T>
	inline
	TZObject *getTZby(T value, T TZObject::*ptr, EditBehaviour behave = EditBehaviour::Retrieve) {
		int i = m_tzs.length()/2, end = m_tzs.length(), beg = 0;
		if (end == 0) {
			return nullptr;
		}
		while (true) {
			if (i == end && end == beg) {
				//Last chance to have the target.
				if ((m_tzs[i]->*ptr) == value) {
					return ((behave == EditBehaviour::Retrieve) ? m_tzs[i] : m_tzs.takeAt(i));
				}
				return nullptr;
			} else {
				const T t = m_tzs[i]->*ptr;
				if (t < value) {
					beg = i;
					i = beg+(end-beg)/2;
				} else if (value > t) {
					end = i;
					i = end-(end-beg)/2;
				} else {
					return (behave == EditBehaviour::Retrieve ? m_tzs[i] : m_tzs.takeAt(i));
				}
			}
		}
	};

	template<typename T>
	inline
	bool tzContained(T value, T TZObject::*ptr) {
		int i = 0;
		while (i < m_tzs.length()) {
			if ((m_tzs[i]->*ptr) == value) {
				return true;
			}
			i++;
		}
		return false;
	}

	template<typename ... Args, typename T = int>
	inline
	TZObject *getTZbyLocaleMutable(QTimeZone *target, T (QTimeZone::*ptr)(Args ...) const, Args ... args, EditBehaviour behave = EditBehaviour::Retrieve) {
		int i = m_tzs.length()/2, end = m_tzs.length(), beg = 0;
		if (end == 0) {
			return nullptr;
		}
		while (true) {
			if (i == beg && end == (beg+1)) {
				//Last chance to have the target.
				if ((m_tzs[i]->tz.*ptr)(args ...) == (target->*ptr)(args ...)) {
					return (behave == EditBehaviour::Retrieve ? m_tzs[i] : m_tzs.takeAt(i));
				}
				return nullptr;
			} else {
				const T t = (m_tzs[i]->tz.*ptr)(args ...), s = (target->*ptr)(args ...);
				if (t < s) {
					beg = i;
					i = beg+(end-beg)/2;
				} else if (s > t) {
					end = i;
					i = end-(end-beg)/2;
				} else {
					return (behave == EditBehaviour::Retrieve ? m_tzs[i] : m_tzs.takeAt(i));
				}
			}
		}
	};

	template<typename ... Args, typename T = int>
	inline
	TZObject *getTZbyLocale(T value, T TZObject::*ptr(Args ...), Args ... args, EditBehaviour behave = EditBehaviour::Retrieve) {
		int i = m_tzs.length()/2, end = m_tzs.length(), beg = 0;
		if (end == 0) {
			return nullptr;
		}
		while (true) {
			if (i == beg && end == (beg+1)) {
				//Last chance to have the target.
				if ((m_tzs[i]->tz.*ptr)(args ...) == value) {
					return (behave == EditBehaviour::Retrieve ? m_tzs[i] : m_tzs.takeAt(i));
				}
				return nullptr;
			} else {
				const T t = (m_tzs[i]->tz.*ptr)(args ...);
				if (t < value) {
					beg = i;
					i = beg+(end-beg)/2;
				} else if (value > t) {
					end = i;
					i = end-(end-beg)/2;
				} else {
					return (behave == EditBehaviour::Retrieve ? m_tzs[i] : m_tzs.takeAt(i));
				}
			}
		}
	};

	template<typename ... Args, typename T = int>
	inline
	int getTZInsertbyLocaleMutable(QTimeZone *target, T (QTimeZone::*ptr)(Args ...) const, Args ... args) {
		int i = m_tzs.length()/2, end = m_tzs.length(), beg = 0;
		if (end == 0) {
			return 0;
		}
		while (true) {
			if (i == beg && end == (beg+1)) {
				//Last chance to have the target.
				if ((m_tzs[i]->tz.*ptr)(args ...) == (target->*ptr)(args ...)) {
					return i+1;
				}
				return i;
			} else {
				const T t = (m_tzs[i]->tz.*ptr)(args ...), s = (target->*ptr)(args ...);
				if (t < s) {
					beg = i;
					i = beg+(end-beg)/2;
				} else if (s > t) {
					end = i;
					i = end-(end-beg)/2;
				} else {
					return i+1;
				}
			}
		}
	};

	template<typename ... Args, typename T = int>
	inline
	int getInsertTZbyLocale(T value, T TZObject::*ptr(Args ...), Args ... args) {
		int i = m_tzs.length()/2, end = m_tzs.length(), beg = 0;
		if (end == 0) {
			return 0;
		}
		while (true) {
			if (i == beg && end == (beg+1)) {
				//Last chance to have the target.
				if ((m_tzs[i]->tz.*ptr)(args ...) == value) {
					return i+1;
				}
				return i;
			} else {
				const T t = (m_tzs[i]->tz.*ptr)(args ...);
				if (t < value) {
					beg = i;
					i = beg+(end-beg)/2;
				} else if (value > t) {
					end = i;
					i = end-(end-beg)/2;
				} else {
					return i+1;
				}
			}
		}
	};

	template<typename ... Args, typename T = int>
	inline
	int getIndexTZbyLocaleMutable(QTimeZone *target, T (QTimeZone::*ptr)(Args ...) const, Args ... args) {
		int i = m_tzs.length()/2, end = m_tzs.length(), beg = 0;
		if (end == 0) {
			return 0;
		}
		while (true) {
			if (i == beg && end == (beg+1)) {
				//Last chance to have the target.
				if ((m_tzs[i]->tz.*ptr)(args ...) == (target->*ptr)(args ...)) {
					return i;
				}
				return -1;
			} else {
				const T t = (m_tzs[i]->tz.*ptr)(args ...), s = (target->*ptr)(args ...);
				if (t < s) {
					beg = i;
					i = beg+(end-beg)/2;
				} else if (s > t) {
					end = i;
					i = end-(end-beg)/2;
				} else {
					return i;
				}
			}
		}
	};

	inline
	int getTZInsertbyDelta(QTimeZone *target) {
		int i = m_tzs.length()/2, end = m_tzs.length(), beg = 0;
		const QDateTime localTime = QDateTime::currentDateTime();
		const double s = (target->offsetFromUtc(localTime) - localTime.offsetFromUtc()) / 3600.0;
		if (end == 0) {
			return 0;
		}
		while (true) {
			if (i == beg && end == (beg+1)) {
				//Last chance to have the target.
				if ((m_tzs[i]->tz.offsetFromUtc(localTime) - localTime.offsetFromUtc()) / 3600.0 == s) {
					return i;
				}
				return i+1;
			} else {
				const double t = (m_tzs[i]->tz.offsetFromUtc(localTime) - localTime.offsetFromUtc()) / 3600.0;
				if (t < s) {
					beg = i;
					i = beg+(end-beg)/2;
				} else if (s > t) {
					end = i;
					i = end-(end-beg)/2;
				} else {
					return i;
				}
			}
		}
	};

	void sort(int = 0, Qt::SortOrder = Qt::SortOrder::AscendingOrder) override;

	TZObject *getTZByName(QString name);
	TZObject *getTZByOffset(QTimeZone tz);

	//Convert raw time to text.
	QString deltaText(int i) const;

	void read(QDataStream &stream) override;
	void write(QDataStream &stream) override;
	quint64 dataSize(QString name = "") override;

private:
	QList<TZObject *> m_tzs;
	int m_largest = -1;
};

inline int indexOfColor(TimeZonesModel::Colors color) {
	return color - TimeZonesModel::Colors::Night;
}

inline int indexOfBackground(bool day)
{
	return day ? 1 : 0;
}

#endif // TIMEZONESMODEL_H
