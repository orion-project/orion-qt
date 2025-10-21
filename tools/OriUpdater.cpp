#include "OriUpdater.h"

#include "../helpers/OriDialogs.h"
#include "../helpers/OriLayouts.h"

#include <QDebug>
#include <QDesktopServices>
#include <QDialog>
#include <QFormLayout>
#include <QLabel>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPushButton>
#include <QTextBrowser>

namespace Ori {

UpdateChecker::UpdateChecker(const Config &config, QObject *parent) : QObject(parent), _config(config)
{
}

void UpdateChecker::check(bool silent)
{
    // This method is called by user with silent=false
    // Even if the check is progress, now we want to show its result to the user
    _silent = silent;
    if (_network)
    {
        qDebug() << "Check is already in progress";
        return;
    }
    qDebug() << "Check for update, current version" << _config.currentVersion.str(3);
    _network = new QNetworkAccessManager(this);
    _reply = _network->get(QNetworkRequest(QUrl(_config.versionFileUrl)));
    connect(_reply, &QNetworkReply::finished, this, [this](){
        if (!_reply) return;
        auto data = _reply->readAll();
        _reply->deleteLater();
        _reply = nullptr;
        versionReceived(data);
    });
    connect(_reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::errorOccurred), this, [this](QNetworkReply::NetworkError){
        _config.statusStore->saveChecked();
        qWarning() << "Failed to get version information" << _reply->errorString();
        if (!_silent)
            Ori::Dlg::error(tr("Failed to get version information"));
        deleteLater();
    });
}

UpdateChecker::~UpdateChecker()
{
    if (_reply) delete _reply;
    if (_network) delete _network;
}

void UpdateChecker::versionReceived(QByteArray data)
{
    auto serverVersion = Ori::Version(QString::fromLatin1(data));
    if (_config.currentVersion >= serverVersion) {
        _config.statusStore->saveChecked();
        qDebug() << "No update available, version on server" << serverVersion.str(3);
        if (!_silent)
            Ori::Dlg::info(tr("You are using the most recent version"));
        deleteLater();
        return;
    }

    _reply = _network->get(QNetworkRequest(QUrl(_config.historyFileUrl)));
    connect(_reply, &QNetworkReply::finished, this, [this](){
        if (!_reply) return;
        _config.statusStore->saveChecked();
        historyReceived(_reply->readAll());
    });
    connect(_reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::errorOccurred), this, [this](QNetworkReply::NetworkError){
        _config.statusStore->saveChecked();
        qWarning() << "Failed to get history information" << _reply->errorString();
        if (!_silent)
            Ori::Dlg::error(tr("Failed to get history information"));
        deleteLater();
    });
}

void UpdateChecker::historyReceived(QByteArray data)
{
/*
History file has format:
{
  "history": [
    {
      "version": "2.1.1",
      "date": "2025-10-06",
      "changes": [
        "text": "Bux fixed"
      ]
    }
  ]
}
*/
    QJsonParseError err;
    auto doc = QJsonDocument::fromJson(data, &err);
    if (doc.isNull()) {
        qWarning() << "Failed to parse version history" << err.errorString();
        if (!_silent)
            Ori::Dlg::error(tr("Failed to parse version history"));
        deleteLater();
        return;
    }
    if (!doc["history"].isArray()) {
        qWarning() << "Unexpected data format, json array expected";
        if (!_silent)
            Ori::Dlg::error(tr("Server returned data in an unexpected format"));
        deleteLater();
        return;
    }
    auto arr = doc["history"].toArray();

    struct Release
    {
        Ori::Version ver;
        QDate date;
        QStringList changes;
    };
    QList<Release> releases;
    for (auto it = arr.cbegin(); it != arr.cend(); it++) {
        auto obj = it->toObject();
        Ori::Version version(obj["version"].toString());
        if (version <= _config.currentVersion)
            break;
        Release release {
            .ver = version,
            .date = QDate::fromString(obj["date"].toString(), Qt::ISODate),
        };
        auto changes = obj["changes"].toArray();
        for (auto ch = changes.cbegin(); ch != changes.cend(); ch++) {
            release.changes << ch->toObject()["text"].toString();
        }
        releases << release;
    }
    qDebug() << "Update available" << _config.currentVersion.str(3) << "->" << releases.first().ver.str(3);
    
    QString report;
    QTextStream stream(&report);
    QRegularExpression bugRegex;
    bool hasBugRegex = !_config.githubUsername.isEmpty() && !_config.githubProject.isEmpty();
    if (hasBugRegex)
        bugRegex = QRegularExpression(QString("https:\\/\\/github.com\\/%1\\/%2\\/issues\\/(\\d+)").arg(_config.githubUsername, _config.githubProject));
    for (auto r = releases.constBegin(); r != releases.constEnd(); r++) {
        stream << "<h3>" << r->ver.str(3) << " (" << QLocale::system().toString(r->date, QLocale::ShortFormat) << ")</h3>";
        for (QString ch : r->changes) {
            if (hasBugRegex && bugRegex.isValid()) {
                auto m = bugRegex.match(ch);
                if (m.hasMatch())
                    ch.replace(m.capturedStart(), m.capturedLength(),
                        QString("<a href='%1'>#%2</a>").arg(m.captured(), m.captured(1)));
            }
            stream << "&bull;&nbsp;&nbsp;" << ch << "<br/>";
        }
    }

    auto label = new QLabel(tr("A new version is available"));
    label->setAlignment(Qt::AlignHCenter);

    auto layout = new QFormLayout;
    layout->addRow(tr("Current version:"), new QLabel("<b>" + _config.currentVersion.str(3) + "</b>"));
    layout->addRow(tr("New version:"), new QLabel("<b>" + releases.first().ver.str(3) + "</b>"));

    auto button = new QPushButton("      " + tr("Open download page") + "      ");
    connect(button, &QPushButton::clicked, this, [this]{ 
        QString url = _config.downloadPageUrl;
        if (url.isEmpty())
            url = QString("https://github.com/%1/%2/releases").arg(_config.githubUsername, _config.githubProject);
        QDesktopServices::openUrl(url);
    });

    auto browser = new QTextBrowser;
    browser->setOpenExternalLinks(true);
    browser->setHtml(report);

    auto w = new QDialog;
    w->setAttribute(Qt::WA_DeleteOnClose);
    Ori::Layouts::LayoutV({
        label,
        layout,
        new QLabel(tr("Changelog:")),
        browser,
        Ori::Layouts::LayoutH({
            Ori::Layouts::Stretch(),
            button,
            Ori::Layouts::Stretch()
        }),
    }).useFor(w);
    w->resize(500, 400);
    w->exec();
    
    deleteLater();
}

} // namespace Ori
