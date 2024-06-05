#include "elementstyle.h"
#include <QDebug>
#include <QWidget>
#include <QStyleOption>
#include <QPainter>
#include <QEvent>
#include <QSvgRenderer>
#include <QPushButton>
#include <QRadioButton>
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
#define PROPERTY_HAS_BORDER "hasBorder"
static const ElementStyle::ThemeColors s_light = {
    {/*primary*/QColor("#337ECC"), QColor("#409EFF"), QColor("#79BBFF"), QColor("#9FCEFF"), QColor("#C6E2FF"), QColor("#D9ECFF"), QColor("#ECF5FF")},
    {/*success*/QColor("#529B2E"), QColor("#67C23A"), QColor("#95D475"), QColor("#B3E09C"), QColor("#D1EDC4"), QColor("#E1F3D8"), QColor("#F0F9EB")},
    {/*warning*/QColor("#B88230"), QColor("#E6A23C"), QColor("#EEBE77"), QColor("#F2D09D"), QColor("#F8E3C5"), QColor("#FAECD8"), QColor("#FCF6EC")},
    {/*danger*/QColor("#C45656"), QColor("#F56C6C"), QColor("#F89898"), QColor("#FAB5B5"), QColor("#FCD3D3"), QColor("#FDE2E2"), QColor("#FEF0F0")},
    {/*info*/QColor("#73767A"), QColor("#909399"), QColor("#B1B3B8"), QColor("#C7C9CC"), QColor("#DEDFE0"), QColor("#E9E9EB"), QColor("#F4F4F5")},
    {/*text*/QColor("#303133"), QColor("#606266"), QColor("#909399"), QColor("#A8ABB2"), QColor("#C0C4CC")},
    {/*border*/QColor("#CDD0D6"), QColor("#D4D7DE"), QColor("#DCDFE6"), QColor("#E4E7ED"), QColor("#EBEEF5"), QColor("#F2F6FC")},
    {/*fill*/QColor("#E6E8EB"), QColor("#EBEDF0"), QColor("#F0F2F5"), QColor("#F5F7FA"), QColor("#FAFAFA"), QColor("#FAFCFF"), QColor("#FFFFFF")},
    {/*background*/QColor("#F2F3F5"), QColor("#FFFFFF"), QColor("#FFFFFF")},
    {/*basic*/QColor("#F2F3F5"), QColor("#FFFFFF"), Qt::transparent}
};
static const ElementStyle::ThemeColors s_dark = {
    {/*primary*/QColor("#66B1FF"), QColor("#409EFF"), QColor("#2C6EB2"), QColor("#204F7F"), QColor("#132F4C"), QColor("#0C1F33"), QColor("#060F19")},
    {/*success*/QColor("#7AC17A"), QColor("#59B259"), QColor("#3E7C3E"), QColor("#2C592C"), QColor("#1A351A"), QColor("#112311"), QColor("#081108")},
    {/*warning*/QColor("#DEB578"), QColor("#D6A356"), QColor("#95723C"), QColor("#6B512B"), QColor("#403019"), QColor("#2A2011"), QColor("#151008")},
    {/*danger*/QColor("#D67E7E"), QColor("#CC5E5E"), QColor("#8E4141"), QColor("#662F2F"), QColor("#3D1C1C"), QColor("#281212"), QColor("#140909")},
    {/*info*/QColor("#808185"), QColor("#606266"), QColor("#434447"), QColor("#303133"), QColor("#1C1D1E"), QColor("#131314"), QColor("#09090A")},
    {/*text*/QColor("#E4E8F2"), QColor("#CCD0D8"), QColor("#9C9FA5"), QColor("#84868C"), QColor("#606266")},
    {/*border*/QColor("#555659"), QColor("#494A4C"), QColor("#3D3E3F"), QColor("#313133"), QColor("#242526"), QColor("#181819")},
    {/*fill*/QColor("#323233"), QColor("#282828"), QColor("#1E1E1E"), QColor("#141414"), QColor("#0A0A0A"), QColor("#323233"), QColor("#000000")},
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
    case QStyle::CE_RadioButton:
    {
        auto buttonOpt = *(qstyleoption_cast<const QStyleOptionButton *>(opt));
        drawRadioButton(element, buttonOpt, p, w);
    }
        break;
    default:
        QProxyStyle::drawControl(element, opt, p, w);
        break;
    }
}

void ElementStyle::drawComplexControl(ComplexControl cc, const QStyleOptionComplex *opt, QPainter *p, const QWidget *widget) const
{
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
    if (widget->metaObject()->inherits(QRadioButton().metaObject()) && !widget->property(PROPERTY_HAS_BORDER).isValid()) {
        widget->setProperty(PROPERTY_HAS_BORDER, false);
    }
    widget->installEventFilter(this);
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
    case QStyle::SE_RadioButtonClickRect:
    case QStyle::SE_PushButtonBevel:
        return widget->rect();
    default:
        break;
    }
    return QProxyStyle::subElementRect(subElement, option, widget);
}

bool ElementStyle::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::DynamicPropertyChange) {
        auto w = qobject_cast<QWidget *>(watched);
        if (w) {
            qDebug() << "a";
            w->style()->polish(w);
        }
    }
    return QProxyStyle::eventFilter(watched, event);
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

void ElementStyle::drawRadioButton(ControlElement element, const QStyleOptionButton &opt, QPainter *p, const QWidget *w) const
{
    auto rect = opt.rect;
    rect.adjust(1, 1, -1, -1);
    int radius = rect.height() / 2;
    if (!w->property(PROPERTY_ROUND_MODE).toBool()) {
        radius = rect.height() / 8;
    }
    QPalette &&palette = radioPalette(w, opt.state);
    BaseWidget::hightQualityPainter(*p);
    p->setPen(palette.color(QPalette::Shadow));
    if (w->property(PROPERTY_HAS_BORDER).toBool()) {
        p->setBrush(Qt::NoBrush);
        p->drawRoundedRect(rect, radius, radius);
    }
    p->setBrush(palette.button());
    p->setFont(w->font());
    int spacing = w->property(PROPERTY_SPACING).toInt();
    auto textWidth = opt.fontMetrics.horizontalAdvance(opt.text);
    QSize iconSize(12, 12);
    QRect r(0, 0, iconSize.width() + spacing + textWidth, rect.height());
    r.moveCenter(rect.center());
    QRect iconRect(QPoint(0, 0), iconSize);
    iconRect.moveCenter(r.center());
    iconRect.moveLeft(r.left());

    if (opt.state.testFlag(QStyle::State_On)) {
        p->setBrush(p->pen().color());
        p->drawRoundedRect(iconRect, 6, 6);
        auto checkRect = QRect(QPoint(0, 0), iconSize / 2);
        checkRect.moveCenter(iconRect.center());
        p->setBrush(palette.color(QPalette::Button));
        p->drawRoundedRect(checkRect, iconSize.width() / 2, iconSize.width() / 2);
    } else {
        p->drawRoundedRect(iconRect, 6, 6);
    }
    r.setLeft(iconRect.right() + spacing);
    p->setPen(palette.color(QPalette::ButtonText));
    p->drawText(r, opt.text, QTextOption(Qt::AlignCenter));
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
                pal.setBrush(QPalette::Button, theme->fill.blank);
                if (!state.testFlag(QStyle::State_Enabled)) {
                    pal.setColor(QPalette::Shadow, theme->border.light);
                    pal.setColor(QPalette::ButtonText, theme->text.placeholder);
                    return pal;
                }
                //pressed
                if (state.testAnyFlag(QStyle::State_Sunken) || state.testAnyFlag(QStyle::State_On)) {
                    pal.setColor(QPalette::Shadow, theme->primary.dark);
                    pal.setColor(QPalette::ButtonText, theme->primary.dark);
                    return pal;
                }
                //hover & focused
                if (state.testAnyFlags(QStyle::State_MouseOver | QStyle::State_HasFocus)) {
                    pal.setColor(QPalette::Shadow, theme->primary.color);
                    pal.setColor(QPalette::ButtonText, theme->primary.color);
                    return pal;
                }
                pal.setColor(QPalette::Shadow, theme->border.base);
                pal.setColor(QPalette::ButtonText, theme->text.regular);
                break;
            case ST_Primary:
            case ST_Success:
            case ST_Warning:
            case ST_Danger:
            case ST_Info:
                //disabled
                pal.setColor(QPalette::ButtonText, darkMode ? Qt::white : theme->fill.blank);
                if (!state.testFlag(QStyle::State_Enabled)) {
                    pal.setColor(QPalette::Shadow, style->color50);
                    pal.setColor(QPalette::ButtonText, style->color50);
                    pal.setBrush(QPalette::Button, style->color10);
                    return pal;
                }
                //pressed
                if (state.testAnyFlag(QStyle::State_Sunken) || state.testAnyFlag(QStyle::State_On)) {
                    pal.setColor(QPalette::Shadow, Qt::transparent);
                    pal.setBrush(QPalette::Button, style->dark);
                    return pal;
                }
                //hover & focused
                if (state.testAnyFlags(QStyle::State_MouseOver | QStyle::State_HasFocus)) {
                    pal.setColor(QPalette::Shadow, Qt::transparent);
                    pal.setBrush(QPalette::Button, style->color);
                    return pal;
                }
                pal.setColor(QPalette::Shadow, style->color);
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
                    pal.setColor(QPalette::Shadow, theme->border.light);
                    pal.setBrush(QPalette::Button, theme->fill.blank);
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
                //hover & focused
                if (state.testAnyFlags(QStyle::State_MouseOver | QStyle::State_HasFocus)) {
                    pal.setColor(QPalette::Shadow, theme->primary.color);
                    pal.setBrush(QPalette::Button, theme->primary.color10);
                    pal.setColor(QPalette::ButtonText, theme->primary.color);
                    return pal;
                }
                pal.setColor(QPalette::Shadow, theme->border.base);
                pal.setBrush(QPalette::Button, theme->fill.blank);
                pal.setColor(QPalette::ButtonText, theme->text.regular);
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
                //hover & focused
                if (state.testAnyFlags(QStyle::State_MouseOver | QStyle::State_HasFocus)) {
                    pal.setBrush(QPalette::Button, style->color70);
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

QPalette ElementStyle::radioPalette(const QWidget *w, const State &state) const
{
    QPalette pal;
    bool darkMode = w->property(PROPERTY_DARK_MODE).toBool();
    StyleMode styleMode = (StyleMode)w->property(PROPERTY_STYLE_MODE).toInt();
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
        switch (styleMode) {
        case ST_Default:
        case ST_Primary:
        case ST_Success:
        case ST_Warning:
        case ST_Danger:
        case ST_Info:
            //disabled
            if (!state.testFlag(QStyle::State_Enabled)) {
                pal.setColor(QPalette::Shadow, theme->border.light);
                pal.setBrush(QPalette::Button, theme->fill.light);
                pal.setColor(QPalette::ButtonText, theme->text.placeholder);
                return pal;
            }
            //hover & focused & pressed
            if (state.testAnyFlags(QStyle::State_MouseOver | QStyle::State_HasFocus | QStyle::State_Sunken | QStyle::State_On)) {
                pal.setColor(QPalette::Shadow, style->color);
                pal.setBrush(QPalette::Button, theme->fill.blank);
                pal.setColor(QPalette::ButtonText, style->color);
                return pal;
            }
            pal.setColor(QPalette::Shadow, theme->border.base);
            pal.setBrush(QPalette::Button, theme->fill.blank);
            pal.setColor(QPalette::ButtonText, theme->text.regular);
            break;
        default:
            break;
        }
        return pal;
    };
    defaultButtonPalette();
    return pal;
}

