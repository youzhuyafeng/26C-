#include "CardItem.h"
#include <QGraphicsSceneMouseEvent>
#include <QCursor>
#include <QDrag>
#include <QMimeData>
#include <QApplication>
#include <QDebug>
#include "GameWindow.h"
#include <QPointer>

CardItem::CardItem(Card* card, QGraphicsItem* parent)
    : QGraphicsObject(parent), m_card(card), m_dragging(false)
{
    setAcceptHoverEvents(true);
    setCursor(Qt::OpenHandCursor);
    // 加载卡牌图片
    QString imgPath = card->getImagePath();
    if (!m_cardPixmap.load(imgPath))
        qWarning() << "Failed to load card image:" << imgPath;
    else
        m_cardPixmap = m_cardPixmap.scaled(boundingRect().size().toSize(),
            Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

QRectF CardItem::boundingRect() const { return QRectF(0, 0, 130, 160); }

void CardItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    painter->save();
    if (!m_cardPixmap.isNull())
        // 使用 QRectF 版本
        painter->drawPixmap(QRect(0, 0, 130, 160), m_cardPixmap);
    else {
        painter->setBrush(QColor(240, 240, 220));
        painter->setPen(QPen(Qt::black, 2));
        painter->drawRoundedRect(boundingRect(), 10, 10);
    }

    painter->restore();
}

void CardItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragStartPos = event->pos();
        m_dragging = true;
        setCursor(Qt::ClosedHandCursor);
    }
    event->accept();
}

void CardItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (m_dragging && (event->pos() - m_dragStartPos).manhattanLength() > QApplication::startDragDistance()) {
        // 用 QPointer 保护 this
        QPointer<CardItem> self(this);

        GameWindow* win = qobject_cast<GameWindow*>(scene()->views().first()->window());
        if (win) win->setDragging(true);

        QDrag* drag = new QDrag(event->widget());
        QMimeData* mimeData = new QMimeData;
        mimeData->setText(QString::number(reinterpret_cast<quintptr>(m_card)));
        drag->setMimeData(mimeData);

        QPixmap pixmap(boundingRect().size().toSize());
        pixmap.fill(Qt::transparent);
        QPainter painter(&pixmap);
        paint(&painter, nullptr, nullptr);
        painter.end();
        drag->setPixmap(pixmap);
        drag->setHotSpot(event->pos().toPoint());

        drag->exec(Qt::MoveAction);

        if (win) win->setDragging(false);

        // 若对象已在拖拽过程中被删除，则直接返回
        if (self.isNull()) return;

        m_dragging = false;
        setCursor(Qt::OpenHandCursor);
    }
    event->accept();
}

void CardItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    QPointer<CardItem> self(this);
    if (m_dragging) {
        m_dragging = false;
        if (!self.isNull())
            setCursor(Qt::OpenHandCursor);
    }
    event->accept();
}