#include "animatewidgets.h"
#include <QPropertyAnimation>
#include <QVariant>
#include <QWidget>
#include <QEvent>
#include <QRegularExpression>
#include <QElapsedTimer>
#include "qss/qcssparser_p.h"
#include "qss/stylehelper.h"

#define STYLE_SHEET "styleSheet"
#define QSS_PROPERTY_TEMPLATE QString("%1:%2")
QVariant qssData(const QString &start, const QString &end, qreal progress)
{
    auto currentColor = [=] (QColor sc, QColor ec) {
        return QColor::fromRgbF(sc.redF() + (ec.redF()-sc.redF()) * progress,
                                sc.greenF() + (ec.greenF()-sc.greenF()) * progress,
                                sc.blueF() + (ec.blueF()-sc.blueF()) * progress,
                                sc.alphaF() + (ec.alphaF()-sc.alphaF()) * progress);
    };
    auto currentNumber = [=] (auto si, auto ei) {
        return si + (ei - si) * progress;
    };
    QCss::Parser parser;
    QMap<QString, QString> startFixedQsses, endFixedQsses;
    QMap<QString, std::pair<QString, QCss::Declaration>> startProperties, endProperties;
    //handle start
    parser.init(start);
    do {
        QCss::Declaration dec;
        parser.skipSpace();
        const int rwind = parser.index;
        if (parser.parseNextDeclaration(&dec) && !dec.isEmpty()) {
            if (dec.d->text.isEmpty()) {
                auto os = parser.symbols[rwind];
                auto cs = parser.symbols[std::min(parser.index, (int)parser.symbols.length() - 1)];
                dec.d->text = parser.symbol().text.mid(os.start, cs.start + cs.len - os.start);
            }
            QString property = dec.d->property;
            if (dec.d->propertyId == QCss::Color || dec.d->propertyId == QCss::BackgroundColor) {
                startProperties[property] = std::pair<QString, QCss::Declaration>(dec.d->text, dec);
                continue;
            } else if (dec.d->propertyId == QCss::BorderRadius) {
                if (auto values = dec.d->values; values.count() == 1) {
                    if (auto v =values.first(); v.type == QCss::Value::Length) {
                        startProperties[property] = std::pair<QString, QCss::Declaration>(dec.d->text, dec);
                        continue;
                    }
                }
            } else if (dec.d->propertyId == QCss::UnknownProperty) {
                if (auto values = dec.d->values; values.count() == 1) {
                    if (auto v =values.first(); v.type == QCss::Value::Number) {
                        startProperties[property] = std::pair<QString, QCss::Declaration>(dec.d->text, dec);
                        continue;
                    }
                }
            } else {
                startFixedQsses[property] = dec.d->text.simplified();
            }
        }

    } while (parser.test(QCss::SEMICOLON));
    //handle end
    parser.init(end);
    do {
        QCss::Declaration dec;
        parser.skipSpace();
        const int rwind = parser.index;
        if (parser.parseNextDeclaration(&dec) && !dec.isEmpty()) {
            if (dec.d->text.isEmpty()) {
                auto os = parser.symbols[rwind];
                auto cs = parser.symbols[std::min(parser.index, (int)parser.symbols.length() - 1)];
                dec.d->text = parser.symbol().text.mid(os.start, cs.start + cs.len - os.start);
            }
            QString property = dec.d->property;
            if (dec.d->propertyId == QCss::Color || dec.d->propertyId == QCss::BackgroundColor) {
                endProperties[property] = std::pair<QString, QCss::Declaration>(dec.d->text, dec);
                continue;
            } else if (dec.d->propertyId == QCss::BorderRadius) {
                if (auto values = dec.d->values; values.count() == 1) {
                    if (auto v =values.first(); v.type == QCss::Value::Length) {
                        endProperties[property] = std::pair<QString, QCss::Declaration>(dec.d->text, dec);
                        continue;
                    }
                }
            }else if (dec.d->propertyId == QCss::UnknownProperty) {
                if (auto values = dec.d->values; values.count() == 1) {
                    if (auto v =values.first(); v.type == QCss::Value::Number) {
                        endProperties[property] = std::pair<QString, QCss::Declaration>(dec.d->text, dec);
                        continue;
                    }
                }
            } else {
                startFixedQsses.remove(property);
                endFixedQsses[property] = dec.d->text.simplified();
            }
        }
    } while (parser.test(QCss::SEMICOLON));

    endFixedQsses.insert(startFixedQsses);
    for (auto it = startProperties.begin(); it != startProperties.end(); it++) {
        auto [sqss, sdata] = startProperties[it.key()];
        if (endProperties.contains(it.key())) {
            auto [eqss, edata] = endProperties[it.key()];
            switch (sdata.d->propertyId) {
            case QCss::Property::NumProperties: {
                qDebug() << "num property:"  << it.key() << sqss;
            }
            break;
            case QCss::Property::BackgroundColor:
            case QCss::Property::Color: {
                auto sColor = sdata.colorValue();
                auto eColor = edata.colorValue();
                endFixedQsses[it.key()] = QSS_PROPERTY_TEMPLATE.arg(it.key(), currentColor(sColor, eColor).name(QColor::HexArgb));
            }
            break;
            case QCss::Property::BorderRadius: {
                auto sValue = sdata.d->values.first();
                auto eValue = edata.d->values.first();
                auto sValueString = sValue.toString();
                auto eValueString = eValue.toString();
                int sRadius, eRadius;
                if (sValueString.endsWith("pt")) {
                    sdata.intValue(&sRadius, "pt");
                    sRadius = (sRadius * 72) / 96;
                } else {
                    sdata.intValue(&sRadius, "px");
                }
                if (eValueString.endsWith("pt")) {
                    edata.intValue(&eRadius, "pt");
                    eRadius = (eRadius * 72) / 96;
                } else {
                    edata.intValue(&eRadius, "px");
                }
                endFixedQsses[it.key()] = QSS_PROPERTY_TEMPLATE.arg(it.key()).arg(currentNumber(sRadius, eRadius)) + "px";
            }
            break;
            case QCss::Property::Width:
                qDebug() << "width:"  << it.key() << sqss;
                break;
            case QCss::Property::Height:
                qDebug() << "height:"  << it.key() << sqss;
                break;
            case QCss::Property::UnknownProperty: {
                auto sValue = sdata.d->values.first().variant;
                auto eValue = edata.d->values.first().variant;
                if (sValue.typeId() == QMetaType::Double) {
                    endFixedQsses[it.key()] = (QSS_PROPERTY_TEMPLATE).arg(it.key()).arg(currentNumber(sValue.toDouble(), eValue.toDouble()));
                } else {
                    qWarning() << "unknown property:"  << it.key() << sValue << eValue;
                }
            }
            break;
            default:
                break;
            }
            endProperties.remove(it.key());
        } else {
            endFixedQsses[it.key()] = sqss;
        }
    }

    for (auto it = endProperties.begin(); it != endProperties.end(); it++) {
        auto [eqss, edata] = endProperties[it.key()];
        endFixedQsses[it.key()] = eqss;
    }
    auto values = endFixedQsses.values();
    return values.join(";");
}
AnimateWidgets::~AnimateWidgets()
{
}

AnimateWidgets *AnimateWidgets::instance()
{
    static AnimateWidgets animateWidgets;
    return &animateWidgets;
}

void AnimateWidgets::addQssAnimation(QObject *obj, QString property, QVariant from, QVariant to, int duration, QAbstractAnimation::Direction direction, const QEasingCurve &easing)
{
    auto animation = new QPropertyAnimation(obj, property.toUtf8(), this);
    animation->setDirection(direction);
    animation->setDuration(duration);
    animation->setEasingCurve(easing);
    animation->setStartValue(from);
    animation->setEndValue(to);
    animation->start(QAbstractAnimation::DeleteWhenStopped);

}

QStringList AnimateWidgets::qssByPseudo(QWidget *item, qint64 type)
{
    auto rules = StyleHelper::styleRules(item);
    for (auto &rule : rules) {
        if (!rule.selectors.isEmpty() &&
            !rule.selectors.first().basicSelectors.isEmpty() &&
            !rule.selectors.first().basicSelectors.first().pseudos.isEmpty()) {
            if (rule.selectors.first().basicSelectors.first().pseudos.first().type == type) {
                QStringList ssLines;
                for (auto &decl : rule.declarations) {
                    ssLines << decl.d->text;
                }
                return ssLines;
            }
        }
    }
    return QStringList();
}

void AnimateWidgets::enableQssAnimation(QWidget *widget)
{
    widget->installEventFilter(this);
    m_animatedWidgets << widget;
    initWidgetAnimation(widget);
}

void AnimateWidgets::disableQssAnimation(QWidget *widget)
{
    widget->removeEventFilter(this);
    m_animatedWidgets.remove(widget);
}

AnimateWidgets::AnimateWidgets(QObject *parent) :QObject(parent)
{
    qRegisterAnimationInterpolator<QString>(qssData);
}

void AnimateWidgets::initWidgetAnimation(QWidget *target)
{
    auto ssLines = qssByPseudo(target, QCss::PseudoClass_Normal);
    if (!ssLines.isEmpty()) {
        target->setStyleSheet(ssLines.join(""));
    }
}

QString AnimateWidgets::currentAnimationQss(QWidget *target)
{
    QString qss = "";
    auto rules = StyleHelper::styleRules(target, false);
    if (!rules.isEmpty()) {
        for (auto &decl : rules.last().declarations) {
            qss += decl.d->text;
        }
    }
    return qss;
}

bool AnimateWidgets::eventFilter(QObject *watched, QEvent *event)
{
    if (m_animatedWidgets.contains(qobject_cast<QWidget*>(watched))) {
        auto widget = static_cast<QWidget*>(watched);
        if (event->type() == QEvent::PolishRequest) {
            // qDebug() << "polish" << widget->objectName();
            // StyleHelper::styleSheetCaches->objectDestroyed(watched);
            // initWidgetAnimation(widget);

        } else if (event->type() == QEvent::Enter) {
            //hover qss
            auto ssLines = qssByPseudo(widget, QCss::PseudoClass_Hover);
            if (!ssLines.isEmpty()) {
                QString hoverQss = ssLines.join("");
                auto currentQss = currentAnimationQss(widget);
                if (!currentQss.isEmpty() && !hoverQss.isEmpty()) {
                    addQssAnimation(watched, STYLE_SHEET, currentQss, hoverQss);
                }
            }
        } else if (event->type() == QEvent::Leave || event->type() == QEvent::MouseButtonRelease) {
            //normal qss
            auto ssLines = qssByPseudo(widget, QCss::PseudoClass_Normal);
            if (!ssLines.isEmpty()) {
                QString normalQss = ssLines.join("");
                auto currentQss = currentAnimationQss(widget);
                if (!currentQss.isEmpty() && !normalQss.isEmpty()) {
                    addQssAnimation(watched, STYLE_SHEET, currentQss, normalQss);
                }
            }
        } else if (event->type() == QEvent::MouseButtonPress) {
            //pressed qss
            auto ssLines = qssByPseudo(widget, QCss::PseudoClass_Pressed);
            if (!ssLines.isEmpty()) {
                QString pressedQss = ssLines.join("");
                auto currentQss = currentAnimationQss(widget);
                if (!currentQss.isEmpty() && !pressedQss.isEmpty()) {
                    addQssAnimation(watched, STYLE_SHEET, currentQss, pressedQss);
                }
            }
        } else if (event->type() == QEvent::Destroy) {
            m_animatedWidgets.remove(widget);
        }
    }
    return QObject::eventFilter(watched, event);
}

void AnimateWidgets::startQssAnimation(QObject *obj, QString start, QString end, int duration, bool loop, const QEasingCurve &easing)
{
    auto animation = new QPropertyAnimation(obj, STYLE_SHEET, this);
    animation->setDirection(QAbstractAnimation::Forward);
    animation->setDuration(duration);
    animation->setEasingCurve(easing);
    animation->setStartValue(start);
    animation->setEndValue(end);
    animation->setLoopCount(loop ? -1 : 1);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
    connect(animation, &QAbstractAnimation::stateChanged, this, [&](QAbstractAnimation::State newState, QAbstractAnimation::State oldState){
        if (newState == QAbstractAnimation::Stopped) {
            auto a = static_cast<QPropertyAnimation*>(QObject::sender());
            if (a) {
                auto t = a->targetObject();
                m_animations.take(t);
            }
        }
    });
    m_animations[obj] = animation;
}

void AnimateWidgets::stopQssAnimation(QObject *obj)
{
    if (m_animations.contains(obj)) {
        auto animation = m_animations.take(obj);
        animation->stop();
    }
}
