#include "tooltipmanager.h"
#include <QEvent>
#include <QDebug>
ToolTipManager::ToolTipManager(QObject *parent)
    : QObject{parent}
{}

void ToolTipManager::installToolTip(QWidget *source, QWidget *toolTip)
{
    if (source) {
        source->installEventFilter(this);
        m_widgetToolTips[source] = toolTip;
    }
}

void ToolTipManager::uninstallToolTip(QWidget *source)
{
    if (source) {
        removeEventFilter(source);
        m_widgetToolTips.remove(source);
    }
}

bool ToolTipManager::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::ToolTip) {
        auto widget = qobject_cast<QWidget *>(watched);
        if (m_widgetToolTips.contains(widget)) {
            qDebug() << widget->toolTip() << m_widgetToolTips[widget];
            return true;
        }
    } else if (event->type() == QEvent::Destroy) {
        auto widget = qobject_cast<QWidget *>(watched);
        m_widgetToolTips.remove(widget);
    }
    return ToolTipManager::eventFilter(watched, event);
}
