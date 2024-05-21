#ifndef SVGICON_H
#define SVGICON_H
#include "qbootstrap_global.h"
#include "qbootstrap.h"
#include "basewidget.h"
class QSvgRenderer;
class QBOOTSTRAP_EXPORT SvgIcon : public BaseWidget
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged FINAL)
public:
    explicit SvgIcon(QWidget *parent = nullptr);
    ~SvgIcon();

    QString name() const;
    void setName(const QString &newName);

signals:
    void nameChanged();

protected:
    void paintEvent(QPaintEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;
private:
    QString m_name = "";
    QImage m_svgImage;
    bool m_needUpdateSvgImage = false;
    QSvgRenderer *m_renderer = nullptr;
};

#endif // SVGICON_H
