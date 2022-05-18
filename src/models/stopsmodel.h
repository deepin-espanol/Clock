#ifndef STOPSMODEL_H
#define STOPSMODEL_H

#include "src/widgets/advancedclasses.h"

/**
 * @brief Implementation of the model used for the stops or 'laps'.
 */
class StopsModel : public AdvancedItemModel
{
	Q_OBJECT
public:
	explicit StopsModel(QObject *parent = nullptr);
	~StopsModel();

	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
	QModelIndex parent(const QModelIndex &) const override;

	//Enums used to retrieve data from the model.
	enum Roles {
		Text = 1,
		Digits,
		Number,
		Color
	};

	enum Colors {
		Best = Roles::Color +1,
		Worst,
		Normal
	};

	void addData(const uint delta);
	void removeData(const int pos);
	/**
	 * @brief Updates internal data, e.g. after adding/removing deltas.
	 */
	void actualise();

	void clear() override;
	void handlePress(QVariant v) override;
	void handleRelease(QVariant v) override;

	//Convert a raw delta into a wonderful text.
	QString deltaText(uint delta) const;
private:
	QList<uint> m_tps;
	int m_worst = 0;
	int m_best = 0;
	int m_maxDigits;
};

Q_DECLARE_METATYPE(StopsModel::Colors);

#endif // STOPSMODEL_H
