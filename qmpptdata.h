#ifndef QMPPTDATA_H
#define QMPPTDATA_H

#include <QObject>
#include <QTcpSocket>
#include <QDebug>

class QmpptData : public QObject
{
    Q_OBJECT

public:

    explicit QmpptData(QObject *parent = 0);

signals:

    void statusBar(QString);
    void mpptData(QHash<QString, float>);

public slots:

    void readData();

private:

    QTcpSocket *socket;
    QHash<QString, float> _hashData;

};

#endif // QMPPTDATA_H
