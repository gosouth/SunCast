#include "qtasktimer.h"
#include <QDebug>

QTaskTimer::QTaskTimer(QObject *parent) :
    QObject(parent)
{

    connect( &taskTimer, SIGNAL(timeout()), this, SLOT(onTaskManager()));

    taskTimer.setInterval( 1000 );       // enter each 10 seconds
    taskTimer.start();
}

void QTaskTimer::onTaskManager()
{

    if( QTime::currentTime().second() % 10 == 0 ) {
//        qDebug() << "time" << QTime::currentTime();
        emit tensecInterval();
    }

    if (QTime::currentTime().second() == 0) {
//        qDebug() << "current min" << QTime::currentTime();
        emit minuteInterval();
    }

    if (QTime::currentTime().minute()==0 && QTime::currentTime().second()==0) {
        qDebug() << "current hour" << QTime::currentTime();
        emit hourInterval();
    }
}

