#include "mysqltablemodel.h"
#include <QColor>
#include <QFont>
#include <QDebug>

MySqlTableModel::MySqlTableModel()
{
}
QVariant MySqlTableModel::data(const QModelIndex &idx, int role) const
{
    QVariant value  = QSqlTableModel::data(idx,role);

    if(Qt::TextAlignmentRole == role)  //居中
    {
        value = int(Qt::AlignCenter | Qt::AlignHCenter);
        return value;
    }
    if(Qt::ForegroundRole == role)  //改变内容颜色
    {
        if("已签到" == idx.data().toString().trimmed())
            return QVariant(QColor(50,205,50));
        else if("缺勤" == idx.data().toString().trimmed())
            return QVariant(QColor(255,0,0));
        else if("早退" == idx.data().toString().trimmed())
            return QVariant(QColor( 147,112,219));

    }
    if(Qt::FontRole == role)  //改变字体
    {
//        if(3 == idx.column())
//        {
//            QFont font;
//            font.setBold(true);
//            return QVariant(font);
//        }
    }

    return value;
}
