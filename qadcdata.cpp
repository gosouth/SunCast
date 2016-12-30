#include "qadcdata.h"

#include <QFile>
#include <QtCore/qmath.h>
#include <QDebug>

#define LDR_PATH "/sys/bus/iio/devices/iio:device0/in_voltage5_raw"
#define ANARES (1.8/4096)       // use here a correction factor

qadcdata::qadcdata(QObject *parent) : QObject(parent)
{

}

/*----------------------------------------------------------------------
;
;   read ADC voltage data
;
-----------------------------------------------------------------------*/

void qadcdata::readData()
{

    QByteArray line;

    QFile file( LDR_PATH );
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    //this->statusBar()->showMessage("ADC error");
        return;
    }

    while ( !file.atEnd() ) {
        line = file.readLine();
        break;
    }

    file.close();

    line = line.simplified();
    if( line.size()==0 ) return;

    adcData.append( line.toInt() );
//    this->statusBar()->showMessage( "ADC = " + line );


    // == display voltage and load in percent

    float volt = line.toInt() * ANARES * 17.25;

    //ui->lcdVb->display( volt );

    float perc;

    //perc = 100*(volt-BAT_VMIN)/(BAT_VMAX-BAT_VMIN);

    if( perc<0.) perc = 0.;
    if( perc>100.) perc = 100;

    //ui->lcdSoC->display( perc );

}

void qadcdata::filterData()
{
    if( adcData.size()==0 ) return;

    // == make some stats to filter the outlaiers ==========

    float pSum = 0;
    float pMax = 0;
    float pMin = 0;
    float pMean = 0;
    float pStdDev = 0;
    float pVariance = 0;

    pMin = 99999.9;
    pMax = -pMin;

    for (int i = 0; i < adcData.size(); i++) {
       float locCurrentValue = adcData.at(i);
       pMin = std::min(locCurrentValue, pMin);
       pMax = std::max(locCurrentValue, pMax);
       pSum += locCurrentValue;
    }

    // -- calculate std dev ---------------------

    pMean = pSum / adcData.size();
    for (int i = 0; i < adcData.size(); i++) {
       pVariance += qPow(adcData.at(i) - pMean, 2);
    }

    pVariance = pVariance / (adcData.size() - 1);
    pStdDev = sqrt(pVariance);

    qDebug() << adcData << "stdev = " <<  pStdDev;

    // -- remove the outliers, brutal out of 1th StdDev -------------------

redo:

    for (int i = 0; i < adcData.size(); i++) {
        if( adcData.at(i)<pMean-pStdDev ) { adcData.removeAt(i); goto redo; }
        if( adcData.at(i)>pMean+pStdDev ) { adcData.removeAt(i); goto redo; }
        }

    // == get a cleaner average ============================

    qDebug() << adcData;

    if(adcData.size()==0) return;

    int avg = 0;
    for( int n=0; n < adcData.size(); ++ n )
        avg += adcData.at(n);

    avg = avg / adcData.size();
    adcData.clear();

}
