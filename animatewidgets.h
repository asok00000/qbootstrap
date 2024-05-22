#ifndef ANIMATEWIDGETS_H
#define ANIMATEWIDGETS_H

#include <QObject>
#include <QAbstractAnimation>
#include <QEasingCurve>
#include <QSet>

#include "qbootstrap_global.h"

class QWidget;
class QBOOTSTRAP_EXPORT AnimateWidgets: public QObject
{
    Q_OBJECT
public:
    ~AnimateWidgets();
    static AnimateWidgets *instance();
    void enableQssAnimation(QWidget* widget);
    void disableQssAnimation(QWidget* widget);
    bool eventFilter(QObject *watched, QEvent *event);
private:
    AnimateWidgets(QObject *parent = nullptr);
    void initWidgetAnimation(QWidget *target);
    QString currentAnimationQss(QWidget* target);
    void addQssAnimation(QObject* obj, QString property, QVariant from, QVariant to, int duration = 400, QAbstractAnimation::Direction direction = QAbstractAnimation::Forward, const QEasingCurve &easing = QEasingCurve::OutQuad);
    QStringList qssByPseudo(QWidget* item, qint64 type);
private:
    QSet<QObject*> m_animatedWidgets;
};

#endif // ANIMATEWIDGETS_H
