#ifndef TOOLTIP_H
#define TOOLTIP_H

#include <QDialog>
#include "qbootstrap_global.h"

class QBOOTSTRAP_EXPORT ToolTip : public QDialog
{
    Q_OBJECT
public:
    ToolTip(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags(Qt::FramelessWindowHint | Qt::ToolTip));

    // QWidget interface
protected:
    void paintEvent(QPaintEvent *event);
};

#endif // TOOLTIP_H
