#ifndef QADCDATA_H
#define QADCDATA_H

#include <QObject>

class qadcdata : public QObject
{
    Q_OBJECT
public:
    explicit qadcdata(QObject *parent = 0);

    QList<int> adcData;             // ADC data

signals:

public slots:

    void readData();

private:

    void filterData();

};

#endif // QADCDATA_H
