#include "configoperator.h"

#include <DStandardPaths>
#include <QFile>
#include <QDir>
#include <QDataStream>

DCORE_USE_NAMESPACE

bool ensureLocation(QFile &f)
{
	if (f.exists()) {
		return true;
	}

	//Obtain the dir path
	QStringList l = f.fileName().split("/");
	l.removeLast();
	QString p = l.join("/");
	QDir d(p);

	return (d.exists() || d.mkpath(p));
}

ConfigData::ConfigData() {}

ConfigData::~ConfigData() {}

QDataStream &operator<<(QDataStream &stream, ConfigData *d)
{
	d->write(stream);
	return stream;
}

QDataStream &operator>>(QDataStream &stream, ConfigData *d)
{
	d->read(stream);
	return stream;
}

ConfigContainer::~ConfigContainer()
{
	if (__operator) {
		__operator->unregisterContainer(this);
	}
}

int ConfigContainer::version()
{
	return containerVersion;
}

bool ConfigContainer::check(QString name, QDataStream &stream)
{
	StringData d;
	qint64 ver;
	d.read(stream);
	stream >> ver;
	return (ver == containerVersion && name == d.string);
}

void ConfigContainer::writeCheck(QString name, QDataStream &stream)
{
	StringData d(name);
	d.write(stream);
	stream << containerVersion;
}

quint64 ConfigContainer::dataSize(QString name)
{
	return checkSize(name);
}

void ConfigContainer::save()
{
	if (__operator) {
		__operator->writeData();
	}
}

quint64 ConfigContainer::checkSize(QString name)
{
	return StringData(name).dataSize() + sizeof(containerVersion);
}

StringData::StringData(QString t)
{
	string = t;
}

StringData::StringData(const StringData &d)
{
	string = d.string;
}

void StringData::read(QDataStream &stream)
{
	QByteArray b;
	qint64 size = 0;
	stream >> size;

	int i = 0;
	while (i < size) {
		char c = 0;
		stream.readRawData(&c, 1);
		b.append(c);
		i++;
	}
	string = QString::fromUtf8(b);
}

void StringData::write(QDataStream &stream)
{
	QByteArray v = string.toUtf8();
	qint64 size = v.size();
	int i = 0;

	stream << size;
	while (i < size) {
		char c = v.at(i);
		stream.writeRawData(&c, 1);
		i++;
	}
}

quint64 StringData::dataSize(QString)
{
	return string.toUtf8().length();
}

ConfigOperator::ConfigOperator(Dtk::Widget::DApplication *app, bool as)
	: QObject(app),
	  m_autoSave(as)
{
	if (!inst) {
		inst = this;
	}

	connect(app, &Dtk::Widget::DApplication::aboutToQuit, this, [this]() {
		writeData();
		delete this;
	}, Qt::DirectConnection);
}

ConfigOperator::~ConfigOperator()
{
	//Update all internal states
	Q_FOREACH(ConfigContainer *ctr, containers) {
		ctr->__operator = nullptr;
	}
}

ConfigOperator *ConfigOperator::instance()
{
	return inst;
}

void ConfigOperator::registerContainer(QString name, ConfigContainer *container)
{
	if (containers.contains(name)) {
		return;
	}
	containers[name] = container;
	container->__operator = this;
}

void ConfigOperator::unregisterContainer(QString container)
{
	containers[container]->__operator = nullptr;
	containers.remove(container);
}

void ConfigOperator::unregisterContainer(ConfigContainer *ctr)
{
	QString v = containers.key(ctr);
	if (!v.isEmpty()) {
		containers.remove(v);
	}
}

/* Description of the DOM.
 * |
 * +-> File DOM:
 * |   ____________________________
 * |   | quint64: table size      |
 * |   |--------------------------|
 * |   | String: Module name no 1 |
 * |   |--------------------------|
 * |   |            ...           |
 * |   |--------------------------|
 * |   | ???: Module 1 content    |
 * |   |--------------------------|
 * |   |            ...           |
 * |   |__________________________|
 * |
 * +-+
 * | +-> Module DOM:
 * | |   ____________________________
 * | |   | quint64: Container size  |
 * | |   |--------------------------|
 * | |   | ???: Check data          |
 * | |   |--------------------------|
 * | |   | ???: Internal data...    |
 * | |   |__________________________|
 * | |
 * | +-+
 * |   +-> Check data DOM:
 * |       ___________________________
 * |       | String: Module name     |
 * |       |-------------------------|
 * |       | quint64: Module version |
 * |       |_________________________|
 * |
 * +-> String DOM
 * |   __________________
 * |   | qint64: size   |
 * |   |----------------|
 * |   | char: c1       |
 * |   |----------------|
 * |   | char: c2       |
 * |   |----------------|
 * |   |       ...      |
 * |   |________________|
 * |
 * +-> Other DOMs...
 *
 */

void ConfigOperator::loadData()
{
	QFile file;
	file.setFileName(DStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/resources");
	if (!ensureLocation(file) || !file.open(QIODevice::ReadWrite)) {
		return;
	}

	QDataStream stream(&file);

	//First of all, read the table.
	QList<QString> table;
	qint64 size;
	stream >> size;

	int i = 0;
	while (i < size) {
		StringData data;
		data.read(stream);
		table.append(data.string);
		i++;
	}

	i = 0;
	while (i < table.length()) {
		quint64 s;
		stream >> s;
		if (containers.contains(table[i])) {
			if (containers[table[i]]->check(table[i], stream)) {
				containers[table[i]]->read(stream);
			} else {
				stream.skipRawData(s - containers[table[i]]->checkSize(table[i]));
			}
		} else {
			//Skip the data ;)
			stream.skipRawData(s);
		}
		i++;
	}
	file.close();
}

void ConfigOperator::writeData()
{
	QFile file(DStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/resources");
	if (!ensureLocation(file) || !file.open(QIODevice::ReadWrite | QIODevice::Truncate)) {
		return;
	}

	QDataStream stream(&file);

	//Write the table.
	QList<QString> table = containers.keys();
	qint64 size = table.size();

	stream << size;
	int i = 0;
	while (i < size) {
		StringData d(table[i]);
		d.write(stream);
		i++;
	}

	i = 0;
	while (i < size) {
		quint64 s = containers[table[i]]->dataSize(table[i]);
		stream << s;
		containers[table[i]]->writeCheck(table[i], stream);
		containers[table[i]]->write(stream);
		i++;
	}

	//Flush everything! :)
	file.flush();
	file.close();
}
