#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <QRegExp>

#include "playerinterface.h"
#include "lyricsdialog.h"

LyricsDialog::LyricsDialog(QWidget *parent, PlayerInterface *player)
    : QWidget(parent) {
    m_player = player;
    ui.setupUi(this);
    setWindowFlags(Qt::Dialog);
    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_QuitOnClose, false);
    m_requestReply = 0;
    m_http = new QNetworkAccessManager(this);
    connect(m_http, SIGNAL(finished(QNetworkReply *)),
            SLOT(showText(QNetworkReply *)));
    on_updatePushButton_clicked();
}

void LyricsDialog::showText(QNetworkReply *reply) {
    ui.stateLabel->setText(tr("Done"));
    if (reply->error() != QNetworkReply::NoError) {
        ui.stateLabel->setText(tr("Error"));
        ui.textBrowser->setText(reply->errorString());
        m_requestReply = 0;
        reply->deleteLater();
        return;
    }
    QString content = QString::fromUtf8(reply->readAll().constData());

    if(m_requestReply == reply) {
        m_requestReply = 0;
        reply->deleteLater();
        QRegExp artist_rgex("<artist>(.*)</artist>");
        artist_rgex.setMinimal(true);
        QRegExp song_rgex("<song>(.*)</song>");
        song_rgex.setMinimal(true);
        QRegExp lyrics_rgex("<lyrics>(.*)</lyrics>");
        lyrics_rgex.setMinimal(true);
        QRegExp url_rgex("<url>(.*)</url>");
        url_rgex.setMinimal(true);

        if(artist_rgex.indexIn(content) < 0 || song_rgex.indexIn(content) < 0 ||
           lyrics_rgex.indexIn(content) < 0 || url_rgex.indexIn(content) < 0) {
            ui.textBrowser->setHtml("<b>" + tr("Error") + "</b>");
            return;
        }
        else if(lyrics_rgex.cap(1) == "Not found") {
            ui.textBrowser->setHtml("<b>" + tr("Not found") + "</b>");
            return;
        }
        else {
            m_artist = artist_rgex.cap(1);
            m_title = song_rgex.cap(1);
        }

        QString temp = url_rgex.cap(1).toAscii();
        temp.replace("http://lyrics.wikia.com/",
                     "http://lyrics.wikia.com/index.php?title=");
        temp.append("&action=edit");

        QUrl url = QUrl::fromEncoded(temp.toAscii());
        QString referer = url_rgex.cap(1);
        QNetworkRequest request;
        request.setUrl(url);
        request.setRawHeader("Referer", referer.toAscii());
        ui.stateLabel->setText(tr("Receiving"));
        m_http->get(request);
        reply->deleteLater();
        return;
    }
    content.replace("&lt;", "<");
    QRegExp lyrics_rgex("<lyrics>(.*)</lyrics>");
    lyrics_rgex.indexIn(content);

    QString text = "<h2>" + m_artist + " - " + m_title + "</h2>";
    QString lyrics = lyrics_rgex.cap(1);
    lyrics = lyrics.trimmed();
    lyrics.replace("\n", "<br>");
    if(lyrics.isEmpty())
        ui.textBrowser->setHtml("<b>" + tr("Not found") + "</b>");
    else {
        text += lyrics;
        ui.textBrowser->setHtml(text);
    }
    reply->deleteLater();
}

void LyricsDialog::on_artistLineEdit_returnPressed() {
    search();
}

void LyricsDialog::on_titleLineEdit_returnPressed() {
    search();
}

void LyricsDialog::on_updatePushButton_clicked() {
    ui.artistLineEdit->setText(m_player->artist());
    ui.titleLineEdit->setText(m_player->title());
    if(!m_player->artist().isEmpty())
        search();
}

void LyricsDialog::search() {
    ui.stateLabel->setText(tr("Receiving"));
    setWindowTitle(QString(tr("%1 - %2")).arg(ui.artistLineEdit->text())
                   .arg(ui.titleLineEdit->text()));
    QNetworkRequest request;
    request.setUrl(QUrl("http://lyrics.wikia.com/api.php?action=lyrics&artist="+
                        ui.artistLineEdit->text() + "&song=" +
                        ui.titleLineEdit->text() + "&fmt=xml"));
    request.setRawHeader("User-Agent", QString("Mozilla/5.0").toAscii());
    m_requestReply = m_http->get(request);
}