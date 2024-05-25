#ifndef ANCHORMANAGER_H
#define ANCHORMANAGER_H

#include <QMargins>
#include <QObject>
#include <QMultiHash>
#include <QMap>
#include "qbootstrap_global.h"
class QWidget;
class QAbstractAnimation;
class QBOOTSTRAP_EXPORT AnchorManager : public QObject
{
    Q_OBJECT

public:
    enum AnchorType {
        AnchorLeft = 0x1,
        AnchorTop = 0x2,
        AnchorRight = 0x4,
        AnchorBottom = 0x8,
        AnchorHCenter = 0x10,
        AnchorVCenter = 0x20,
        AnchorCenter = AnchorHCenter | AnchorVCenter,
        AnchorFill = AnchorLeft | AnchorTop | AnchorRight | AnchorBottom,
        AnchorNone = 0
    };
    Q_DECLARE_FLAGS(AnchorTypes, AnchorType)
    Q_FLAG(AnchorTypes)
    struct AnchorInfo
    {
        bool animated = false;
        QWidget* target = nullptr;
        QMargins margins;
        std::map<AnchorType, AnchorType> anchors;
    };
    bool installAnchor(QWidget* item, AnchorTypes anchors = AnchorType::AnchorNone, QMargins margins = QMargins(), QWidget* source = nullptr, bool animated = false);
    bool installAnchor(QWidget* item, std::map<AnchorType, AnchorType> anchors, QMargins margins = QMargins(), QWidget* source = nullptr, bool animated = false);
    void removeAnchors(QWidget* item);
    bool eventFilter(QObject *watched, QEvent *event);
    static AnchorManager *instance();
private:
    explicit AnchorManager(QObject *parent = nullptr);
    void updateAnchor(AnchorInfo anchorInfo, QWidget* source);
signals:

private:
    QMultiHash<QWidget*, AnchorInfo> m_anchors;
};

#endif // ANCHORMANAGER_H
