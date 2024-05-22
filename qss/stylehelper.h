#ifndef STYLEHELPER_H
#define STYLEHELPER_H
#include "qcssparser_p.h"
class QStyleSheetStyleCaches : public QObject
{
    Q_OBJECT
public Q_SLOTS:
    void objectDestroyed(QObject *);
    void styleDestroyed(QObject *);
public:
    QHash<const QObject *, QList<QCss::StyleRule>> styleRulesCache;
    QHash<const QObject *, QHash<int, bool> > hasStyleRuleCache;
    QHash<const void *, QCss::StyleSheet> styleSheetCache; // parsed style sheets
    QSet<const QWidget *> autoFillDisabledWidgets;
    // widgets with whose palettes and fonts we have tampered:
    template <typename T>
    struct Tampered {
        T oldWidgetValue;
        decltype(std::declval<T>().resolveMask()) resolveMask;

        // only call this function on an rvalue *this (it mangles oldWidgetValue)
        T reverted(T current)
#ifdef Q_COMPILER_REF_QUALIFIERS
            &&
#endif
        {
            oldWidgetValue.setResolveMask(oldWidgetValue.resolveMask() & resolveMask);
            current.setResolveMask(current.resolveMask() & ~resolveMask);
            current.resolve(oldWidgetValue);
            current.setResolveMask(current.resolveMask() | oldWidgetValue.resolveMask());
            return current;
        }
    };
    QHash<const QWidget *, Tampered<QPalette>> customPaletteWidgets;
    QHash<const QWidget *, Tampered<QFont>> customFontWidgets;
};
template <typename T>
class QTypeInfo<QStyleSheetStyleCaches::Tampered<T>>
    : public QTypeInfoMerger<QStyleSheetStyleCaches::Tampered<T>, T> {};

class StyleHelper {
public:
    static QStyleSheetStyleCaches *styleSheetCaches;
    static bool unstylable(const QWidget *w);
    static bool initObject(const QObject *obj);
    static QList<QCss::StyleRule> styleRules(const QObject *obj, bool fromCache = true);
};

#endif // STYLEHELPER_H
