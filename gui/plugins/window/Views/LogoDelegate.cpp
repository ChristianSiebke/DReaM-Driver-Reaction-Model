#include "Views/LogoDelegate.h"

#include <QPixmap>

QPixmap LogoDelegate::logoRequest()
{
    return QPixmap(":/Views/logo_openpass.png");
}
