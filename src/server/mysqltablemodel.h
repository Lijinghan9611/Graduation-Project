#ifndef MYSQLTABLEMODEL_H
#define MYSQLTABLEMODEL_H

#include <QSqlTableModel>

class MySqlTableModel : public QSqlTableModel
{
public:
    MySqlTableModel();
    QVariant data(const QModelIndex &idx, int role = Qt::DisplayRole) const;
private:
};

#endif // MYSQLTABLEMODEL_H
