#include "qwebdata.h"

QwebData::QwebData()
{
    connect(&m_WebCtrl, SIGNAL (finished(QNetworkReply*)), this, SLOT (webDataDownloaded(QNetworkReply*)));
}

void QwebData::webDataDownloaded(QNetworkReply* pReply) //fileDownloaded
{
    m_webData = pReply->readAll();
    pReply->deleteLater();
    emit readyDownload();
}

QByteArray QwebData::downloadedData() const
{
    return m_webData;
}

void QwebData::setUrl(const QString &url)
{
    _url.setUrl(url);
}

void QwebData::startDownload()           // download
{
    m_WebCtrl.get(QNetworkRequest(_url));
}
