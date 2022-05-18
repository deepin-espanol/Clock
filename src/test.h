#ifndef TEST_H
#define TEST_H

#include <QDBusAbstractInterface>
#include <QDBusAbstractAdaptor>
#include <QDBusPendingReply>
#include <QMetaProperty>
#include <QDBusVariant>

#include <iostream>
#include <cxxabi.h>

/*
class Pong: public QDBusAbstractAdaptor
{
	Q_OBJECT
	Q_CLASSINFO("D-Bus Interface", "org.example.QtDBus.ComplexPong.Pong")
	Q_PROPERTY(QString value READ value WRITE setValue)
public:
	QString m_value;
	QString value() const;
	void setValue(const QString &newValue);

	Pong(QObject *obj) : QDBusAbstractAdaptor(obj)
	{ }
signals:
	void aboutToQuit();
public slots:
	QDBusVariant query(const QString &query);
	Q_NOREPLY void quit();
};
*/

class DBusInterfacer : public QDBusAbstractAdaptor
{
	Q_OBJECT
	Q_CLASSINFO("D-Bus Interface", "com.deepin-espanol.Clock")
	/*Q_CLASSINFO("D-Bus Introspection", ""
				"  <interface name=\"com.deepin-espanol.Clock\">\n"
				"    <method name=\"trigger\"></method>\n"
				"  </interface>\n"
				"")*/
public:
	Q_PROPERTY(QList<QVariant> trigger READ triggers WRITE trigger)

	inline DBusInterfacer(const QString &service, const QString &path, const QString &iname, const QDBusConnection &connection = QDBusConnection::sessionBus(), QObject *parent = 0)
		: QDBusAbstractAdaptor(parent)//(service, path, iname.toUtf8(), connection, parent)
	{
		/*QDBusConnection::sessionBus().connect(this->service(), this->path(),
		"org.freedesktop.DBus.Properties", "PropertiesChanged", "sa{sv}as",
											  this, SLOT(__propertyChanged__(QDBusMessage)));*/
	}
	inline ~DBusInterfacer()
	{
		/*QDBusConnection::sessionBus().disconnect(service(), path(),
		"org.freedesktop.DBus.Properties", "PropertiesChanged", "sa{sv}as",
												 this, SLOT(propertyChanged(QDBusMessage)));*/
	}

	inline void trigger(QList<QVariant> d)
	{
		std::cout << "Triggered!" << std::endl;
		QString prop = d.first().toString();
		d.removeFirst();
		handleMessage(prop, d);
	}

	inline QList<QVariant> triggers()
	{
		return {};
	}

	inline void attach(QString name, QFunctionPointer ptr)
	{
		std::cout << "Attached: " << name.toStdString() << std::endl;;
		ensureRegisterMethod(name);
		simple_handlers[name].append(ptr);
	}

	inline void attach(QString name, QObject *ptr, QString func)
	{
		std::cout << "Attached: " << name.toStdString() << std::endl;;
		ensureRegisterMethod(name);
		object_handlers[name].append(QPair<QObject *, QString>(ptr, func));
	}

	inline void attach(QString name, QObject *ptr, QMetaMethod &meth)
	{
		std::cout << "Attached: " << name.toStdString() << std::endl;;
		ensureRegisterMethod(name);
		meta_handlers[name].append(QPair<QObject *, QMetaMethod>(ptr, meth));
	}

	template <typename Func2>
	inline typename std::enable_if<int(QtPrivate::FunctionPointer<Func2>::ArgumentCount) <= 1 && !QtPrivate::FunctionPointer<Func2>::IsPointerToMemberFunction, QMetaObject::Connection>::type
	attach(QString name, const QObject *context, Func2 slot)
	{
		ensureRegisterMethod(name);
		int status = 0;
		char *demangled = abi::__cxa_demangle(typeid(Func2).name(), 0, 0, &status);
		std::cout << "Attached: " << demangled << std::endl;;
		return connect(this, &DBusInterfacer::trigger, context, [name, context, slot](QString n, QVariant msg)
		{
			if (name == n) {
				if (int(QtPrivate::FunctionPointer<Func2>::ArgumentCount) == 1 /*&& QtPrivate::AreArgumentsCompatible<void, >*/) {
					QtPrivate::FunctorCall<QtPrivate::IndexesList<1>, QtPrivate::List<QDBusMessage>, void, Func2>::call(slot, context, msg);
				} else {
					QtPrivate::FunctorCall<QtPrivate::IndexesList<0>, QtPrivate::List<>, void, Func2>::call(slot, context);
				}
			}
		},
		Qt::DirectConnection);
	}

	inline void ensureRegisterMethod(QString mN)
	{
		if (simple_handlers.keys().contains(mN) || meta_handlers.keys().contains(mN) || object_handlers.keys().contains(mN)) {
			return;
		}
		setProperty(mN.toUtf8(), "");
	}

Q_SIGNALS:
	void triggered(QString, QList<QVariant>);

protected:
	void handleMessage(QString prop, QList<QVariant> data)
	{
		QList<QFunctionPointer> funcs = simple_handlers[prop];
		foreach(const QFunctionPointer &ptr, funcs) {
			ptr();
		}

		QList<QPair<QObject *, QMetaMethod>> metas = meta_handlers[prop];
		foreach(auto meth, metas) {
			meth.second.invoke(meth.first, Qt::DirectConnection, QGenericReturnArgument(meth.second.typeName()), QGenericArgument("QList<QVariant>", &data));
		}

		QList<QPair<QObject *, QString>> objs = object_handlers[prop];
		foreach(auto obj, objs) {
			obj.first->metaObject()->invokeMethod(obj.first,
												  obj.second.toUtf8(),
												  Qt::DirectConnection,
												  QGenericReturnArgument(obj.first->metaObject()
																			->method(obj.first->metaObject()
																				->indexOfMethod(obj.second.toUtf8()
																				)
																			)
																			.typeName()
																		 ),
												  QGenericArgument("QList<QVariant>", &data));
		}

		Q_EMIT triggered(prop, data);
	}

private:
	QMap<QString, QList<QFunctionPointer>> simple_handlers;
	QMap<QString, QList<QPair<QObject *, QMetaMethod>>> meta_handlers;
	QMap<QString, QList<QPair<QObject *, QString>>> object_handlers;
};

#endif // TEST_H
