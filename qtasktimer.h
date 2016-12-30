#ifndef QTASKTIMER_H
#define QTASKTIMER_H

#include <QObject>
#include <QTimer>
#include <QTime>

class QTaskTimer : public QObject
{
    Q_OBJECT

signals:

    void tensecInterval();
    void minuteInterval();
    void hourInterval();

public:
    explicit QTaskTimer(QObject *parent = 0);

private slots:
    void onTaskManager();

private:

    int _interval;
    QTimer taskTimer;

};

#endif // QTASKTIMER_H
