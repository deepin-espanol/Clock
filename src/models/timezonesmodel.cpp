#include "timezonesmodel.h"

#include "src/external/sunrise.h"

#include <QColor>
#include <cmath>

qint64 timeToMins(const QTime &t)
{
	return t.hour()*3600 + t.minute()*60;
}

TimeZonesModel::TimeZonesModel(QObject *parent)
	: AdvancedItemModel(parent)
{
	ConfigOperator::instance()->registerContainer("timezones", this);
}

TimeZonesModel::~TimeZonesModel()
{
	qDeleteAll(m_tzs);
	m_tzs.clear();
}

int TimeZonesModel::rowCount(const QModelIndex &/*parent*/) const
{
	return m_tzs.length();
}

int TimeZonesModel::columnCount(const QModelIndex &/*parent*/) const
{
	return 1;
}

QString TimeZonesModel::deltaText(int i) const
{
	const QDateTime localTime = QDateTime::currentDateTime();
	const double timeDelta = (m_tzs[i]->tz.offsetFromUtc(localTime) - localTime.offsetFromUtc()) / 3600.0;

	QString dateLiteral;
	if (localTime.time().hour() + timeDelta >= 24) {
		dateLiteral = tr("Tomorrow");
	} else if (localTime.time().hour() + timeDelta <= 0) {
		dateLiteral = tr("Yesterday");
	} else {
		dateLiteral = tr("Today");
	}

	int decimalNumber = 1;
	//小时取余,再取分钟,将15分钟的双倍只显示一位小数,其他的都显示两位小数
	switch (m_tzs[i]->tz.offsetFromUtc(localTime) % 3600 / 60 / 15) {
	case -1:
	case -3:
	case 1:
	case 3:
		decimalNumber = 2;
		break;
	default:
		decimalNumber = 1;
		break;
	}

	QString compareLiteral;
	if (timeDelta == 0 && localTime.timeZone() == QTimeZone::systemTimeZone()) {
		compareLiteral = tr("Local time");
	} else if (timeDelta > 0) {
		compareLiteral = tr("%1 hours more than local").arg(QString::number(timeDelta, 'f', decimalNumber));
	} else {
		compareLiteral = tr("%1 hours less than local").arg(QString::number(-timeDelta, 'f', decimalNumber));
	}

	return compareLiteral;
}

QVariant TimeZonesModel::data(const QModelIndex &index, int role) const
{
	if (index.row() < m_tzs.count() && index.row() > -1) {
		switch (role) {
		case Offset: return m_tzs[index.row()]->tz.offsetFromUtc(QDateTime::currentDateTime());
		case Diff: {
			return deltaText(index.row());
		}
		case Name: {
			return m_tzs[index.row()]->name;
		}
		case IsSystem: {
			return m_tzs[index.row()]->isDefault;
		}
		default: {
			QDateTime target = QDateTime::currentDateTime().toTimeZone(m_tzs[index.row()]->tz);
			Sunrise sunrise(m_tzs[index.row()]->info.latitude, m_tzs[index.row()]->info.longitude);

			qint64 zoned = timeToMins(target.time());
			qint64 night = timeToMins(sunrise.sunrise(target.date()));
			qint64 day = timeToMins(sunrise.sunset(target.date()));

			target.toTime_t();

			switch (role) {
			case NearBegin: {
				return (abs(day - zoned) <= 1);
			}
			case NearEnd: {
				return (abs(night - zoned) <= 1);
			}
			case DayTime: {
				return (night > zoned && zoned >= day);
			}
			case Time: {
				return target.time();
			}
			}

			if (role == Color) {
				QTime midnight1(0, 0, 0);
				QTime midnight2(24, 0, 0);
				QTime midday(12, 0, 0);

				if ((abs(day-zoned) <= 3600)) {
					return (int)Colors::Raising;
				} else if ((abs(night - zoned)) <= 3600) {
					return (int)Colors::Nightfall;
				} else if ((abs(timeToMins(midday) - zoned)) <= 3600) {
						return (int)Colors::Midday;
				} else if ((abs(timeToMins(midnight1) - zoned)) <= 3600 || (abs(timeToMins(midnight2) - zoned)) <= 3600) { //Had 23*3600
					return (int)Colors::Midnight;
				} else if (day <= zoned && zoned <= night) {
					return (int)Colors::Day;
				} else {
					return (int)Colors::Night;
				}
			}
		}
		}
	}
	return QVariant();
}

QModelIndex TimeZonesModel::index(int row, int column, const QModelIndex &parent) const
{
	if (hasIndex(row, column, parent)) {
		return createIndex(row, column, m_tzs.at(row));
	}
	return QModelIndex();
}

QModelIndex TimeZonesModel::parent(const QModelIndex &) const
{
	return QModelIndex();
}

void TimeZonesModel::sort(int, Qt::SortOrder)
{
	//This is a bubble sort.
	const QDateTime localTime = QDateTime::currentDateTime();
	int i, j, n = m_tzs.length();
	for (i = 0; i < n-1; i++) {
		for (j = 0; j < n-i-1; j++) {
			const double t = (m_tzs[j]->tz.offsetFromUtc(localTime) - localTime.offsetFromUtc()) / 3600.0;
			const double s = (m_tzs[j+1]->tz.offsetFromUtc(localTime) - localTime.offsetFromUtc()) / 3600.0;
			if (t > s) {
				m_tzs.swapItemsAt(j, j+1);
			}
		}
	}
}

TZObject *TimeZonesModel::getTZByName(QString name)
{
	return getTZby(name, &TZObject::name);
}

TZObject *TimeZonesModel::getTZByOffset(QTimeZone tz)
{
	const QDateTime c = QDateTime::currentDateTime();
	return getTZbyLocaleMutable<const QDateTime &>(&tz, &QTimeZone::offsetFromUtc, c, EditBehaviour::Retrieve);
}

void TimeZonesModel::addInfo(const QString loca)
{
	QTimeZone tz(loca.toLocal8Bit());
	addInfo(tz);
}

void TimeZonesModel::addInfo(const QTimeZone &info, bool system)
{
	if (tzContained(QString(info.id()), &TZObject::name)) {
		return;
	}

	const QDateTime c = QDateTime::currentDateTime();
	TZObject *obj = new TZObject;
	obj->name = info.id();
	obj->tz = info;
	obj->isDefault = system;

	int i = getTZInsertbyDelta(&obj->tz);
	beginInsertRows(createIndex(i, 0), i, i);
	m_tzs.insert(i, obj);
	endInsertRows();
	Q_EMIT dataUpdated();
}

void TimeZonesModel::addInfo(installer::ZoneInfo info)
{
	if (tzContained(info.timezone, &TZObject::name)) {
		return;
	}

	const QDateTime c = QDateTime::currentDateTime();
	TZObject *data = new TZObject;
	data->tz = QTimeZone(info.timezone.toLocal8Bit());
	data->name = data->tz.id();

	int i = getTZInsertbyDelta(&data->tz);
	beginInsertRows(createIndex(i, 0), i, i);
	m_tzs.insert(i, data);
	sort();
	endInsertRows();
	Q_EMIT dataUpdated();
}

void TimeZonesModel::removeInfo(installer::ZoneInfo info)
{
	QTimeZone tz(info.timezone.toLocal8Bit());
	const QDateTime c = QDateTime::currentDateTime();
	int i = getIndexTZbyLocaleMutable<const QDateTime &>(&tz, &QTimeZone::offsetFromUtc, c);

	if (i < 0) {
		return;
	}

	beginRemoveRows(createIndex(i, 0), i, i);
	if (TZObject *obj = m_tzs.takeAt(i)) {
		delete obj;
	}
	endRemoveRows();
	Q_EMIT dataUpdated();
}

void TimeZonesModel::clear()
{
	beginRemoveRows(createIndex(0, 0), 0, m_tzs.count()-1);
	qDeleteAll(m_tzs);
	m_tzs.clear();
	endRemoveRows();
}

void TimeZonesModel::handlePress(QVariant) {}

void TimeZonesModel::handleRelease(QVariant v)
{
	int r = v.toInt();
	if (-1 < r && r < m_tzs.count()) {
		//It cannot be the local time!
		if (m_tzs[r]->isDefault) {
			return;
		}
		beginRemoveRows(createIndex(r, 0), r, r);
		if (TZObject *obj = m_tzs.takeAt(r)) {
			delete obj;
		}
		endRemoveRows();
		Q_EMIT dataUpdated();
	}
}

void TimeZonesModel::read(QDataStream &stream)
{
	qDeleteAll(m_tzs);
	m_tzs.clear();

	qint64 size = 0;
	stream >> size;

	addInfo(QTimeZone::systemTimeZone(), true);

	int i = 0;
	while (i < size) {
		StringData name;
		name.read(stream);
		addInfo(name.string);
		i++;
	}
}

void TimeZonesModel::write(QDataStream &stream)
{
	qint64 size = m_tzs.size();
	stream << (size-1);

	int i = 0;
	while (i < size) {
		TZObject *o = m_tzs[i];
		if (!o->isDefault) {
			StringData name(o->tz.id());
			name.write(stream);
		}
		i++;
	}
}

quint64 TimeZonesModel::dataSize(QString name)
{
	quint64 sum = ConfigContainer::dataSize(name);
	qint64 size = m_tzs.size();
	int i = 0;
	while (i < size) {
		TZObject *o = m_tzs[i];
		if (!o->isDefault) {
			sum += StringData(o->tz.id()).dataSize();
		}
		i++;
	}
	return sum + sizeof(size);
}
