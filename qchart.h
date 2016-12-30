#ifndef QCHART_H
#define QCHART_H

#include <QWidget>
#include <QDateTime>

class Qchart : public QWidget
{
    Q_OBJECT

public:

    explicit Qchart(QWidget *parent = 0);
    void paintEvent(QPaintEvent *);

    signals:

public slots:

    void onDataChanged(const QList<float> &data);

private:
    QList<float> _data;

};

#endif // QCHART_H
