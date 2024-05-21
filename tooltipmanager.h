#ifndef TOOLTIPMANAGER_H
#define TOOLTIPMANAGER_H

#include <QObject>
#include <QWidget>
#include <QHash>
#include "qbootstrap_global.h"
class QBOOTSTRAP_EXPORT ToolTipManager : public QObject
{
    Q_OBJECT
public:
    explicit ToolTipManager(QObject *parent = nullptr);
    void installToolTip(QWidget* source, QWidget* toolTip = nullptr);
    void uninstallToolTip(QWidget* source);
signals:

    // QObject interface
public:
    bool eventFilter(QObject *watched, QEvent *event);
private:
    QHash<QWidget*, QWidget*> m_widgetToolTips;
};

#endif // TOOLTIPMANAGER_H
