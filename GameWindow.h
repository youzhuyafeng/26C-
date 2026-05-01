#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QVector>
#include <QGraphicsPixmapItem>
#include <QGraphicsSimpleTextItem>
#include <QObject>   // 重要
#include "Card.h"

class GameController;
class CharacterItem;
class CardItem;

class EndTurnButton : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT   // 必须加
public:
    explicit EndTurnButton(const QPixmap& pixmap, QGraphicsItem* parent = nullptr);
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

signals:
    void clicked();
};

class GameWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit GameWindow(QWidget* parent = nullptr);
    ~GameWindow();
    void setDragging(bool dragging) { m_isDragging = dragging; }
    void updateUI();
    GameController* getController() { return m_controller; }

private slots:
    void onCardDroppedOnEnemy(Card* card);
    void onGameStateChanged();
    void onEndTurnClicked();

private:
    void setupScene();
    void refreshHand();

    bool m_isDragging;
    GameController* m_controller;
    QGraphicsView* m_view;
    QGraphicsScene* m_scene;
    CharacterItem* m_playerItem;
    CharacterItem* m_enemyItem;
    QVector<CardItem*> m_handItems;
    int m_currentEnergy;

    QGraphicsPixmapItem* m_energyIcon;
    QGraphicsSimpleTextItem* m_energyText;
    EndTurnButton* m_endTurnBtn;
};

#endif // GAMEWINDOW_H