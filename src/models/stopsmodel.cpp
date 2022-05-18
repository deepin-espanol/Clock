#include "stopsmodel.h"

StopsModel::StopsModel(QObject *parent)
	: AdvancedItemModel(parent)
{
}

StopsModel::~StopsModel()
{
	m_tps.clear();
}

int StopsModel::rowCount(const QModelIndex &/*parent*/) const
{
	return m_tps.length();
}

int StopsModel::columnCount(const QModelIndex &/*parent*/) const
{
	return 1;
}

void StopsModel::addData(const uint delta)
{
	beginInsertRows(createIndex(0, 0), 0, 0);
	m_tps.insert(0, delta);
	endInsertRows();
	actualise();
}

void StopsModel::removeData(const int r)
{
	beginRemoveRows(createIndex(r, 0), r, r);
	m_tps.removeAt(r);
	actualise();
	endRemoveRows();
}

void StopsModel::actualise()
{
	if (m_tps.isEmpty()) {
		m_maxDigits = 0;
		return;
	}

	QList<uint> cpy = m_tps;
	std::sort(cpy.begin(), cpy.end());

	m_best = m_tps.indexOf(cpy.first());
	m_worst = m_tps.indexOf(cpy.last());

	m_maxDigits = QString::number(m_tps.count()).length();
}

QString StopsModel::deltaText(uint delta) const
{
	uint d = 0, s = 0, m = 0, h = 0;

	if (delta != 0) {
		d = delta;
		s = d/100;
		m = s/60;
		h = m/60;

		m -= h*60;
		s -= h*60*60 + m*60;
		d -= (h*60*60 + m*60 + s)*100;
	}

	return tr("%1:%2:%3:%4").arg((h < 10) ? "0" + QString::number(h) : QString::number(h))
				.arg((m < 10)  ? "0" + QString::number(m) : QString::number(m))
				.arg((s < 10) ? "0" + QString::number(s) : QString::number(s))
				.arg((d < 10) ? "0" + QString::number(d) : QString::number(d));
}

QVariant StopsModel::data(const QModelIndex &index, int role) const
{
	if (index.row() < m_tps.count() && index.row() > -1) {
		switch (role) {
		case Roles::Text: {
			return deltaText(m_tps[index.row()]);
		}
		case Roles::Digits: {
			return m_maxDigits;
		}
		case Roles::Number: {
			QString v = QString::number(m_tps.count() - index.row());
			while (v.length() < m_maxDigits) {
				v.prepend("0");
			}
			v.prepend("#");
			return v;
		}
		case Roles::Color: {
			return (m_best == index.row() ? Colors::Best : (m_worst == index.row() ? Colors::Worst : Colors::Normal));
		}
		}
	}
	return QVariant();
}

QModelIndex StopsModel::index(int row, int column, const QModelIndex &parent) const
{
	if (hasIndex(row, column, parent)) {
		return createIndex(row, column, m_tps.at(row));
	}
	return QModelIndex();
}

QModelIndex StopsModel::parent(const QModelIndex &) const
{
	return QModelIndex();
}

void StopsModel::clear()
{
	beginRemoveRows(createIndex(0, 0), 0, m_tps.count()-1);
	m_tps.clear();
	endRemoveRows();
}

void StopsModel::handlePress(QVariant) {}

void StopsModel::handleRelease(QVariant v)
{
	int r = v.toInt();
	if (-1 < r && r < m_tps.count()) {
		removeData(r);
		Q_EMIT dataUpdated();
	}
}
