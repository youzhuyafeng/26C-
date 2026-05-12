#pragma once
#ifndef CHARACTERITEM_H
#define CHARACTERITEM_H

#include <QGraphicsObject>
#include <QPainter>
#include <QSize>
#include <QGraphicsPixmapItem>
#include <QGraphicsSimpleTextItem>
#include <QVector>
#include "Entity.h"
#include "Card.h"

class GameController;

class CharacterItem : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit CharacterItem(Entity* entity, const QString& name, const QString& avatarPath,
        const QSize& avatarSize = QSize(80, 80),
        QGraphicsItem* parent = nullptr);
    ~CharacterItem();

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    void updateData();
    void refreshIntent(GameController* gc); 

signals:
    void cardDropped(Card* card);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void dragEnterEvent(QGraphicsSceneDragDropEvent* event) override;
    void dropEvent(QGraphicsSceneDragDropEvent* event) override;

private:
    void updateIntentIcons(GameController* gc = nullptr);

    Entity* m_entity;
    QString m_name;
    QPixmap m_avatar;
    QSize m_avatarSize;
    QPixmap m_weakIcon, m_vulnerableIcon, m_strengthIcon;

    QVector<QGraphicsPixmapItem*> m_intentIcons;
    QVector<QGraphicsSimpleTextItem*> m_intentTexts;
    bool m_isEnemy;
};

#endif