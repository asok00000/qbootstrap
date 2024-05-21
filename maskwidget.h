#ifndef MASKWIDGET_H
#define MASKWIDGET_H

#include "basewidget.h"
#include "qbootstrap_global.h"

#include <QImage>
class QBOOTSTRAP_EXPORT MaskWidget : public BaseWidget
{
    Q_OBJECT
    Q_PROPERTY(QWidget* sourceWidget READ sourceWidget WRITE setSourceWidget NOTIFY sourceWidgetChanged FINAL)
public:
    MaskWidget(QWidget *parent = nullptr);
    void setSource(QImage source);
protected:
    void paintEvent(QPaintEvent *event);
    void updateMaskImage();
private:
    QImage m_source;
    QBitmap m_mask;
    QWidget *m_sourceWidget = nullptr;

public:
    bool event(QEvent *event);
    QWidget *sourceWidget() const;
    void setSourceWidget(QWidget *newSourceWidget);
signals:
    void sourceWidgetChanged();
};

#endif // MASKWIDGET_H
