#ifndef ORI_UPDATER_H
#define ORI_UPDATER_H

#include <QObject>

#include "../core/OriVersion.h"

QT_BEGIN_NAMESPACE
class QNetworkAccessManager;
class QNetworkReply;
QT_END_NAMESPACE

namespace Ori {

class IUpdateStatusStore
{
public:
    virtual void saveChecked() = 0;
};

class UpdateChecker : public QObject
{
    Q_OBJECT

public:
    struct Config {
        Ori::Version currentVersion;
        QString versionFileUrl;
        QString historyFileUrl;
        QString downloadPageUrl;
        QString githubUsername; // for making GitHub links
        QString githubProject; // for making GitHub links
        std::shared_ptr<IUpdateStatusStore> statusStore;
    };
    
    UpdateChecker(const Config &config, QObject *parent = nullptr);
    ~UpdateChecker();
    
    void check(bool silent);
    
private:
    bool _silent;
    Config _config;
    QNetworkAccessManager* _network = nullptr;
    QNetworkReply* _reply = nullptr;
    
    void versionReceived(QByteArray data);
    void historyReceived(QByteArray data);
};

} // namespace Ori

#endif // ORI_UPDATER_H
