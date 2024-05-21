#include "stylehelper.h"
#include <QWidget>
#include <QLineEdit>
#include <QAbstractScrollArea>
#include <QComboBox>
#include <QMetaProperty>
#include <QLabel>
#include <QApplication>
QStyleSheetStyleCaches *StyleHelper::styleSheetCaches = new QStyleSheetStyleCaches();
using namespace QCss;
QCss::Parser parser;
#define OBJECT_PTR(x) (static_cast<QObject *>(x.ptr))

static inline QObject *parentObject(const QObject *obj)
{
#if QT_CONFIG(tooltip)
    if (qobject_cast<const QLabel *>(obj) && qstrcmp(obj->metaObject()->className(), "QTipLabel") == 0) {
        QObject *p = qvariant_cast<QObject *>(obj->property("_q_stylesheet_parent"));
        if (p)
            return p;
    }
#endif
    return obj->parent();
}
static QWidget *containerWidget(const QWidget *w)
{
#if QT_CONFIG(lineedit)
    if (qobject_cast<const QLineEdit *>(w)) {
        //if the QLineEdit is an embeddedWidget, we need the rule of the real widget
#if QT_CONFIG(combobox)
        if (qobject_cast<const QComboBox *>(w->parentWidget()))
            return w->parentWidget();
#endif
#if QT_CONFIG(spinbox)
        if (qobject_cast<const QAbstractSpinBox *>(w->parentWidget()))
            return w->parentWidget();
#endif
    }
#endif // QT_CONFIG(lineedit)

#if QT_CONFIG(scrollarea)
    if (const QAbstractScrollArea *sa = qobject_cast<const QAbstractScrollArea *>(w->parentWidget())) {
        if (sa->viewport() == w)
            return w->parentWidget();
    }
#endif

    return const_cast<QWidget *>(w);
}

bool StyleHelper::unstylable(const QWidget *w)
{
    if (w->windowType() == Qt::Desktop)
        return true;

    if (!w->styleSheet().isEmpty())
        return false;

    if (containerWidget(w) != w)
        return true;

#ifndef QT_NO_FRAME
    // detect QComboBoxPrivateContainer
    else if (qobject_cast<const QFrame *>(w)) {
        if (0
#if QT_CONFIG(combobox)
            || qobject_cast<const QComboBox *>(w->parentWidget())
#endif
            )
            return true;
    }
#endif

#if QT_CONFIG(tabbar)
    if (w->metaObject() == &QWidget::staticMetaObject
        && qobject_cast<const QTabBar*>(w->parentWidget()))
        return true; // The moving tab of a QTabBar
#endif

    return false;
}

bool StyleHelper::initObject(const QObject *obj)
{
    if (!obj)
        return false;
    if (const QWidget *w = qobject_cast<const QWidget*>(obj)) {
        if (w->testAttribute(Qt::WA_StyleSheet))
            return true;
        if (unstylable(w))
            return false;
        const_cast<QWidget *>(w)->setAttribute(Qt::WA_StyleSheet, true);
    }

    QObject::connect(obj, SIGNAL(destroyed(QObject*)), styleSheetCaches, SLOT(objectDestroyed(QObject*)), Qt::UniqueConnection);
    return true;
}


class QStyleSheetStyleSelector : public StyleSelector
{
public:
    QStyleSheetStyleSelector() { }

    QStringList nodeNames(NodePtr node) const override
    {
        if (isNullNode(node))
            return QStringList();
        const QMetaObject *metaObject = OBJECT_PTR(node)->metaObject();
#if QT_CONFIG(tooltip)
        if (qstrcmp(metaObject->className(), "QTipLabel") == 0)
            return QStringList(QLatin1String("QToolTip"));
#endif
        QStringList result;
        do {
            result += QString::fromLatin1(metaObject->className()).replace(QLatin1Char(':'), QLatin1Char('-'));
            metaObject = metaObject->superClass();
        } while (metaObject != nullptr);
        return result;
    }
    QString attributeValue(NodePtr node, const QCss::AttributeSelector& aSelector) const override
    {
        if (isNullNode(node))
            return QString();

        const QString &name = aSelector.name;
        QHash<QString, QString> &cache = m_attributeCache[OBJECT_PTR(node)];
        QHash<QString, QString>::const_iterator cacheIt = cache.constFind(name);
        if (cacheIt != cache.constEnd())
            return cacheIt.value();

        QVariant value;
        QString valueStr;
        QObject *obj = OBJECT_PTR(node);
        const int propertyIndex = obj->metaObject()->indexOfProperty(name.toLatin1());
        if (propertyIndex == -1) {
            value = obj->property(name.toLatin1()); // might be a dynamic property
            if (!value.isValid()) {
                if (name == u"class"_qs) {
                    QString className = QString::fromLatin1(obj->metaObject()->className());
                    if (className.contains(QLatin1Char(':')))
                        className.replace(QLatin1Char(':'), QLatin1Char('-'));
                    valueStr = className;
                } else if (name == u"style"_qs) {
                    // QWidget *w = qobject_cast<QWidget *>(obj);
                    // QStyleSheetStyle *proxy = w ? qt_styleSheet(w->style()) : nullptr;
                    // if (proxy)
                    //     valueStr = QString::fromLatin1(proxy->baseStyle()->metaObject()->className());
                }
            }
        } else {
            const QMetaProperty property = obj->metaObject()->property(propertyIndex);
            value = property.read(obj);
            // support Qt 5 selector syntax, which required the integer enum value
            if (property.isEnumType()) {
                bool isNumber;
                aSelector.value.toInt(&isNumber);
                if (isNumber)
                    value.convert(QMetaType::fromType<int>());
            }
        }
        if (value.isValid()) {
            valueStr = (value.userType() == QMetaType::QStringList
                        || value.userType() == QMetaType::QVariantList)
                           ? value.toStringList().join(QLatin1Char(' '))
                           : value.toString();
        }
        cache[name] = valueStr;
        return valueStr;
    }
    bool nodeNameEquals(NodePtr node, const QString& nodeName) const override
    {
        if (isNullNode(node))
            return false;
        const QMetaObject *metaObject = OBJECT_PTR(node)->metaObject();
#if QT_CONFIG(tooltip)
        if (qstrcmp(metaObject->className(), "QTipLabel") == 0)
            return nodeName == QLatin1String("QToolTip");
#endif
        do {
            const ushort *uc = (const ushort *)nodeName.constData();
            const ushort *e = uc + nodeName.length();
            const uchar *c = (const uchar *)metaObject->className();
            while (*c && uc != e && (*uc == *c || (*c == ':' && *uc == '-'))) {
                ++uc;
                ++c;
            }
            if (uc == e && !*c)
                return true;
            metaObject = metaObject->superClass();
        } while (metaObject != nullptr);
        return false;
    }
    bool hasAttributes(NodePtr) const override
    { return true; }
    QStringList nodeIds(NodePtr node) const override
    { return isNullNode(node) ? QStringList() : QStringList(OBJECT_PTR(node)->objectName()); }
    bool isNullNode(NodePtr node) const override
    { return node.ptr == nullptr; }
    NodePtr parentNode(NodePtr node) const override
    { NodePtr n; n.ptr = isNullNode(node) ? nullptr : parentObject(OBJECT_PTR(node)); return n; }
    NodePtr previousSiblingNode(NodePtr) const override
    { NodePtr n; n.ptr = nullptr; return n; }
    NodePtr duplicateNode(NodePtr node) const override
    { return node; }
    void freeNode(NodePtr) const override
    { }

private:
    mutable QHash<const QObject *, QHash<QString, QString> > m_attributeCache;
};

QList<QCss::StyleRule> StyleHelper::styleRules(const QObject *obj)
{
    QHash<const QObject *, QList<StyleRule>>::const_iterator cacheIt =
        styleSheetCaches->styleRulesCache.constFind(obj);
    if (cacheIt != styleSheetCaches->styleRulesCache.constEnd())
        return cacheIt.value();

    if (!initObject(obj)) {
        return QList<StyleRule>();
    }

    QStyleSheetStyleSelector styleSelector;

    if (!qApp->styleSheet().isEmpty()) {
        StyleSheet appSs;
        QHash<const void *, StyleSheet>::const_iterator appCacheIt = styleSheetCaches->styleSheetCache.constFind(qApp);
        if (appCacheIt == styleSheetCaches->styleSheetCache.constEnd()) {
            QString ss = qApp->styleSheet();
            if (ss.startsWith(QLatin1String("file:///")))
                ss.remove(0, 8);
            parser.init(ss, qApp->styleSheet() != ss);
            if (Q_UNLIKELY(!parser.parse(&appSs)))
                qWarning("Could not parse application stylesheet");
            appSs.origin = StyleSheetOrigin_Inline;
            appSs.depth = 1;
            styleSheetCaches->styleSheetCache.insert(qApp, appSs);
        } else {
            appSs = appCacheIt.value();
        }
        styleSelector.styleSheets += appSs;
    }

    QList<QCss::StyleSheet> objectSs;
    for (const QObject *o = obj; o; o = parentObject(o)) {
        QString styleSheet = o->property("styleSheet").toString();
        if (styleSheet.isEmpty())
            continue;
        StyleSheet ss;
        QHash<const void *, StyleSheet>::const_iterator objCacheIt = styleSheetCaches->styleSheetCache.constFind(o);
        if (objCacheIt == styleSheetCaches->styleSheetCache.constEnd()) {
            parser.init(styleSheet);
            if (!parser.parse(&ss)) {
                parser.init(QLatin1String("* {") + styleSheet + QLatin1Char('}'));
                if (Q_UNLIKELY(!parser.parse(&ss)))
                    qWarning() << "Could not parse stylesheet of object" << o;
            }
            ss.origin = StyleSheetOrigin_Inline;
            styleSheetCaches->styleSheetCache.insert(o, ss);
        } else {
            ss = objCacheIt.value();
        }
        objectSs.append(ss);
    }

    for (int i = 0; i < objectSs.count(); i++)
        objectSs[i].depth = objectSs.count() - i + 2;

    styleSelector.styleSheets += objectSs;

    StyleSelector::NodePtr n;
    n.ptr = const_cast<QObject *>(obj);
    QList<QCss::StyleRule> rules = styleSelector.styleRulesForNode(n);
    styleSheetCaches->styleRulesCache.insert(obj, rules);
    return rules;
}

void QStyleSheetStyleCaches::objectDestroyed(QObject *o)
{
    styleRulesCache.remove(o);
    hasStyleRuleCache.remove(o);
    customPaletteWidgets.remove((const QWidget *)o);
    customFontWidgets.remove(static_cast<QWidget *>(o));
    styleSheetCache.remove(o);
    autoFillDisabledWidgets.remove((const QWidget *)o);
}

void QStyleSheetStyleCaches::styleDestroyed(QObject *o)
{
    styleSheetCache.remove(o);
}
