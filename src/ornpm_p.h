#pragma once

#define SSU_CONFIG_PATH        QStringLiteral("/etc/ssu/ssu.ini")
#define SSU_REPOS_GROUP        QStringLiteral("repository-urls")
#define SSU_DISABLED_KEY       QStringLiteral("disabled-repos")

#define SSU_SERVICE            QStringLiteral("org.nemo.ssu")
#define SSU_PATH               QStringLiteral("/org/nemo/ssu")
#define SSU_METHOD_ADDREPO     "addRepo"
#define SSU_METHOD_MODIFYREPO  "modifyRepo"

#define PK_SERVICE      QStringLiteral("org.freedesktop.PackageKit")
#define PK_PATH         QStringLiteral("/org/freedesktop/PackageKit")
#define PK_TR_INTERFACE QStringLiteral("org.freedesktop.PackageKit.Transaction")

#define PK_METHOD_GETUPDATES        "GetUpdates"
#define PK_METHOD_INSTALLPACKAGES   "InstallPackages"
#define PK_METHOD_INSTALLFILES      "InstallFiles"
#define PK_METHOD_REMOVEPACKAGES    "RemovePackages"
#define PK_METHOD_REPOSETDATA       "RepoSetData"
#define PK_METHOD_RESOLVE           "Resolve"
#define PK_METHOD_REFRESHCACHE      "RefreshCache"

#define PK_FLAG_NONE  quint64(0)

#define REPO_URL_TMPL  QStringLiteral("https://sailfish.openrepos.net/%0/personal/main")


#include "ornpm.h"
#include "orninstalledpackage.h"

#include <private/qobject_p.h>
#include <QSet>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusPendingCallWatcher>


class OrnPmPrivate : public QObjectPrivate
{
    Q_DISABLE_COPY(OrnPmPrivate)
    Q_DECLARE_PUBLIC(OrnPm)

public:
    OrnPmPrivate() = default;
    ~OrnPmPrivate() override = default;

    void initialise();
    QDBusInterface *transaction(const QString &item = QString());
    void preparePackageVersions(const QString &packageName);
    bool enableRepos(bool enable);
    void removeAllRepos();
    void onRepoModified(const QString &repoAlias, OrnPm::RepoAction action);
    OrnInstalledPackageList prepareInstalledPackages(const QString &packageName);

    // Check for updates
    void getUpdates();
    void onPackageUpdate(quint32 info, const QString& packageId, const QString &summary);
    void onGetUpdatesFinished(quint32 status, quint32 runtime);
    // Install package
    void onPackageInstalled(quint32 exit, quint32 runtime);
    // Remove package
    void onPackageRemoved(quint32 exit, quint32 runtime);
    // Update package
    void onPackageUpdated(quint32 exit, quint32 runtime);
    // Refresh repos
    void refreshNextRepo(quint32 exit, quint32 runtime);

    // <alias, enabled>
    using RepoHash   = QHash<QString, bool>;
    using StringSet  = QSet<QString>;
    using StringHash = QHash<QString, QString>;

    bool            initialised{false};
#ifdef QT_DEBUG
    quint64         refreshRuntime{0};
#endif
    QString         solvPathTmpl;
    StringSet       archs;
    QDBusInterface  *ssuInterface{nullptr};
    QDBusInterface  *pkInterface{nullptr};
    RepoHash        repos;
    StringHash      installedPackages;
    StringHash      updatablePackages;
    StringHash      newUpdatablePackages;
    QHash<QString, OrnPm::Operation> operations;
    QHash<QObject *, QString> transactionHash;
    QStringList     reposToRefresh;
    QString         forceRefresh;
};
