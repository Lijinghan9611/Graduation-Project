#ifndef QUESTION_H
#define QUESTION_H
#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>

class Question : public QLabel
{
    Q_OBJECT
public:
    Question(QWidget *parent = 0);
    QLabel * m_pQuestion;
    QLineEdit * m_pAnswer ;
    QPushButton *button;

signals:
    void AnswerMg(const QString&);
public slots:
    void onClick();

};

#endif // QUESTION_H
