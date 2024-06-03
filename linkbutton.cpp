#include "linkbutton.h"

LinkButton::LinkButton(QWidget *parent)
    : QPushButton(parent)
{
    connect(this, &QPushButton::clicked, this, [this]() {
        emit clicked(m_link);
    });
}

LinkButton::LinkButton(const QString &text, QWidget *parent)
    : QPushButton(text, parent)
{
    connect(this, &QPushButton::clicked, this, [this]() {
        emit clicked(m_link);
    });
}

LinkButton::LinkButton(const QString &link, const QString &text, QWidget *parent)
    : QPushButton(text, parent)
{
    m_link = link;
}


QString LinkButton::link() const
{
    return m_link;
}

void LinkButton::setLink(const QString &newLink)
{
    if (m_link == newLink)
        return;
    m_link = newLink;
    emit linkChanged();
}
