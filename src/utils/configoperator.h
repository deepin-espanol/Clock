#ifndef CONFIGOPERATOR_H
#define CONFIGOPERATOR_H

#include <DApplication>
#include <QMap>

/*
 * These classes provide abstractions to read and save data automatically.
 * You can use them when you have internal application data that is user defined.
 * For example, the timezones list and alarms one use them to save their models' content.
 */

class ConfigOperator;

/**
 * @brief Abstract class defining basis of a data that can be saved.
 */
class ConfigData
{
public:
	explicit ConfigData();
	virtual ~ConfigData();

	/**
	 * @brief Load the data of the stream into the instance.
	 * @param stream Input file stream.
	 */
	virtual void read(QDataStream &stream) = 0;
	/**
	 * @brief Write the data stored in the instance on the file.
	 * @param stream Output file stream.
	 */
	virtual void write(QDataStream &stream) = 0;
	/**
	 * @brief Get the size of the data when written on file.
	 * @param name Used for ConfigContainer.
	 * @return Total size.
	 */
	virtual quint64 dataSize(QString name = "") = 0;
};

/**
 * @brief Abstract class representing a group of ConfigData, or sub parts.
 */
class ConfigContainer : public ConfigData
{
public:
	~ConfigContainer();

	/**
	 * @brief Get the current container's version.
	 */
	int version();
	/**
	 * @brief Read a beginning of the data section of the container and check if it can be read.
	 * @param name The written container's instance name.
	 * @param stream Input file stream.
	 * @return If it will handle a read OP.
	 */
	virtual bool check(QString name, QDataStream &stream);
	/**
	 * @brief Write the check part of the container (see check(QString, QDataStream&))
	 * @param name The container's instance name.
	 * @param stream The output file stream.
	 */
	virtual void writeCheck(QString name, QDataStream &stream);
	/**
	 * @brief Get the size of the data when written on file, including check data.
	 * @param name Used for ConfigContainer.
	 * @return Total size.
	 */
	quint64 dataSize(QString name = "") override;

protected:
	/**
	 * @brief Request the data to be written of file.
	 */
	void save();
	quint64 checkSize(QString name);

	/**
	 * @brief The container's version (user defined).
	 */
	qint64 containerVersion = 0;

private:
	//Used to perform request to write/read data.
	ConfigOperator *__operator = nullptr;
	friend class ConfigOperator;
};

/**
 * @brief Implementation of ConfigData to store a QString.
 */
class StringData : public ConfigData
{
public:
	explicit StringData(QString t = "");
	StringData(const StringData &d);

	QString string;

	void read(QDataStream &stream) override;
	void write(QDataStream &stream) override;
	quint64 dataSize(QString name = "") override;
};

//Usual operators.
QDataStream &operator<<(QDataStream &stream, ConfigData *d);
QDataStream &operator>>(QDataStream &stream, ConfigData *d);

/**
 * @brief Static instance performing the save/read operations on files and managing the containers.
 */
class ConfigOperator : public QObject
{
	Q_OBJECT
public:
	explicit ConfigOperator(Dtk::Widget::DApplication *app, bool autoSave = true);
	~ConfigOperator();

	/**
	 * @brief Get the instance (if any).
	 */
	static ConfigOperator *instance();

	/**
	 * @brief Register a new container for the name, "attaching" it to the name.
	 * @param name The container's name.
	 * @param container The container instance.
	 */
	void registerContainer(QString name, ConfigContainer *container);
	/**
	 * @brief Unregister the container attached to the name.
	 * @param container Container's name.
	 */
	void unregisterContainer(QString container);

	/**
	 * @brief Load the data from the file to the containers.
	 */
	void loadData();
	/**
	 * @brief Write the data of the containers to the file.
	 */
	void writeData();

protected:
	//Used by ConfigContainer on destruction.
	void unregisterContainer(ConfigContainer *ctr);

private:
	//Save data when app gets closed.
	bool m_autoSave;
	//Registered containers.
	QMap<QString, ConfigContainer*> containers;
	//Static instance pointer.
	inline static ConfigOperator *inst = nullptr;

	friend class ConfigContainer;
};

#endif // CONFIGOPERATOR_H
