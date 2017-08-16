#ifndef PERMISSIONS_H
#define PERMISSIONS_H

#include <QObject>
#ifdef Q_OS_ANDROID
    #include <QAndroidJniObject>
    #include <QAndroidJniEnvironment>
    #include <QtAndroid>
#endif

namespace QtAndroid
{
enum class PermissionResult {
    Granted,
    Denied
};
typedef QHash<QString, PermissionResult> PermissionResultMap;
typedef std::function<void(const PermissionResultMap &)> PermissionResultCallback;
void requestPermissions(const QStringList &permissions, const PermissionResultCallback &callbackFunc);
PermissionResultMap requestPermissionsSync(const QStringList &permissions, int timeoutMs = INT_MAX);
PermissionResult checkPermission(const QString &permission);
bool shouldShowRequestPermissionRationale(const QString &permission);
};

#endif // PERMISSIONS_H
