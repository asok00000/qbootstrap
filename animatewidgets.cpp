#include "animatewidgets.h"
#include <QPropertyAnimation>
#include <QVariant>
#include <QWidget>
#include <QEvent>
#include <QRegularExpression>
#include "qss/qcssparser_p.h"
#include "qss/stylehelper.h"

#define PSEUDO_NORMAL "normal"
#define PSEUDO_HOVER "hover"
#define PSEUDO_PRESSED "pressed"
#define PSEUDO_SHOW "show"
#define PSEUDO_HIDE "hide"
#define STYLE_SHEET "styleSheet"
#define QSS_NORMAL_TEMPLATE QString("%1 {%2}")
#define QSS_HOVER_TEMPLATE QString(":hover {%1}")
#define QSS_PRESSED_TEMPLATE QString(":pressed {%1}")
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
    //handle start
    auto qsses = start.split(";");
    QCss::Parser parser;
    QMap<QString, QString> startFixedQsses, endFixedQsses;
    QMap<QString, std::pair<QString, QCss::Declaration>> startProperties, endProperties;
    for (const auto &q : qsses) {
        parser.init(q);
        do {
            QCss::Declaration dec;
            parser.skipSpace();
            if (parser.parseNextDeclaration(&dec) && !dec.isEmpty()) {
                QString property = dec.d->property;
                if (dec.d->propertyId == QCss::Color || dec.d->propertyId == QCss::BackgroundColor) {
                    startProperties[property] = std::pair<QString, QCss::Declaration>(q, dec);
                    continue;
                } else if (dec.d->propertyId == QCss::BorderRadius) {
                    if (auto values = dec.d->values; values.count() == 1) {
                        if (auto v =values.first(); v.type == QCss::Value::Length) {
                            startProperties[property] = std::pair<QString, QCss::Declaration>(q, dec);
                            continue;
                        }
                    }
                }else if (dec.d->propertyId == QCss::UnknownProperty) {
                    if (auto values = dec.d->values; values.count() == 1) {
                        if (auto v =values.first(); v.type == QCss::Value::Number) {
                            startProperties[property] = std::pair<QString, QCss::Declaration>(q, dec);
                            continue;
                        }
                    }
                } else {
                    startFixedQsses[property] = q.simplified();
                }
            }

        } while (parser.test(QCss::SEMICOLON));
    }
    qsses = end.split(";");
    for (const auto &q : qsses) {
        parser.init(q);
        do {
            QCss::Declaration dec;
            parser.skipSpace();
            if (parser.parseNextDeclaration(&dec) && !dec.isEmpty()) {
                QString property = dec.d->property;
                if (dec.d->propertyId == QCss::Color || dec.d->propertyId == QCss::BackgroundColor) {
                    endProperties[property] = std::pair<QString, QCss::Declaration>(q, dec);
                    continue;
                } else if (dec.d->propertyId == QCss::BorderRadius) {
                    if (auto values = dec.d->values; values.count() == 1) {
                        if (auto v =values.first(); v.type == QCss::Value::Length) {
                            endProperties[property] = std::pair<QString, QCss::Declaration>(q, dec);
                            continue;
                        }
                    }
                }else if (dec.d->propertyId == QCss::UnknownProperty) {
                    if (auto values = dec.d->values; values.count() == 1) {
                        if (auto v =values.first(); v.type == QCss::Value::Number) {
                            endProperties[property] = std::pair<QString, QCss::Declaration>(q, dec);
                            continue;
                        }
                    }
                } else {
                    startFixedQsses.remove(property);
                    endFixedQsses[property] = q.simplified();
                }
            }
        } while (parser.test(QCss::SEMICOLON));
    }
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
    return endFixedQsses.values().join(";");
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

void AnimateWidgets::enableQssAnimation(QWidget *widget)
{
    widget->installEventFilter(this);
    initWidgetAnimation(widget);
}

void AnimateWidgets::disableQssAnimation(QWidget *widget)
{
    widget->removeEventFilter(this);
    m_animatedWidgetQss.remove(widget);
}

AnimateWidgets::AnimateWidgets(QObject *parent) :QObject(parent)
{
    m_handledPseudoes << PSEUDO_NORMAL << PSEUDO_HOVER << PSEUDO_PRESSED;
    qRegisterAnimationInterpolator<QString>(qssData);
}

void AnimateWidgets::initWidgetAnimation(QWidget *target)
{

    auto rules = StyleHelper::styleRules(target);
    qDebug() << rules.count() << target->objectName();
    for (auto &rule : rules) {
        for (auto &selector : rule.selectors) {
            qDebug() << selector.pseudoElement();
        }
    }

    auto qss = target->styleSheet();
    auto pseudoes = qss.split("}", Qt::SkipEmptyParts);
    QMap<QString, QString> declarations;
    for (auto pseudo : pseudoes) {
        pseudo += "}";
        QCss::Parser parser(pseudo);
        if (QCss::StyleSheet sheet; parser.parse(&sheet, Qt::CaseInsensitive)) {
            for (const auto &sr : sheet.styleRules) {
                QStringList pseudoSelectors;
                for (const auto &selector : sr.selectors) {
                    for (const auto &basicSelector : selector.basicSelectors) {
                        for (const auto &pseudoSelector : basicSelector.pseudos) {
                            pseudoSelectors << pseudoSelector.name;
                        }
                    }
                }
                for (auto &selector : pseudoSelectors) {
                    static QRegularExpression pseudoReg(R"(.*{|}.*|\n|\t)");
                    declarations[selector] = pseudo.remove(pseudoReg);
                }
            }
        } else {
            qWarning() << "Invalid QSS.";
        }
    }

    if (declarations.contains(PSEUDO_NORMAL)) {
        auto originQss = declarations[PSEUDO_NORMAL];
        originQss.replace(QString(":") + PSEUDO_NORMAL, target->metaObject()->className());
        target->setStyleSheet(originQss);
    }
    m_animatedWidgetQss[target] = declarations;

}

QString AnimateWidgets::currentAnimationQss(QWidget* target, QString pseudo)
{
    QString qssTemplate = "";
    auto animatedData = m_animatedWidgetQss[target];
    if (target->styleSheet().indexOf("{") >= 0) {
        auto declaration = animatedData.find(PSEUDO_NORMAL);
        if (declaration != animatedData.end()) {
            return declaration.value();
        }
    }
    if (pseudo == PSEUDO_NORMAL) {
        qssTemplate = QSS_NORMAL_TEMPLATE.arg(target->metaObject()->className());
    } else if (pseudo == PSEUDO_HOVER) {
        qssTemplate = QSS_HOVER_TEMPLATE;
    } else if (pseudo == PSEUDO_PRESSED) {
        qssTemplate = QSS_PRESSED_TEMPLATE;
    } else {
        return "";
    }
    auto rules = StyleHelper::styleRules(target);
    qDebug() << rules.count() << target->objectName();
    if (auto declaration = animatedData.find(pseudo); declaration != animatedData.end())  {
        auto qss = declaration.value();
        QString currentQss = target->styleSheet();
        QStringList subQsses;
        auto qsses = currentQss.split(";");
        QCss::Parser parser;
        for (const auto &q : qsses) {
            parser.init(q);
            do {
                QCss::Declaration dec;
                parser.skipSpace();
                if (parser.parseNextDeclaration(&dec) && !dec.isEmpty()) {
                    QString property = dec.d->property;
                    if (dec.d->propertyId == QCss::Color || dec.d->propertyId == QCss::BackgroundColor) {
                        subQsses << QSS_PROPERTY_TEMPLATE.arg(property).arg(dec.d->values.first().variant.toString());
                        continue;
                    } else if (dec.d->propertyId == QCss::BorderRadius) {
                        if (auto values = dec.d->values; values.count() == 1) {
                            if (auto v =values.first(); v.type == QCss::Value::Length) {
                                subQsses << QSS_PROPERTY_TEMPLATE.arg(property).arg(v.variant.toString());
                                continue;
                            }
                        }
                    }else if (dec.d->propertyId == QCss::UnknownProperty) {
                        if (auto values = dec.d->values; values.count() == 1) {
                            if (auto v =values.first(); v.type == QCss::Value::Number) {
                                subQsses << QSS_PROPERTY_TEMPLATE.arg(property).arg(v.variant.toFloat());
                                continue;
                            }
                        }
                    }
                }
                subQsses << q;
            } while (parser.test(QCss::SEMICOLON));
        }
        currentQss = subQsses.join(";");
        return currentQss;
    }
    return "";
}

bool AnimateWidgets::eventFilter(QObject *watched, QEvent *event)
{
    if (m_animatedWidgetQss.contains(qobject_cast<QWidget*>(watched))) {
        auto animatedData = m_animatedWidgetQss[static_cast<QWidget*>(watched)];
        auto widget = static_cast<QWidget*>(watched);
        if (event->type() == QEvent::Polish) {
            // initWidgetAnimation(widget);
        } else if (event->type() == QEvent::Show) {
            return true;
        } else if (event->type() == QEvent::Hide) {
            return true;
        } else if (event->type() == QEvent::Enter) {
            //hover qss
            auto hoverData = animatedData.find(PSEUDO_HOVER);
            if (hoverData != animatedData.end()) {
                auto currentQss = currentAnimationQss(widget, PSEUDO_HOVER);
                auto qss = hoverData.value();
                if (!currentQss.isEmpty() && !qss.isEmpty()) {
                    addQssAnimation(watched, STYLE_SHEET, currentQss, qss);
                }
            }
        } else if (event->type() == QEvent::Leave || event->type() == QEvent::MouseButtonRelease) {
            //normal qss
            auto nomalData = animatedData.find(PSEUDO_NORMAL);
            if (nomalData != animatedData.end()) {
                auto currentQss = currentAnimationQss(widget, PSEUDO_NORMAL);
                auto qss = nomalData.value();
                if (!currentQss.isEmpty() && !qss.isEmpty()) {
                    addQssAnimation(watched, STYLE_SHEET, currentQss, qss);
                }
            }
        } else if (event->type() == QEvent::MouseButtonPress) {
            //pressed qss
            auto pressedData = animatedData.find(PSEUDO_PRESSED);
            if (pressedData == animatedData.end()) {
                pressedData = animatedData.find(PSEUDO_HOVER);
            }
            if (pressedData != animatedData.end()) {
                auto currentQss = currentAnimationQss(widget, PSEUDO_PRESSED);
                auto qss = pressedData.value();
                if (!currentQss.isEmpty() && !qss.isEmpty()) {
                    addQssAnimation(watched, STYLE_SHEET, currentQss, qss);
                }
            }
        } else if (event->type() == QEvent::Destroy) {
            m_animatedWidgetQss.remove(widget);
        }
    }
    return QObject::eventFilter(watched, event);
}
