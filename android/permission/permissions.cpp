#include "permissions.h"

#include <QtCore/private/qjni_p.h>
#include <QtCore/private/qjnihelpers_p.h>

static QtAndroid::PermissionResultMap privateToPublicPermissionsHash(const QtAndroidPrivate::PermissionsHash &privateHash)
{
    QtAndroid::PermissionResultMap hash;
    for (auto it = privateHash.constBegin(); it != privateHash.constEnd(); ++it)
        hash[it.key()] = QtAndroid::PermissionResult(it.value());
    return hash;
}

/*!
  \since 5.10
  \enum QtAndroid::PermissionResult

  This enum is used to describe the permission status.

  \value Granted    The permission was granted.
  \value Denied     The permission was denied.
*/

/*!
    \typedef QtAndroid::PermissionResultMap

    Synonym for QHash<QString, PermissionResult>.
*/

/*!
    \typedef QtAndroid::PermissionResultCallback

    Synonym for std::function<void(const PermissionResultMap &)>.
*/

/*!
  \since 5.10
  \fn void QtAndroid::requestPermissions(const QStringList &permissions, const PermissionResultCallback &callbackFunc)

  Asynchronously requests \a permissions to be granted to this application, \a callbackFunc will be called with the results.
*/
void QtAndroid::requestPermissions(const QStringList &permissions, const QtAndroid::PermissionResultCallback &callbackFunc)
{
    QtAndroidPrivate::requestPermissions(QJNIEnvironmentPrivate(), permissions,
                                         [callbackFunc](const QtAndroidPrivate::PermissionsHash &privatePerms){
                                            callbackFunc(privateToPublicPermissionsHash(privatePerms));
                                         }, false);
}

/*!
  \since 5.10
  \fn QtAndroid::PermissionResultMap QtAndroid::requestPermissionsSync(const QStringList &permissions, int timeoutMs)

  Synchronously requests \a permissions to be granted to this application, waits \a timeoutMs to complete.
 */
QtAndroid::PermissionResultMap QtAndroid::requestPermissionsSync(const QStringList &permissions, int timeoutMs)
{
    return privateToPublicPermissionsHash(QtAndroidPrivate::requestPermissionsSync(QJNIEnvironmentPrivate(), permissions, timeoutMs));
}

/*!
  \since 5.10
  \fn QtAndroid::PermissionResult QtAndroid::checkPermission(const QString &permission)

  Checks if the \a permission was granted or not. This function should be called every time when
  the application starts for needed permissions, as the users might disable them from Android Settings.
 */
QtAndroid::PermissionResult QtAndroid::checkPermission(const QString &permission)
{
    return QtAndroid::PermissionResult(QtAndroidPrivate::checkPermission(permission));
}

/*!
  \since 5.10
  \fn bool QtAndroid::shouldShowRequestPermissionRationale(const QString &permission)

  Returns \c true if you should show UI with a rationale for requesting a \a permission.
*/
bool QtAndroid::shouldShowRequestPermissionRationale(const QString &permission)
{
    return QtAndroidPrivate::shouldShowRequestPermissionRationale(permission);
}

