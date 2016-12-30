#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>           // for a timer that periodically reads the temp
#include <QDateTime>        // to find out the date/time of the sample
#include <QDebug>

#include "qwebdata.h"
#include "qtasktimer.h"
#include "qmpptData.h"


namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

signals:
    void sunHoursUpdated(const QList<float>);

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QList<float> SunHours;          // sun hours list
    QList<int> adcData;             // ADC data

private slots:

    void on_pushButton_clicked();
    void getWebForecast();

public slots:

    void displayMppt(const QHash<QString, float> &mpptData);
    void logData(const QHash<QString, float> &mpptData);

private:

    Ui::MainWindow *ui;

    QwebData webdata;

    QmpptData readMppt;

    QTaskTimer taskTimer;

    int readAnalog(void);
    void displayForecast();

    void displayVoltage(QString sADC);


};

#endif // MAINWINDOW_H
