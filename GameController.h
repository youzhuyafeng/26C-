#pragma once
#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include <QObject>
#include <QVector>
#include "Card.h"
#include "Player.h"
#include "Enemy.h"
#include "Buff.h"


class Card;
class GameWindow;

class GameController : public QObject {
    Q_OBJECT
public:
    explicit GameController(GameWindow* view);
    ~GameController();

    void startGame();

    // 获取状态用于更新界面
    Player* player() { return m_player; }
    Enemy* enemy() { return m_enemy; }
    const QVector<Card*>& handCards() const { return m_hand; }
    int drawPileCount() const { return m_drawPile.size(); }
    int discardPileCount() const { return m_discardPile.size(); }
    int turnNumber() const { return m_turn; }

    // 游戏操作
    void playCard(int handIndex);
    void endPlayerTurn();

    // 供卡牌使用的效果接口
    void dealDamageToEnemy(int baseDamage);
    void dealDamageToPlayer(int baseDamage);
    void addBlockToPlayer(int amount);
    void applyBuffToEnemy(Buff::Type type, int amount);
    void applyBuffToPlayer(Buff::Type type, int amount);
    void playCard(Card* card);   // 根据卡牌指针出牌
    int findCardIndex(Card* card) const; // 辅助查找

signals:
    void stateChanged();

private:
    void initDeck();
    void shuffleDrawPile();
    void drawCards(int count);
    void discardHand();
    void startPlayerTurn();
    void enemyTurn();
    void checkGameOver();

    GameWindow* m_view;
    Player* m_player;
    Enemy* m_enemy;
    QVector<Card*> m_drawPile;
    QVector<Card*> m_discardPile;
    QVector<Card*> m_hand;
    int m_turn;
    bool m_gameOver;
};

#endif // GAMECONTROLLER_H
