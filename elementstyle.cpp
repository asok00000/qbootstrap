#include "elementstyle.h"
#include <QDebug>
#include <QWidget>
#include <QStyleOption>
#include <QPainter>
#include <QEvent>
#include <QSvgRenderer>
#include <QPushButton>
#include "linkbutton.h"
#include "basewidget.h"
#include "qbootstrap.h"
#define PROPERTY_DARK_MODE "darkMode"
#define PROPERTY_ROUND_MODE "roundMode"
#define PROPERTY_PLAIN_MODE "plainMode"
#define PROPERTY_STYLE_MODE "styleMode"
#define PROPERTY_BUTTON_TYPE "buttonType"
#define PROPERTY_ICON_NAME "iconName"
#define PROPERTY_SPACING "spacing"
#define PROPERTY_ICON_POSITION "iconPosition"
static const ElementStyle::ThemeColors s_light = {
    {/*primary*/QColor("#337ECC"), QColor("#409EFF"), QColor("#79BBFF"), QColor("#9FCEFF"), QColor("#C6E2FF"), QColor("#D9ECFF"), QColor("#ECF6FF")},
    {/*success*/QColor("#529B2E"), QColor("#67C23A"), QColor("#95D576"), QColor("#B2E09C"), QColor("#D2EDC4"), QColor("#E1F3D8"), QColor("#F0F9EC")},
    {/*warning*/QColor("#B88230"), QColor("#E6A23C"), QColor("#F3D2A0"), QColor("#F2D09D"), QColor("#F8E3C5"), QColor("#FAECD8"), QColor("#FCF6EC")},
    {/*danger*/QColor("#C45656"), QColor("#F56C6C"), QColor("#FAB8B8"), QColor("#F9B5B5"), QColor("#FCD3D3"), QColor("#FDE2E2"), QColor("#FEF0F0")},
    {/*info*/QColor("#73767A"), QColor("#909399"), QColor("#C9CACD"), QColor("#C7C8CB"), QColor("#DEDFE1"), QColor("#E9E9EB"), QColor("#F4F5F5")},
    {/*text*/QColor("#303133"), QColor("#606266"), QColor("#909399"), QColor("#A8ABB2"), QColor("#C0C4CC")},
    {/*border*/QColor("#CDD0D6"), QColor("#D4D7DE"), QColor("#DCDFE6"), QColor("#E4E7ED"), QColor("#EBEEF5"), QColor("#F2F6FC")},
    {/*fill*/QColor("#E6E8EB"), QColor("#EBEDF0"), QColor("#F0F2F5"), QColor("#F5F7FA"), QColor("#FAFAFA"), QColor("#FAFCFF"), QColor("#FFFFFF")},
    {/*background*/QColor("#F2F3F5"), QColor("#FFFFFF"), QColor("#FFFFFF")},
    {/*basic*/QColor("#F2F3F5"), QColor("#FFFFFF"), Qt::transparent},
};
static const ElementStyle::ThemeColors s_dark = {
    {/*primary*/QColor("#66B1FF"), QColor("#409EFF"), QColor("#3375B9"), QColor("#2A598A"), QColor("#213D5B"), QColor("#1D3043"), QColor("#18222C")},
    {/*success*/QColor("#85CE61"), QColor("#67C23A"), QColor("#4E8E2F"), QColor("#3E6B27"), QColor("#2D481F"), QColor("#25371C"), QColor("#1C2518")},
    {/*warning*/QColor("#EBB563"), QColor("#E6A23C"), QColor("#A77730"), QColor("#7D5B28"), QColor("#533F20"), QColor("#3E301C"), QColor("#292218")},
    {/*danger*/QColor("#F78989"), QColor("#F56C6C"), QColor("#B25252"), QColor("#854040"), QColor("#582E2E"), QColor("#412626"), QColor("#2B1D1D")},
    {/*info*/QColor("#A6A9AD"), QColor("#909399"), QColor("#6B6D71"), QColor("#525457"), QColor("#393A3C"), QColor("#2D2D2F"), QColor("#202121")},
    {/*text*/QColor("#E5EAF3"), QColor("#CFD3DC"), QColor("#393A3C"), QColor("#8D9095"), QColor("#6C6E72")},
    {/*border*/QColor("#636466"), QColor("#58585B"), QColor("#4C4D4F"), QColor("#414243"), QColor("#363637"), QColor("#2B2B2C")},
    {/*fill*/QColor("#424243"), QColor("#39393A"), QColor("#303030"), QColor("#262727"), QColor("#1D1D1D"), QColor("#191919"), QColor("#191919")},
    {/*background*/QColor("#0A0A0A"), QColor("#141414"), Qt::transparent},
    {/*basic*/QColor("#0A0A0A"), QColor("#141414"), Qt::transparent},
    };
ElementStyle::ElementStyle(const QString &key)
    : QProxyStyle(key)
{

}

ElementStyle::ElementStyle(QStyle *style)
    : QProxyStyle(style)
{
}

void ElementStyle::drawPrimitive(PrimitiveElement pe, const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    // qDebug() << "drawPrimitive" << w->objectName() << pe;
    switch (pe) {
    default:
        QProxyStyle::drawPrimitive(pe, opt, p, w);
        break;
    }
}

void ElementStyle::drawControl(ControlElement element, const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    switch (element) {
    // case QStyle::CE_PushButtonLabel:
    // case QStyle::CE_PushButtonBevel:
    case QStyle::CE_PushButton:
    {
        auto buttonOpt = *(qstyleoption_cast<const QStyleOptionButton *>(opt));
        drawPushButton(element, buttonOpt, p, w);
    }
        break;
    default:
        QProxyStyle::drawControl(element, opt, p, w);
        break;
    }
}

void ElementStyle::drawComplexControl(ComplexControl cc, const QStyleOptionComplex *opt, QPainter *p, const QWidget *widget) const
{
    // qDebug() << "drawComplexControl" << widget->objectName() << opt->type;
    QProxyStyle::drawComplexControl(cc, opt, p, widget);
}

void ElementStyle::polish(QWidget *widget)
{
    widget->setAttribute(Qt::WA_Hover);
    widget->setAttribute(Qt::WA_MouseTracking);
    if (!widget->property(PROPERTY_DARK_MODE).isValid()) {
        widget->setProperty(PROPERTY_DARK_MODE, false);
    }
    if (!widget->property(PROPERTY_ROUND_MODE).isValid()) {
        widget->setProperty(PROPERTY_ROUND_MODE, false);
    }
    if (!widget->property(PROPERTY_PLAIN_MODE).isValid()) {
        widget->setProperty(PROPERTY_PLAIN_MODE, false);
    }
    if (!widget->property(PROPERTY_STYLE_MODE).isValid()) {
        widget->setProperty(PROPERTY_STYLE_MODE, ST_Default);
    }
    if (!widget->property(PROPERTY_ICON_POSITION).isValid()) {
        widget->setProperty(PROPERTY_ICON_POSITION, IP_LEFT);
    }
    if (widget->metaObject()->inherits(QPushButton().metaObject()) && !widget->property(PROPERTY_BUTTON_TYPE).isValid()) {
        widget->setProperty(PROPERTY_BUTTON_TYPE, BT_Default);
    }
    QString iconName = widget->property(PROPERTY_ICON_NAME).toString();
    if (!iconName.isEmpty() && !m_icons.contains(iconName)) {
        QSvgRenderer *renderer = new QSvgRenderer(this);
        renderer->load(QBootstrap::iconPrefix.arg(iconName));
        m_icons[iconName] = renderer;
    }
    QProxyStyle::polish(widget);
}

QRect ElementStyle::subElementRect(SubElement subElement, const QStyleOption *option, const QWidget *widget) const
{
    switch (subElement) {
    case QStyle::SE_PushButtonBevel:
        return widget->rect();
    default:
        break;
    }
    return QProxyStyle::subElementRect(subElement, option, widget);
}

void ElementStyle::drawPushButton(ControlElement element, const QStyleOptionButton &opt, QPainter *p, const QWidget *w) const
{
    auto rect = opt.rect;
    rect.adjust(1, 1, -1, -1);
    int radius = rect.height() / 2;
    if (!w->property(PROPERTY_ROUND_MODE).toBool()) {
        radius = rect.height() / 8;
    }
    QPalette &&palette = buttonPalette(w, opt.state);
    BaseWidget::hightQualityPainter(*p);
    p->setPen(palette.color(QPalette::Shadow));
    p->setBrush(palette.button());
    p->drawRoundedRect(rect, radius, radius);
    p->setPen(palette.color(QPalette::ButtonText));
    auto f = w->font();
    if (w->metaObject()->inherits(LinkButton().metaObject())) {
        if (opt.state.testAnyFlags(QStyle::State_MouseOver | QStyle::State_Sunken)) {
            f.setUnderline(true);
        }
    }
    p->setFont(f);
    ButtonType buttonType = (ButtonType)w->property(PROPERTY_BUTTON_TYPE).toInt();
    QString iconName = w->property(PROPERTY_ICON_NAME).toString();
    QImage svgImage;
    if (iconName != "") {
        svgImage = QImage(opt.iconSize * w->devicePixelRatio(), QImage::Format_ARGB32_Premultiplied);
        svgImage.fill(Qt::transparent);
        QPainter painter(&svgImage);
        BaseWidget::hightQualityPainter(painter);
        m_icons[iconName]->render(&painter);
        painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
        painter.setBrush(palette.color(QPalette::ButtonText));
        painter.setPen(Qt::NoPen);
        painter.drawRect(svgImage.rect());
    }
    if (buttonType == BT_Icon) {
        QRect iconRect(QPoint(0, 0), opt.iconSize);
        iconRect.moveCenter(rect.center());
        p->drawImage(iconRect, svgImage);
    } else {
        if (iconName != "") {
            int spacing = w->property(PROPERTY_SPACING).toInt();
            IconPosition ip = (IconPosition)w->property(PROPERTY_ICON_POSITION).toInt();
            auto textWidth = opt.fontMetrics.horizontalAdvance(opt.text);
            QRect r(0, 0, opt.iconSize.width() + spacing + textWidth, rect.height());
            r.moveCenter(rect.center());
            QRect iconRect(QPoint(0, 0), opt.iconSize);
            iconRect.moveCenter(r.center());
            if (ip == IP_LEFT) {
                iconRect.moveLeft(r.left());
                p->drawImage(iconRect, svgImage);
                r.setLeft(iconRect.right() + spacing);
                p->drawText(r, opt.text, QTextOption(Qt::AlignCenter));
            } else {
                iconRect.moveRight(r.right());
                p->drawImage(iconRect, svgImage);
                r.setRight(iconRect.left() - spacing);
                p->drawText(r, opt.text, QTextOption(Qt::AlignCenter));
            }
        } else {
            p->drawText(rect, opt.text, QTextOption(Qt::AlignCenter));
        }

    }

}

QPalette ElementStyle::buttonPalette(const QWidget *w, const State &state) const
{
    QPalette pal;
    bool darkMode = w->property(PROPERTY_DARK_MODE).toBool();
    bool plainMode = w->property(PROPERTY_PLAIN_MODE).toBool();
    ButtonType buttonType = (ButtonType)w->property(PROPERTY_BUTTON_TYPE).toInt();
    StyleMode styleMode = (StyleMode)w->property(PROPERTY_STYLE_MODE).toInt();
    if (w->metaObject()->inherits(LinkButton().metaObject())) {
        plainMode = true;
        buttonType = BT_Text;
    }
    const ThemeColors *theme = &s_light;
    if (darkMode) {
        theme = &s_dark;
    }
    const StyleColors *style = &theme->primary;
    if (styleMode == ST_Success) style = &theme->success;
    if (styleMode == ST_Warning) style = &theme->warning;
    if (styleMode == ST_Danger) style = &theme->danger;
    if (styleMode == ST_Info) style = &theme->info;
    auto defaultButtonPalette = [&] () {
        if (plainMode) {
            switch (styleMode) {
            case ST_Default:
                //disabled
                if (!state.testFlag(QStyle::State_Enabled)) {
                    pal.setColor(QPalette::Shadow, theme->border.base);
                    pal.setBrush(QPalette::Button, theme->fill.light);
                    pal.setColor(QPalette::ButtonText, theme->text.placeholder);
                    return pal;
                }
                //pressed
                if (state.testAnyFlag(QStyle::State_Sunken) || state.testAnyFlag(QStyle::State_On)) {
                    pal.setColor(QPalette::Shadow, theme->primary.dark);
                    pal.setBrush(QPalette::Button, theme->fill.blank);
                    pal.setColor(QPalette::ButtonText, theme->primary.dark);
                    return pal;
                }
                //hover
                if (state.testAnyFlag(QStyle::State_MouseOver)) {
                    pal.setColor(QPalette::Shadow, theme->primary.color20);
                    pal.setBrush(QPalette::Button, theme->fill.blank);
                    pal.setColor(QPalette::ButtonText, theme->primary.color);
                    return pal;
                }
                //focused
                if (state.testAnyFlag(QStyle::State_HasFocus)) {
                    pal.setColor(QPalette::Shadow, theme->primary.color20);
                    pal.setBrush(QPalette::Button, theme->fill.blank);
                    pal.setColor(QPalette::ButtonText, theme->primary.color);
                    return pal;
                }
                pal.setColor(QPalette::Shadow, theme->border.base);
                pal.setBrush(QPalette::Button, theme->fill.blank);
                pal.setColor(QPalette::ButtonText, theme->text.primary);
                break;
            case ST_Primary:
            case ST_Success:
            case ST_Warning:
            case ST_Danger:
            case ST_Info:
                //disabled
                pal.setColor(QPalette::Shadow, style->color);
                pal.setColor(QPalette::ButtonText, darkMode ? Qt::white : theme->fill.blank);
                if (!state.testFlag(QStyle::State_Enabled)) {
                    pal.setColor(QPalette::ButtonText, style->color50);
                    pal.setBrush(QPalette::Button, style->color10);
                    return pal;
                }
                //pressed
                if (state.testAnyFlag(QStyle::State_Sunken) || state.testAnyFlag(QStyle::State_On)) {
                    pal.setBrush(QPalette::Button, style->dark);
                    return pal;
                }
                //hover
                if (state.testAnyFlag(QStyle::State_MouseOver)) {
                    pal.setBrush(QPalette::Button, style->color);
                    return pal;
                }
                //focused
                if (state.testAnyFlag(QStyle::State_HasFocus)) {
                    pal.setBrush(QPalette::Button, style->color);
                    return pal;
                }
                pal.setColor(QPalette::ButtonText, style->color);
                pal.setBrush(QPalette::Button, style->color10);
                break;
            default:
                break;
            }
        } else {
            switch (styleMode) {
            case ST_Default:
                //disabled
                if (!state.testFlag(QStyle::State_Enabled)) {
                    pal.setColor(QPalette::Shadow, theme->border.base);
                    pal.setBrush(QPalette::Button, theme->fill.light);
                    pal.setColor(QPalette::ButtonText, theme->text.placeholder);
                    return pal;
                }
                //pressed
                if (state.testAnyFlag(QStyle::State_Sunken) || state.testAnyFlag(QStyle::State_On)) {
                    pal.setColor(QPalette::Shadow, theme->primary.dark);
                    pal.setBrush(QPalette::Button, theme->primary.color10);
                    pal.setColor(QPalette::ButtonText, theme->primary.dark);
                    return pal;
                }
                //hover
                if (state.testAnyFlag(QStyle::State_MouseOver)) {
                    pal.setColor(QPalette::Shadow, theme->primary.color20);
                    pal.setBrush(QPalette::Button, theme->primary.color10);
                    pal.setColor(QPalette::ButtonText, theme->primary.color);
                    return pal;
                }
                //focused
                if (state.testAnyFlag(QStyle::State_HasFocus)) {
                    pal.setColor(QPalette::Shadow, theme->primary.color20);
                    pal.setBrush(QPalette::Button, theme->primary.color10);
                    pal.setColor(QPalette::ButtonText, theme->primary.color);
                    return pal;
                }
                pal.setColor(QPalette::Shadow, theme->border.base);
                pal.setBrush(QPalette::Button, theme->fill.blank);
                pal.setColor(QPalette::ButtonText, theme->text.primary);
                break;
            case ST_Primary:
            case ST_Success:
            case ST_Warning:
            case ST_Danger:
            case ST_Info:
                //disabled
                pal.setColor(QPalette::Shadow, theme->basic.transparent);
                pal.setColor(QPalette::ButtonText, darkMode ? Qt::white : theme->fill.blank);
                if (!state.testFlag(QStyle::State_Enabled)) {
                    pal.setBrush(QPalette::Button, style->color50);
                    return pal;
                }
                //pressed
                if (state.testAnyFlag(QStyle::State_Sunken) || state.testAnyFlag(QStyle::State_On)) {
                    pal.setBrush(QPalette::Button, style->dark);
                    return pal;
                }
                //hover
                if (state.testAnyFlag(QStyle::State_MouseOver)) {
                    pal.setBrush(QPalette::Button, style->color50);
                    return pal;
                }
                //focused
                if (state.testAnyFlag(QStyle::State_HasFocus)) {
                    pal.setBrush(QPalette::Button, style->color50);
                    return pal;
                }
                pal.setBrush(QPalette::Button, style->color);
                break;
            default:
                break;
            }
        }
        return pal;
    };

    auto textButtonPalette = [&]() {
        if (plainMode) {
            switch (styleMode) {
            case ST_Default:
                pal.setColor(QPalette::ButtonText, theme->text.primary);
                pal.setBrush(QPalette::Button, theme->fill.light);
                pal.setColor(QPalette::Shadow, theme->basic.transparent);
                //disabled
                if (!state.testFlag(QStyle::State_Enabled)) {
                    pal.setColor(QPalette::ButtonText, theme->text.disable);
                    return pal;
                }
                //pressed
                if (state.testAnyFlag(QStyle::State_Sunken)) {
                    pal.setBrush(QPalette::Button, theme->fill.dark);
                    return pal;
                }
                //hover
                if (state.testAnyFlag(QStyle::State_MouseOver)) {
                    pal.setBrush(QPalette::Button, theme->fill.base);
                    return pal;
                }
                //focused
                if (state.testAnyFlag(QStyle::State_HasFocus)) {
                    pal.setColor(QPalette::Shadow, theme->primary.color50);
                    pal.setBrush(QPalette::Button, theme->fill.base);
                    return pal;
                }
                break;
            case ST_Primary:
            case ST_Success:
            case ST_Warning:
            case ST_Danger:
            case ST_Info:
                //disabled
                pal.setColor(QPalette::Shadow, theme->basic.transparent);
                pal.setColor(QPalette::ButtonText, style->color);
                if (!state.testFlag(QStyle::State_Enabled)) {
                    pal.setColor(QPalette::ButtonText, style->color50);
                    pal.setBrush(QPalette::Button, theme->fill.light);
                    return pal;
                }
                //pressed
                if (state.testAnyFlag(QStyle::State_Sunken)) {
                    pal.setBrush(QPalette::Button, theme->fill.dark);
                    return pal;
                }
                //hover
                if (state.testAnyFlag(QStyle::State_MouseOver)) {
                    pal.setBrush(QPalette::Button, theme->fill.dark);
                    return pal;
                }
                //focused
                if (state.testAnyFlag(QStyle::State_HasFocus)) {
                    pal.setColor(QPalette::Shadow, theme->primary.color50);
                    pal.setBrush(QPalette::Button, theme->fill.dark);
                    return pal;
                }
                pal.setBrush(QPalette::Button, theme->fill.light);
                break;
            default:
                break;
            }
        } else {
            switch (styleMode) {
            case ST_Default:
                pal.setColor(QPalette::ButtonText, theme->text.primary);
                pal.setColor(QPalette::Shadow, theme->basic.transparent);
                pal.setBrush(QPalette::Button, theme->basic.transparent);
                //disabled
                if (!state.testFlag(QStyle::State_Enabled)) {
                    pal.setColor(QPalette::ButtonText, theme->text.disable);
                    return pal;
                }
                //pressed
                if (state.testAnyFlag(QStyle::State_Sunken)) {
                    pal.setBrush(QPalette::Button, theme->fill.base);
                    return pal;
                }
                //hover
                if (state.testAnyFlag(QStyle::State_MouseOver)) {
                    pal.setBrush(QPalette::Button, theme->fill.light);
                    return pal;
                }
                //focused
                if (state.testAnyFlag(QStyle::State_HasFocus)) {
                    pal.setColor(QPalette::Shadow, theme->primary.color50);
                    pal.setBrush(QPalette::Button, theme->fill.light);
                    return pal;
                }
                break;
            case ST_Primary:
            case ST_Success:
            case ST_Warning:
            case ST_Danger:
            case ST_Info:
                pal.setColor(QPalette::ButtonText, style->color);
                pal.setColor(QPalette::Shadow, theme->basic.transparent);
                pal.setBrush(QPalette::Button, theme->basic.transparent);
                //disabled
                if (!state.testFlag(QStyle::State_Enabled)) {
                    pal.setColor(QPalette::ButtonText, style->color50);
                    return pal;
                }
                //pressed
                if (state.testAnyFlag(QStyle::State_Sunken)) {
                    pal.setBrush(QPalette::Button, theme->fill.base);
                    return pal;
                }
                //hover
                if (state.testAnyFlag(QStyle::State_MouseOver)) {
                    pal.setBrush(QPalette::Button, theme->fill.light);
                    return pal;
                }
                //focused
                if (state.testAnyFlag(QStyle::State_HasFocus)) {
                    pal.setColor(QPalette::Shadow, theme->primary.color50);
                    pal.setBrush(QPalette::Button, theme->fill.light);
                    return pal;
                }
                break;
            default:
                break;
            }
        }
        return pal;
    };
    // auto iconButtonPalette = [&]() {
    //     return pal;
    // };
    switch (buttonType) {
    case BT_Default:
    case BT_Icon:
        defaultButtonPalette();
        break;
    case BT_Text:
        textButtonPalette();
        break;
    default:
        break;
    }
    if (w->metaObject()->inherits(LinkButton().metaObject())) {
        pal.setColor(QPalette::Shadow, theme->basic.transparent);
        pal.setBrush(QPalette::Button, theme->basic.transparent);
    }
    return pal;
}

