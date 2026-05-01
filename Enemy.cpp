// ==================== Enemy.cpp ====================
#include "Enemy.h"
#include "GameController.h"

Enemy::Enemy() : Entity(42), m_actionIndex(0) {
    setupActionList();
    // 不在构造函数中调用 determineIntent，第一次意图将在第一次玩家回合开始时设置
}

void Enemy::setupActionList() {
    // 定义三个动作，按顺序循环
    // 动作1: 造成12点伤害
    m_actionList.append(Intent(12, Buff::Weak, 0, 0));
    // 动作2: 造成6点伤害，为自身提供5护甲
    m_actionList.append(Intent(6, Buff::Weak, 0, 5));
    // 动作3: 为自己提供2力量
    m_actionList.append(Intent(0, Buff::Strength, 2, 0));
}

void Enemy::determineIntent() {
    if (m_actionList.isEmpty()) {
        m_currentIntent = Intent();
        return;
    }
    // 顺序循环使用动作
    m_currentIntent = m_actionList[m_actionIndex];
    m_actionIndex = (m_actionIndex + 1) % m_actionList.size();
}

void Enemy::executeTurn(GameController* gc) {
    // 1. 造成伤害（如果有）
    if (m_currentIntent.damage > 0) {
        gc->dealDamageToPlayer(m_currentIntent.damage);
    }

    // 2. 为自己添加格挡（在攻击之后获得护甲）
    if (m_currentIntent.blockSelf > 0) {
        addBlock(m_currentIntent.blockSelf);
    }

    // 3. 附加buff（根据类型决定目标）
    if (m_currentIntent.hasBuff()) {
        if (m_currentIntent.buffType == Buff::Strength) {
            // 力量增益施加给自己
            addBuff(Buff::Strength, m_currentIntent.buffAmount);
        }
        else {
            // 脆弱/易伤等负面buff施加给玩家
            gc->applyBuffToPlayer(m_currentIntent.buffType, m_currentIntent.buffAmount);
        }
    }
}