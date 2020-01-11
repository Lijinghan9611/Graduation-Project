#ifndef COMBOBOXITEM_H
#define COMBOBOXITEM_H

#include <QWidget>
#include <QLabel>
#include <QMouseEvent>
#include <QPushButton>

class ComboboxItem : public QWidget
{
    Q_OBJECT
public:
    explicit ComboboxItem(QWidget *parent = 0);
    QLabel* label;
    QPushButton *button;
    QLabel * head;
    QLabel * name;

public:
    void mouseReleaseEvent(QMouseEvent*);

signals:
    void itemClick(const QString&,const QString&);
    void removeItem(const QString&,const QString&);

public slots:
    void removeItem();
};

#endif // COMBOBOXITEM_H
