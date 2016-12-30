#include "qchart.h"

#include <QPainter>
#include <QDebug>

// QVector<QVector<QString>>
// QHash<QString, Qstring> "name", "Ricardo"; "name", "Sergey"
// QVector <QHash<QString, float>>
// Qhash<QString, float>. 1. "Mon", 5.6; "Thu", 10; "Wed"
// QVariant

//struct SunDay;
//QHash<QString, QVariant>
// "DownloadTime", QDateTime
        // onDataChanged(const SunDay &data)

Qchart::Qchart(QWidget *parent) :
    QWidget(parent)
{

}

void Qchart::onDataChanged(const QList<float> &data)
{
    _data = data;
   emit repaint();
}

/*------------------------------------------------------------------------------------------------
;
;	chart paintings - All paints doing only in this fuction
;
;------------------------------------------------------------------------------------------------*/

void Qchart::paintEvent(QPaintEvent *)
{

    QPainter p(this);
    QPen pen(Qt::black);

    if( _data.size() == 0 ) return;

    // qDebug() << "== paint chart : " << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm");

    // -- paint background ----------------------------

    QColor bgColor = palette().color(QPalette::Window);
    pen.setBrush( QBrush(bgColor) );

    QColor kool(92, 92, 92 ); //, 200);
    p.setPen(pen);
    p.setBrush(kool);

    p.drawRect(0, 0, rect().width(), rect().height());

    // -- write update time

    p.setPen(Qt::white);
    p.setFont(QFont("Verdana", 10));

    QString s = "Sunshine Forecast, ";
    s += QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm");
    p.drawText( 10, 15, s );

    // == paint the chart =============

    int res = rect().height()/12;     // max 14 hours sun by day

    int dx, dy, colWidth;
    colWidth = rect().width() / _data.size() - 1;

    for (int i=0; i < _data.size(); i++) {

        dx = colWidth * i;
        dy = res * _data.at(i);

        QRect rectBar( dx+5, rect().height()-dy, colWidth-6, dy );

        // -- Border color for each column in distribution
        p.setPen(QColor(0, 0x80, 0xff));
        p.drawRect( rectBar );

        // -- Fill color for each column in distribution
        p.setBrush(QBrush(Qt::blue));
        p.drawRect( rectBar );

        // -- write day value label
        p.setPen(Qt::white);
        p.setFont(QFont("Verdana", 12));
        p.drawText( dx+12, rect().height() - 3, QString("%1").arg( _data.at(i), 0, 'g', 2) );
    }

}
