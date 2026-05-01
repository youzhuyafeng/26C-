#include "CharacterItem.h"
#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>
#include <QRect>
#include <QDebug>
#include "Enemy.h"
#include "GameWindow.h"
#include "GameController.h"

CharacterItem::CharacterItem(Entity* entity, const QString& name, const QString& avatarPath,
    const QSize& avatarSize, QGraphicsItem* parent)
    : QGraphicsObject(parent), m_entity(entity), m_name(name), m_avatarSize(avatarSize),
    m_isEnemy(false)
{
    setAcceptDrops(true);
    if (!m_avatar.load(avatarPath))
        qWarning() << "Failed load avatar:" << avatarPath;

    m_weakIcon.load("res/Buff/weak_power.png");
    m_vulnerableIcon.load("res/Buff/vulnerable_power.png");
    m_strengthIcon.load("res/Buff/strength_power.png");

    if (dynamic_cast<Enemy*>(entity)) {
        m_isEnemy = true;
        // 预创建最多3组图标和文本（足够覆盖攻击+防御+增益）
        for (int i = 0; i < 3; ++i) {
            QGraphicsPixmapItem* icon = new QGraphicsPixmapItem(this);
            icon->setVisible(false);
            icon->setZValue(1);
            m_intentIcons.append(icon);

            QGraphicsSimpleTextItem* text = new QGraphicsSimpleTextItem(this);
            text->setVisible(false);
            text->setZValue(2);
            text->setFont(QFont("微软雅黑", 10, QFont::Bold));
            text->setBrush(Qt::white);
            m_intentTexts.append(text);
        }
        updateIntentIcons();
    }

    updateData();
}

CharacterItem::~CharacterItem()
{
    // 子对象自动删除
}

QRectF CharacterItem::boundingRect() const
{
    int width = qMax(m_avatarSize.width(), 100);
    int height = m_avatarSize.height() + 20 + 18 + 32 + 10;
    return QRectF(0, 0, width, height);
}

void CharacterItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    painter->save();

    if (!m_avatar.isNull()) {
        QRect targetRect(0, 0, m_avatarSize.width(), m_avatarSize.height());
        painter->drawPixmap(targetRect, m_avatar);
    }
    else {
        painter->fillRect(0, 0, m_avatarSize.width(), m_avatarSize.height(), QColor(150, 150, 150));
    }

    int currentY = m_avatarSize.height() + 5;
    int barWidth = qMax(m_avatarSize.width(), 100);

    int hp = m_entity->hp(), maxHp = m_entity->maxHp();
    double percent = (double)hp / maxHp;
    painter->setBrush(Qt::red);
    painter->drawRect(0, currentY, barWidth, 18);
    painter->setBrush(Qt::green);
    painter->drawRect(0, currentY, barWidth * percent, 18);
    painter->setPen(Qt::white);
    painter->drawText(QRectF(0, currentY, barWidth, 18), Qt::AlignCenter,
        QString("%1 / %2").arg(hp).arg(maxHp));
    currentY += 20;

    int block = m_entity->block();
    if (block > 0) {
        painter->setBrush(QColor(200, 200, 100));
        painter->drawRect(0, currentY, barWidth, 16);
        painter->drawText(QRectF(0, currentY, barWidth, 16), Qt::AlignCenter,
            QString("格挡 %1").arg(block));
        currentY += 18;
    }

    int iconSize = 28;
    int iconSpacing = 5;
    int iconStartX = 0;
    int iconY = currentY;
    for (const Buff& b : m_entity->buffs()) {
        if (b.amount <= 0) continue;
        QPixmap* icon = nullptr;
        if (b.type == Buff::Weak) icon = &m_weakIcon;
        else if (b.type == Buff::Vulnerable) icon = &m_vulnerableIcon;
        else if (b.type == Buff::Strength) icon = &m_strengthIcon;
        if (icon && !icon->isNull()) {
            painter->drawPixmap(QRect(iconStartX, iconY, iconSize, iconSize), *icon);
            painter->setPen(Qt::white);
            painter->setFont(QFont("微软雅黑", 8, QFont::Bold));
            painter->drawText(iconStartX + iconSize - 12, iconY + iconSize - 4,
                QString::number(b.amount));
            iconStartX += iconSize + iconSpacing;
            if (iconStartX + iconSize > barWidth) break;
        }
    }

    painter->restore();
}

void CharacterItem::updateData()
{
    update();
    if (m_isEnemy) {
        updateIntentIcons();
    }
}

void CharacterItem::updateIntentIcons()
{
    if (!m_isEnemy) return;
    Enemy* enemy = dynamic_cast<Enemy*>(m_entity);
    if (!enemy) return;

    const Intent& intent = enemy->currentIntent();
    struct IconInfo { QString path; int damage; bool isAttack; };
    QVector<IconInfo> infos;

    if (intent.damage > 0) {
        int dmg = intent.damage;
        QString attackPath;
        if (dmg < 5) attackPath = "res/intent/attack/intent_attack_1.png";
        else if (dmg < 15) attackPath = "res/intent/attack/intent_attack_2.png";
        else if (dmg < 25) attackPath = "res/intent/attack/intent_attack_3.png";
        else if (dmg < 40) attackPath = "res/intent/attack/intent_attack_4.png";
        else attackPath = "res/intent/attack/intent_attack_5.png";
        infos.append({ attackPath, dmg, true });
    }
    if (intent.blockSelf > 0) {
        infos.append({ "res/intent/defend/intent_defend.png", 0, false });
    }
    if (intent.hasBuff() && intent.buffType == Buff::Strength) {
        infos.append({ "res/intent/buff/intent_buff.png", 0, false });
    }

    const int iconSize = 48;
    const int spacing = 5;
    int totalWidth = 0;
    for (const auto& info : infos) {
        totalWidth += iconSize + spacing;
    }
    if (!infos.isEmpty()) totalWidth -= spacing;
    int startX = qMax(0, (m_avatarSize.width() - totalWidth) / 2);

    for (int i = 0; i < infos.size(); ++i) {
        if (i >= m_intentIcons.size()) break;
        QGraphicsPixmapItem* iconItem = m_intentIcons[i];
        QGraphicsSimpleTextItem* textItem = m_intentTexts[i];
        const IconInfo& info = infos[i];
        QPixmap pixmap(info.path);
        if (!pixmap.isNull()) {
            QPixmap scaled = pixmap.scaled(iconSize, iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            iconItem->setPixmap(scaled);
            qreal x = startX + i * (iconSize + spacing);
            qreal y = -scaled.height() - 5;
            iconItem->setPos(x, y);
            iconItem->setVisible(true);

            // 处理攻击数值文本
            if (info.isAttack) {
                textItem->setText(QString::number(info.damage));
                // 文本居中：位于图标正下方
                QRectF textRect = textItem->boundingRect();
                qreal textX = x + (scaled.width() - textRect.width()) / 2;
                qreal textY = y + scaled.height() + 2;
                textItem->setPos(textX, textY);
                textItem->setVisible(true);
            }
            else {
                textItem->setVisible(false);
            }
        }
        else {
            qWarning() << "Failed load intent icon:" << info.path;
            iconItem->setVisible(false);
            textItem->setVisible(false);
        }
    }
    // 隐藏多余的图标和文本
    for (int i = infos.size(); i < m_intentIcons.size(); ++i) {
        m_intentIcons[i]->setVisible(false);
        m_intentTexts[i]->setVisible(false);
    }
}

void CharacterItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    event->accept();
}

void CharacterItem::dragEnterEvent(QGraphicsSceneDragDropEvent* event)
{
    if (event->mimeData()->hasText()) {
        event->setAccepted(true);
        event->setDropAction(Qt::MoveAction);
    }
    else {
        event->setAccepted(false);
    }
}

void CharacterItem::dropEvent(QGraphicsSceneDragDropEvent* event)
{
    if (!event->mimeData()->hasText()) return;
    quintptr ptr = event->mimeData()->text().toULongLong();
    Card* card = reinterpret_cast<Card*>(ptr);

    GameWindow* win = qobject_cast<GameWindow*>(scene()->views().first()->window());
    if (win) {
        GameController* gc = win->getController();
        bool valid = false;
        for (Card* c : gc->handCards()) {
            if (c == card) { valid = true; break; }
        }
        if (!valid) return;
    }
    emit cardDropped(card);
    event->setDropAction(Qt::MoveAction);
    event->accept();
}