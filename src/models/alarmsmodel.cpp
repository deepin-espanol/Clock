#include "alarmsmodel.h"

#include "src/delegates/alarmsdelegate.h"
#include "src/helpers.h"

#include <QTime>
#include <cmath>

#include <iostream>

AlarmsModel::AlarmsModel(QObject *parent)
	: AdvancedItemModel(parent)
{
	ConfigOperator::instance()->registerContainer("alarms", this);
}

AlarmsModel::~AlarmsModel()
{
	qDeleteAll(m_als);
	m_als.clear();
}

int AlarmsModel::rowCount(const QModelIndex &/*parent*/) const
{
	return m_als.length();
}

int AlarmsModel::columnCount(const QModelIndex &/*parent*/) const
{
	return 1;
}

QString AlarmsModel::deltaText(int i) const
{
	QTime t;
	t.setHMS(0, 0, i);
	return t.toString("hh:mm");
}

QVariant AlarmsModel::data(const QModelIndex &index, int role) const
{
	if (index.row() < m_als.count() && index.row() > -1) {
		switch (role) {
		case Roles::Time: {
			int dh = m_als[index.row()]->delta/60;
			return QTime(dh, m_als[index.row()]->delta - dh*60);
		}
		case Roles::Enabled: return m_als[index.row()]->enable;
		case Roles::Days: return m_als[index.row()]->days;
		case Roles::Name: return m_als[index.row()]->name;
		}
	}
	return QVariant();
}

QModelIndex AlarmsModel::index(int row, int column, const QModelIndex &parent) const
{
	if (hasIndex(row, column, parent)) {
		return createIndex(row, column, m_als.at(row));
	}
	return QModelIndex();
}

QModelIndex AlarmsModel::parent(const QModelIndex &) const
{
	return QModelIndex();
}

void AlarmsModel::sort(int, Qt::SortOrder)
{
	//This is a bubble sort.
	const QDateTime localTime = QDateTime::currentDateTime();
	int i, j, n = m_als.length();
	for (i = 0; i < n-1; i++) {
		for (j = 0; j < n-i-1; j++) {
			if (m_als[j]->delta > m_als[j+1]->delta) {
				m_als.swapItemsAt(j, j+1);
			}
		}
	}
}

void AlarmsModel::clear()
{
	beginRemoveRows(createIndex(0, 0), 0, m_als.count()-1);
	qDeleteAll(m_als);
	m_als.clear();
	endRemoveRows();
}

void AlarmsModel::handlePress(QVariant) {}

void AlarmsModel::handleRelease(QVariant v)
{
	const AlarmEvent p = v.value<AlarmEvent>();
	if (-1 < p.row && p.row < m_als.count() && p.type != AlarmsDelegate::Roles::None) {
		switch (p.type) {
		case AlarmsDelegate::Roles::Remove: {
			beginRemoveRows(createIndex(p.row, 0), p.row, p.row);
			if (AlarmObject *obj = m_als.takeAt(p.row)) {
				delete obj;
			}
			endRemoveRows();
			break;
		}
		case AlarmsDelegate::Roles::Switch: {
			m_als[p.row]->enable = !m_als[p.row]->enable;
			Q_EMIT dataChanged(createIndex(0, p.row), createIndex(p.row, 0));
			break;
		}
		case AlarmsDelegate::Roles::Day: {
			toggleBit((unsigned int &)m_als[p.row]->days, p.additional);
			break;
		}
		case AlarmsDelegate::Roles::Edit: {
			Q_EMIT needsPopup(p.row, m_als[p.row]->delta, m_als[p.row]->name);
			break;
		}
		default: return;
		}
		Q_EMIT dataUpdated();
	}
}

void AlarmsModel::addAlarm(AlarmObject *obj)
{
	if (alarmContained(obj->delta, &AlarmObject::delta)) {
		delete obj;
		return;
	}

	const int i = getAlarmInsertbyDelta(obj);
	beginInsertRows(createIndex(i, 0), i, i);
	m_als.insert(i, obj);
	endInsertRows();
	Q_EMIT dataUpdated();
}

void AlarmsModel::changeData(int row, int delta, QString name)
{
	if (-1 < row && row < m_als.length()) {
		m_als[row]->delta = delta;
		m_als[row]->name = name;
		Q_EMIT dataChanged(createIndex(0, row), createIndex(row, 0));
		Q_EMIT dataUpdated();
	}
}

QString AlarmsModel::getNextAlarm()
{
	if (m_als.isEmpty()) {
		return "";
	}

	QTime t = QTime::currentTime();
	const int local = t.hour()*60+t.minute();

	int closest = 0;
	int smallest = m_als[0]->delta - local;
	int i = 1;
	while (i < m_als.length()) {
		int v = m_als[i]->delta;
		if (v < local) {
			v += 24*60;
		}
		const int d = (v - local);
		if (d < smallest) {
			closest = i;
			smallest = d;
		}
		i++;
	}

	return tr("Next alarm in %1, %2").arg(deltaText(smallest)).arg(m_als[closest]->name);
}

QStringList AlarmsModel::alarmsOnSystemTime() const
{
	if (m_als.isEmpty()) {
		return {};
	}

	QTime t = QTime::currentTime();
	const int local = t.hour()*60+t.minute();
	const int d = QDate::currentDate().dayOfWeek();
	QStringList out;

	int i = 0;
	while (i < m_als.length()) {
		AlarmObject *a = m_als[i];
		if (a->enable && CHECK_BIT(a->days, d) && a->delta == local) {
			out << a->name;
		}
		i++;
	}
	return out;

	return {};
}

void AlarmsModel::read(QDataStream &stream)
{
	const int oldc = m_als.count();
	qDeleteAll(m_als);
	m_als.clear();

	qint64 size = 0;
	stream >> size;

	int i = 0;
	while (i < size) {
		AlarmObject *o = new AlarmObject;
		StringData name;
		name.read(stream);

		o->name = name.string;
		stream >> o->delta >> o->days >> o->enable;

		addAlarm(o);
		i++;
	}
	Q_EMIT dataChanged(createIndex(0, 0), createIndex(oldc-1, 0));
	Q_EMIT dataUpdated();
}

void AlarmsModel::write(QDataStream &stream)
{
	qint64 size = m_als.size();
	stream << size;

	int i = 0;
	while (i < size) {
		AlarmObject *o = m_als[i];
		StringData name(o->name);
		name.write(stream);
		stream << o->delta << o->days << o->enable;
		i++;
	}
}

quint64 AlarmsModel::dataSize(QString name)
{
	quint64 sum = ConfigContainer::dataSize(name);
	qint64 size = m_als.size();
	int i = 0;
	while (i < size) {
		AlarmObject *o = m_als[i];
		sum += StringData(o->name).dataSize() + sizeof(o->delta) + sizeof(o->days) + sizeof(o->enable);
		i++;
	}
	return sum + sizeof(size);
}
