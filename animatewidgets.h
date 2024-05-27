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
    void startQssAnimation(QObject *obj, QString start, QString end, int duration = 1000, bool loop = true, const QEasingCurve &easing = QEasingCurve::OutQuad);
    void stopQssAnimation(QObject *obj);
private:
    AnimateWidgets(QObject *parent = nullptr);
    void initWidgetAnimation(QWidget *target);
    QString currentAnimationQss(QWidget* target);
    void addQssAnimation(QObject* obj, QString property, QVariant from, QVariant to, int duration = 400, QAbstractAnimation::Direction direction = QAbstractAnimation::Forward, const QEasingCurve &easing = QEasingCurve::OutQuad);
    QStringList qssByPseudo(QWidget* item, qint64 type);
private:
    QSet<QObject*> m_animatedWidgets;
    QHash<QObject*, QAbstractAnimation*> m_animations;
    QHash<QObject*, QAbstractAnimation*> m_pseudoAnimations;
};

#endif // ANIMATEWIDGETS_H
