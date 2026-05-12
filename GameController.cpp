// ==================== GameController.cpp ====================
#include "GameController.h"
#include "GameWindow.h"
#include <algorithm>
#include <random>

GameController::GameController(GameWindow* view)
    : QObject(view), m_view(view), m_turn(0), m_gameOver(false)
{
    m_player = new Player();
    m_enemy = new Enemy();
    initDeck();
}

GameController::~GameController() {
    delete m_player;
    delete m_enemy;
    qDeleteAll(m_drawPile);
    qDeleteAll(m_discardPile);
}

void GameController::startGame() {
    shuffleDrawPile();
    startPlayerTurn();
    emit stateChanged();
}

void GameController::initDeck() {
    for (int i = 0; i < 5; ++i) m_drawPile.append(new StrikeCard());
    for (int i = 0; i < 4; ++i) m_drawPile.append(new DefendCard());
    m_drawPile.append(new BashCard());
}

void GameController::shuffleDrawPile() {
    auto rng = std::default_random_engine(std::random_device{}());
    std::shuffle(m_drawPile.begin(), m_drawPile.end(), rng);
}

void GameController::drawCards(int count) {
    for (int i = 0; i < count; ++i) {
        if (m_drawPile.isEmpty()) {
            if (m_discardPile.isEmpty()) break;
            m_drawPile = m_discardPile;
            m_discardPile.clear();
            shuffleDrawPile();
        }
        if (!m_drawPile.isEmpty()) {
            Card* card = m_drawPile.takeLast();
            m_hand.append(card);
        }
    }
}

void GameController::discardHand() {
    m_discardPile.append(m_hand);
    m_hand.clear();
}

void GameController::playCard(int handIndex) {
    if (m_gameOver) return;
    if (handIndex < 0 || handIndex >= m_hand.size()) return;

    Card* card = m_hand[handIndex];
    if (card->cost() > m_player->energy()) return;

    m_hand.removeAt(handIndex);
    m_discardPile.append(card);
    m_player->setEnergy(m_player->energy() - card->cost());

    card->play(this);

    emit stateChanged();
    checkGameOver();
}

void GameController::endPlayerTurn() {
    if (m_gameOver) return;

    m_player->tickBuffs();
    m_player->clearBlock();

    enemyTurn();

    m_enemy->tickBuffs();

    if (!m_gameOver) {
        startPlayerTurn();
        emit stateChanged();
    }
}

void GameController::enemyTurn() {
    m_enemy->executeTurn(this);
    checkGameOver();
}

void GameController::startPlayerTurn() {
    ++m_turn;
    m_player->resetEnergy();       // 重置能量
    discardHand();                 // 弃掉手牌
    drawCards(5);                  // 抽 5 张牌
    m_enemy->determineIntent();    // 确定敌人本回合意图
    emit stateChanged();
}
// ====================================================================

void GameController::checkGameOver() {
    if (!m_player->isAlive()) {
        m_gameOver = true;
        emit stateChanged();
    }
    else if (!m_enemy->isAlive()) {
        m_gameOver = true;
        emit stateChanged();
    }
}

void GameController::dealDamageToEnemy(int baseDamage) {
    int dmg = m_player->calculateDamage(baseDamage, m_enemy);
    m_enemy->takeDamage(dmg);
}

void GameController::dealDamageToPlayer(int baseDamage) {
    int dmg = m_enemy->calculateDamage(baseDamage, m_player);
    m_player->takeDamage(dmg);
}

void GameController::addBlockToPlayer(int amount) {
    m_player->addBlock(amount);
}

void GameController::applyBuffToEnemy(Buff::Type type, int amount) {
    m_enemy->addBuff(type, amount);
}

void GameController::applyBuffToPlayer(Buff::Type type, int amount) {
    m_player->addBuff(type, amount);
}

int GameController::findCardIndex(Card* card) const {
    for (int i = 0; i < m_hand.size(); ++i) {
        if (m_hand[i] == card) return i;
    }
    return -1;
}

void GameController::playCard(Card* card) {
    int index = findCardIndex(card);
    if (index != -1) playCard(index);
}

