// ==================== Enemy.cpp ====================
#include "Enemy.h"
#include "GameController.h"

Enemy::Enemy() : Entity(42), m_actionIndex(0) {
    setupActionList();
}

void Enemy::setupActionList() {
    m_actionList.append(Intent(12, Buff::Weak, 2, 0));
    m_actionList.append(Intent(6, Buff::Weak, 0, 5));
    m_actionList.append(Intent(0, Buff::Strength, 2, 0));
}

void Enemy::determineIntent() {
    if (m_actionList.isEmpty()) {
        m_currentIntent = Intent();
        return;
    }
    m_currentIntent = m_actionList[m_actionIndex];
    m_actionIndex = (m_actionIndex + 1) % m_actionList.size();
}

void Enemy::executeTurn(GameController* gc) {
    if (m_currentIntent.damage > 0) {
        gc->dealDamageToPlayer(m_currentIntent.damage);
    }

    if (m_currentIntent.blockSelf > 0) {
        addBlock(m_currentIntent.blockSelf);
    }

    if (m_currentIntent.hasBuff()) {
        if (m_currentIntent.buffType == Buff::Strength) {
            addBuff(Buff::Strength, m_currentIntent.buffAmount);
        }
        else {
            gc->applyBuffToPlayer(m_currentIntent.buffType, m_currentIntent.buffAmount);
        }
    }
}