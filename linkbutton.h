#ifndef LINKBUTTON_H
#define LINKBUTTON_H

#include <QPushButton>
#include "qbootstrap_global.h"
class QBOOTSTRAP_EXPORT LinkButton : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(QString link READ link WRITE setLink NOTIFY linkChanged FINAL)
public:
    explicit LinkButton(QWidget *parent = nullptr);
    explicit LinkButton(const QString &text, QWidget *parent = nullptr);
    LinkButton(const QString& link, const QString &text, QWidget *parent = nullptr);
    QString link() const;
    void setLink(const QString &newLink);
signals:
    void linkChanged();
    void clicked(const QString link);
private:
    QString m_link;
};

#endif // LINKBUTTON_H
