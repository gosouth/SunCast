#ifndef QWEBDATA
#define QWEBDATA_H

#include <QObject>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

class QwebData: public QObject
{
    Q_OBJECT

    signals:
       void readyDownload();

    public:
        explicit QwebData();

        QByteArray downloadedData() const;
        void setUrl(const QString &url);

    public slots:
        void startDownload();

    private slots:
        void webDataDownloaded(QNetworkReply* pReply);

    private:
        QUrl _url;
        QNetworkAccessManager m_WebCtrl;
        QByteArray m_webData;
};

#endif // QWEBDATA
