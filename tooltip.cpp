#include "tooltip.h"
#include <QPainter>
#include <QPainterPath>
ToolTip::ToolTip(QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
{

}

void ToolTip::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    // QPainterPath path;
    // path.addRoundedRect(rect(), 10, 10);
    // p.setClipPath(path);
    p.drawRoundedRect(rect(), 10, 10);
}
