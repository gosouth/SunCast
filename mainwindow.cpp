#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFile>
#include <QtCore/qmath.h>

using namespace std;

/*==============================================================================

    To start Qt: cd /home/ricardo/Qt/Tools/QtCreator/bin/qtcreator.sh
    ADC used:    /sys/bus/iio/devices/iio:device0/in_voltage5_raw
    Display:    480x272
    pkill -f SunCast

================================================================================*/

#define FORECAST_URL "http://api.thingspeak.com/apps/thinghttp/send_request?api_key=U2AZ2JEEYH4LJTXS"

#define BAT_VMIN 23.8
#define BAT_VMAX 26.0               // 25.4


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->statusBar()->showMessage("Initializing");

    // -- change LCD color -------------

    ui->lcdVb->setAutoFillBackground(true);
    ui->lcdSoC->setAutoFillBackground(true);
    ui->lcdPower->setAutoFillBackground(true);

    QPalette Pal = ui->lcdVb->palette();

    Pal.setColor(QPalette::Normal, QPalette::WindowText, Qt::green);
    Pal.setColor(QPalette::Normal, QPalette::Window, Qt::black);
    ui->lcdVb->setPalette(Pal);
    ui->lcdPower->setPalette(Pal);

    Pal.setColor(QPalette::Normal, QPalette::WindowText, Qt::yellow);
    Pal.setColor(QPalette::Normal, QPalette::Window, Qt::black);
    ui->lcdSoC->setPalette(Pal);

    ui->lcdSoC->display( "--" );
    ui->lcdVb->display( "--" );
    ui->lcdPower->display( "--" );

  //  ui->SunDayLabel->setText( "Quilaco" );

    webdata.setUrl( FORECAST_URL );
    webdata.startDownload();

    // == task timer =============================

//    connect( &taskTimer, SIGNAL(tensecInterval()), this, SLOT(getVoltage()) );

    connect( &taskTimer, SIGNAL(tensecInterval()), &readMppt, SLOT(readData()) );

 //   connect( &taskTimer, SIGNAL(minuteInterval()), this, SLOT(logVoltage()) );

    connect( &taskTimer, SIGNAL(hourInterval()), &webdata, SLOT(startDownload()) );


    // == signals ===================================================================

    connect( this, SIGNAL(sunHoursUpdated(QList<float>)), ui->chart, SLOT(onDataChanged(QList<float>)) );

    connect( &webdata, SIGNAL(readyDownload()), this, SLOT(getWebForecast()));

    connect( &readMppt, SIGNAL(mpptData(QHash<QString, float> )), this, SLOT(displayMppt( QHash<QString, float>)) );
    connect( &readMppt, SIGNAL(mpptData(QHash<QString, float> )), this, SLOT(logData( QHash<QString, float>)) );

    connect( &readMppt, SIGNAL(statusBar(QString)), this->statusBar(), SLOT(showMessage(QString)));

}

MainWindow::~MainWindow()
{

    delete ui;
}


/*----------------------------------------------------------------------
;
;   Downlad and parse Sun days forecast from Web
;
-----------------------------------------------------------------------*/

#define MAXDAYS 7  // it is defined as a 7 days forecast old fashion parser

void MainWindow::getWebForecast()
{

    this->statusBar()->showMessage("Loading web Data");

    QString sForecast = webdata.downloadedData();

    // search to first keyword and position on sun "td class"
    int nPos =  sForecast.indexOf("<!-- Amount of Sun -->");
    if( nPos==-1) {
        this->statusBar()->showMessage("Error parsing web Data");
        return;
    }

    nPos = sForecast.indexOf("<td class", nPos );
    nPos = sForecast.indexOf( "<td class=", nPos+1 );

    int nl;
    QString sDay;
    SunHours.clear();

    for( int nDay=0; nDay<MAXDAYS; nDay++) {

        nPos = sForecast.indexOf( "<td class=", nPos );
        nl = sForecast.indexOf( "</td>", nPos ) - nPos;

        sDay = sForecast.mid( nPos, nl) ;
        nPos += nl;

        QStringList sList = sDay.split(0x0a);

        SunHours.append(sList.at(2).trimmed().toFloat());
    }

//    qDebug() << "sunhours" << SunHours;

    emit sunHoursUpdated(SunHours);

   // StatusBar("Ready");

}

/*----------------------------------------------------------------------
;
;   log SunCast data
;
;   Date Time, SunHours, Vb, Ib, Va, Ia
;
-----------------------------------------------------------------------*/

void MainWindow::logData(const QHash<QString, float> &mpptData )
{

    if( mpptData.size()==0 ) return;

    QString sLog;

    sLog.append( QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm") );
    sLog.append( "," );

    sLog.append( QString::number(SunHours.at(0),'g',3) );
    sLog.append( "," );

    sLog.append( QString::number(mpptData.value("Vb"),'g',4));
    sLog.append( "," );

    sLog.append( QString::number(mpptData.value("Ib"),'g',3));
    sLog.append( "," );

    sLog.append( QString::number(mpptData.value("Va"),'g',4));
    sLog.append( "," );

    sLog.append( QString::number(mpptData.value("Ia"),'g',3));
    sLog.append( "," );

    sLog.append( QString::number(mpptData.value("Tb"),'g',2));
//    sLog.append( "," );

    // -- write append into log file ---

    qDebug() << sLog;

    sLog.append( "\n");

    QFile file( "suncast.log" );
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append)) {
    this->statusBar()->showMessage("Error: writing log file");
        return;
    }

    QTextStream out(&file);

    if( file.size()==0 )
        out << "Date Time, SunHours, Vb, Ib, Va, Ia, Tb\n";

    out << sLog;

    file.close();

}

/*----------------------------------------------------------------------
;
;   Display Mppt data
;
-----------------------------------------------------------------------*/

void MainWindow::displayMppt(const QHash<QString, float> &mpptData )
{

    // qDebug() << "voltages" << mpptData;

    if( mpptData.size()==0 ) {
        ui->lcdVb->display( "---" );
        ui->lcdSoC->display( "---" );
        ui->lcdPower->display( "---" );

        return;
    }

    ui->lcdVb->display( mpptData.value("Vb") );
    ui->lcdPower->display( mpptData.value("Vb") * mpptData.value("Ib") );

    QString s;

    s.sprintf( "Va: %.2f", mpptData.value("Va"));
    ui->VaLabel->setText( s );

    s.sprintf( "Ia: %.1f", mpptData.value("Ia"));
    ui->IaLabel->setText( s );

    s.sprintf( "Ib: %.1f", mpptData.value("Ib"));
    ui->IbLabel->setText( s );

    s.sprintf( "Tb: %.0f", mpptData.value("Tb"));
    ui->TbLabel->setText( s );

    // == calculate SoC ================

    float fSoC;

    fSoC = 100*(mpptData.value("Vb")-BAT_VMIN)/(BAT_VMAX-BAT_VMIN);

    if( fSoC<0.) fSoC = 0.;
    if( fSoC>100.) fSoC = 100;

    ui->lcdSoC->display( fSoC );


}


void MainWindow::on_pushButton_clicked()
{

}
