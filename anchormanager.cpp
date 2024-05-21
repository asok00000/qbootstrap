#include "anchormanager.h"
#include <QWidget>
#include <QEvent>
#include <QPropertyAnimation>
#include <QDebug>
AnchorManager::AnchorManager(QObject *parent)
    : QObject{parent}
{}

bool AnchorManager::installAnchor(QWidget *item, AnchorTypes anchors, QMargins margins, QWidget *source, bool animated)
{
    if (source == nullptr) {
        source = item->parentWidget();
    } else if (source->parentWidget() != item->parentWidget()) {
        qWarning() << "Only can anchor parent or siblings";
        return false;
    }
    if (!item || anchors.testFlag(AnchorType::AnchorNone) || source == nullptr) return false;
    std::map<AnchorType, AnchorType> anchorsMap;
    for (int i = 0; i < sizeof(AnchorTypes)*8; i++) {
        AnchorType temp = static_cast<AnchorType>(1 << i);
        if (anchors.testFlag(temp)) {
            anchorsMap[temp] = temp;
        }
    }
    auto anchorInfo = AnchorInfo({animated, item, margins, anchorsMap});
    m_anchors.insert(source, anchorInfo);
    source->installEventFilter(this);
    updateAnchor(anchorInfo, source);
    return true;
}

bool AnchorManager::installAnchor(QWidget *item, std::map<AnchorType, AnchorType> anchors, QMargins margins, QWidget *source, bool animated)
{
    if (source == nullptr) {
        source = item->parentWidget();
    } else if (source->parentWidget() != item->parentWidget()) {
        qWarning() << "Only can anchor parent or siblings";
        return false;
    }
    if (!item || anchors.size() <= 0 || source == nullptr) return false;
    if (anchors.find(AnchorType::AnchorFill) != anchors.end()) {
        anchors.clear();
        anchors = {{AnchorLeft, AnchorLeft},
                   {AnchorTop, AnchorTop},
                   {AnchorRight, AnchorRight},
                   {AnchorBottom, AnchorBottom}};
    }
    auto anchorInfo = AnchorInfo({animated, item, margins, anchors});
    m_anchors.insert(source, anchorInfo);
    source->installEventFilter(this);
    updateAnchor(anchorInfo, source);
    return true;
}

void AnchorManager::removeAnchors(QWidget *item)
{
    if (item) {
        removeEventFilter(item);
        m_anchors.remove(item->parentWidget());
    }
}

bool AnchorManager::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::Resize || event->type() == QEvent::Move || event->type() == QEvent::Destroy) {
        auto watchedWidget = qobject_cast<QWidget* >(watched);
        if (m_anchors.contains(watchedWidget)) {
            if (event->type() == QEvent::Destroy) {
                m_anchors.remove(watchedWidget);
            } else {
                auto anchorInfos = m_anchors.values(watchedWidget);
                for (const auto &anchorInfo : anchorInfos) {
                    updateAnchor(anchorInfo, watchedWidget);
                }
            }
        }
    }
    return QObject::eventFilter(watched, event);
}

AnchorManager *AnchorManager::instance()
{
    static AnchorManager manager;
    return &manager;
}

void AnchorManager::updateAnchor(AnchorInfo anchorInfo, QWidget *source)
{
    auto target = anchorInfo.target;
    if (target == nullptr || source == nullptr) {
        return;
    }
    auto anchorMap = anchorInfo.anchors;
    auto margins = anchorInfo.margins;
    auto rect = target->rect(), sourceRect = source->rect();
    if (source != target->parentWidget()) {
        sourceRect = source->geometry();
    }
    QVector<AnchorType> skipTypes;
    for (auto& [targetAnchorType, sourceAnchorType]: anchorMap) {
        if (skipTypes.contains(targetAnchorType)) continue;
        switch (targetAnchorType) {
        case AnchorType::AnchorHCenter:
            skipTypes << AnchorType::AnchorLeft << AnchorType::AnchorRight;
            rect.moveLeft(sourceRect.left() +  sourceRect.width() / 2 - rect.width() / 2);
            break;
        case AnchorType::AnchorVCenter:
            skipTypes << AnchorType::AnchorTop << AnchorType::AnchorBottom;
            rect.moveTop(sourceRect.top() + sourceRect.height() / 2 - rect.height() / 2);
            break;
        case AnchorType::AnchorLeft:
            if (sourceAnchorType == targetAnchorType) {
                rect.moveLeft(sourceRect.left() + margins.left());
            } else if (sourceAnchorType == AnchorType::AnchorRight) {
                rect.moveLeft(sourceRect.right() + margins.left());
            }
            break;
        case AnchorType::AnchorTop:
            if (sourceAnchorType == targetAnchorType) {
                rect.moveTop(sourceRect.top() + margins.top());
            } else if (sourceAnchorType == AnchorType::AnchorBottom) {
                rect.moveLeft(sourceRect.bottom() + margins.left());
            }
            break;
        case AnchorType::AnchorRight:
            if (sourceAnchorType == targetAnchorType) {
                if (anchorMap.find(AnchorType::AnchorLeft) != anchorMap.end()) {
                    rect.setRight(sourceRect.right() - margins.right());
                } else {
                    rect.moveRight(sourceRect.right() - margins.right());
                }
            } else if (sourceAnchorType == AnchorType::AnchorLeft) {
                if (anchorMap.find(AnchorType::AnchorLeft) != anchorMap.end()) {
                    rect.setRight(sourceRect.left() - margins.right());
                } else {
                    rect.moveRight(sourceRect.left() - margins.right());
                }
            }
            break;
        case AnchorType::AnchorBottom:
            if (sourceAnchorType == targetAnchorType) {
                if (anchorMap.find(AnchorType::AnchorTop) != anchorMap.end()) {
                    rect.setBottom(sourceRect.bottom() - margins.bottom());
                } else {
                    rect.moveBottom(sourceRect.bottom() - margins.bottom());
                }
            } else if (sourceAnchorType == AnchorType::AnchorTop) {
                if (anchorMap.find(AnchorType::AnchorTop) != anchorMap.end()) {
                    rect.setBottom(sourceRect.top() - margins.bottom());
                } else {
                    rect.moveBottom(sourceRect.top() - margins.bottom());
                }
            }
            break;
        default:
            qDebug() << "skip " << targetAnchorType;
        }
    }
    if (anchorInfo.animated) {
        auto animation = new QPropertyAnimation(target, "geometry");
        animation->setDuration(300);
        animation->setEasingCurve(QEasingCurve::InCurve);
        animation->setStartValue(target->geometry());
        animation->setEndValue(rect);
        animation->start(QAbstractAnimation::DeleteWhenStopped);
    } else {
        target->setGeometry(rect);
    }
}
