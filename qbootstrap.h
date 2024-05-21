#ifndef QBOOTSTRAP_H
#define QBOOTSTRAP_H

#include "QtCore/qstring.h"
#include "qbootstrap_global.h"

class QBOOTSTRAP_EXPORT QBootstrap
{
public:
    QBootstrap();
    inline static const QString iconPrefix = ":/icons/%1.svg";
};

#endif // QBOOTSTRAP_H
