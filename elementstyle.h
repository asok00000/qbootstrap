#ifndef ELEMENTSTYLE_H
#define ELEMENTSTYLE_H

#include <QProxyStyle>
#include <QMap>
#include "qbootstrap_global.h"
class QStyleOptionButton;
class QSvgRenderer;
class QBOOTSTRAP_EXPORT ElementStyle : public QProxyStyle
{
    Q_OBJECT
public:
    ElementStyle(const QString &key);
    ElementStyle(QStyle *style = nullptr);
    enum ButtonType {
        BT_Default,
        BT_Text,
        BT_Icon
    };
    enum StyleMode {
        ST_Default,
        ST_Primary,
        ST_Success,
        ST_Warning,
        ST_Danger,
        ST_Info
    };
    enum IconPosition {
        IP_LEFT,
        IP_RIGHT
    };

    struct StyleColors {
        QColor dark;
        QColor color;
        QColor color70;
        QColor color50;
        QColor color30;
        QColor color20;
        QColor color10;
    };
    struct TextColors
    {
        QColor primary;
        QColor regular;
        QColor secondary;
        QColor placeholder;
        QColor disable;
    };
    struct BorderColors
    {
        QColor darker;
        QColor dark;
        QColor base;
        QColor light;
        QColor lighter;
        QColor extraLight;
    };
    struct FillColors
    {
        QColor darker;
        QColor dark;
        QColor base;
        QColor light;
        QColor lighter;
        QColor extraLight;
        QColor blank;
    };
    struct BackgroundColors
    {
        QColor page;
        QColor base;
        QColor overlay;
    };
    struct Basic {
        QColor black;
        QColor white;
        QColor transparent;
    };

    struct ThemeColors {
        StyleColors primary;
        StyleColors success;
        StyleColors warning;
        StyleColors danger;
        StyleColors info;
        TextColors text;
        BorderColors border;
        FillColors fill;
        BackgroundColors background;
        Basic basic;
    };

    // QStyle interface
public:
    void drawPrimitive(PrimitiveElement pe, const QStyleOption *opt, QPainter *p, const QWidget *w) const;
    void drawControl(ControlElement element, const QStyleOption *opt, QPainter *p, const QWidget *w) const;
    void drawComplexControl(ComplexControl cc, const QStyleOptionComplex *opt, QPainter *p, const QWidget *widget) const;
    void polish(QWidget *widget);
    QRect subElementRect(SubElement subElement, const QStyleOption *option, const QWidget *widget) const;
    bool eventFilter(QObject *watched, QEvent *event);
private:
    void drawPushButton(ControlElement element, const QStyleOptionButton &opt, QPainter *p, const QWidget *w) const;
    void drawRadioButton(ControlElement element, const QStyleOptionButton &opt, QPainter *p, const QWidget *w) const;
    QPalette buttonPalette(const QWidget *w, const State &state) const;
    QPalette radioPalette(const QWidget *w, const State &state) const;
private:
    QMap<QString, QSvgRenderer *> m_icons;
};

#endif // ELEMENTSTYLE_H
