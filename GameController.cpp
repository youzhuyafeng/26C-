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
    // m_hand 中的牌也在 pile 中，不要重复释放
}

void GameController::startGame() {
    shuffleDrawPile();
    startPlayerTurn();
    emit stateChanged();
}

void GameController::initDeck() {
    // 战士初始卡组：5打击 4防御 1重击
    for (int i = 0; i < 5; ++i) m_drawPile.append(new StrikeCard());
    for (int i = 0; i < 4; ++i) m_drawPile.append(new DefendCard());
    m_drawPile.append(new BashCard());
}

void GameController::shuffleDrawPile() {
    // 使用随机引擎洗牌
    auto rng = std::default_random_engine(std::random_device{}());
    std::shuffle(m_drawPile.begin(), m_drawPile.end(), rng);
}

void GameController::drawCards(int count) {
    for (int i = 0; i < count; ++i) {
        if (m_drawPile.isEmpty()) {
            // 弃牌堆洗入抽牌堆
            if (m_discardPile.isEmpty()) break; // 无牌可抽
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

    // 从手牌移除
    m_hand.removeAt(handIndex);
    m_discardPile.append(card);
    m_player->setEnergy(m_player->energy() - card->cost());

    // 执行卡牌
    card->play(this);

    emit stateChanged();
    checkGameOver();
}

void GameController::endPlayerTurn() {
    if (m_gameOver) return;
    enemyTurn();
    if (!m_gameOver) {
        // 回合结束，tick buffs
        m_player->tickBuffs();
        m_enemy->tickBuffs();

        startPlayerTurn();
        emit stateChanged();
    }
    emit stateChanged();
}

void GameController::enemyTurn() {
    // 注意：不要清除怪物的护甲（保留）
    // 意图已经在玩家回合开始时确定，直接执行即可
    m_enemy->executeTurn(this);
    checkGameOver();
}
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

// ---------- 伤害与Buff接口 ----------
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

void GameController::startPlayerTurn() {
    ++m_turn;
    m_player->resetEnergy();
    m_player->clearBlock();          // 玩家回合开始时清除自己的格挡
    discardHand();
    drawCards(5);

    // 提前确定敌人的下一次意图，玩家回合开始时就能看到
    m_enemy->determineIntent();

    emit stateChanged();
}

int GameController::findCardIndex(Card* card) const
{
    for (int i = 0; i < m_hand.size(); ++i) {
        if (m_hand[i] == card) return i;
    }
    return -1;
}

void GameController::playCard(Card* card)
{
    int index = findCardIndex(card);
    if (index != -1) {
        playCard(index); // 复用原有逻辑
    }
}

