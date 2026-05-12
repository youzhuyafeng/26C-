#pragma once
#ifndef CARDITEM_H
#define CARDITEM_H

#include <QGraphicsObject>
#include <QPainter>
#include "Card.h"

class CardItem : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit CardItem(Card* card, QGraphicsItem* parent = nullptr);
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    Card* getCard() const { return m_card; }

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

private:
    Card* m_card;
    QPointF m_dragStartPos;
    bool m_dragging;
    QPixmap m_cardPixmap;
};

#endif