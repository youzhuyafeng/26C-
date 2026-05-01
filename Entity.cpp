// ==================== Entity.cpp ====================
#include "Entity.h"
#include "GameController.h"
#include <cmath>

Entity::Entity(int maxHp)
    : m_maxHp(maxHp), m_hp(maxHp), m_block(0) {
}

bool Entity::isAlive() const {
    return m_hp > 0;
}

void Entity::takeDamage(int damage) {
    if (damage <= 0) return;
    if (m_block > 0) {
        if (damage <= m_block) {
            m_block -= damage;
        }
        else {
            damage -= m_block;
            m_block = 0;
            m_hp -= damage;
        }
    }
    else {
        m_hp -= damage;
    }
    if (m_hp < 0) m_hp = 0;
}

void Entity::addBlock(int amount) {
    m_block += amount;
}

void Entity::addBuff(Buff::Type type, int amount) {
    // 查找是否已有同类buff，若有则叠加层数
    for (auto& b : m_buffs) {
        if (b.type == type) {
            b.amount += amount;
            return;
        }
    }
    m_buffs.append(Buff(type, amount));
}

void Entity::tickBuffs() {
    // 易伤和脆弱每回合层数-1，力量不减
    for (int i = m_buffs.size() - 1; i >= 0; --i) {
        Buff& b = m_buffs[i];
        if (b.type == Buff::Vulnerable || b.type == Buff::Weak) {
            b.amount -= 1;
            if (b.amount <= 0) {
                m_buffs.removeAt(i);
            }
        }
    }
}

int Entity::calculateDamage(int baseDamage, const Entity* target) const {
    double dmg = baseDamage;

    // 力量加成
    for (const auto& b : m_buffs) {
        if (b.type == Buff::Strength)
            dmg += b.amount;
    }

    // 攻击者脆弱：伤害 *0.75
    for (const auto& b : m_buffs) {
        if (b.type == Buff::Weak && b.amount > 0) {
            dmg *= 0.75;
            break;
        }
    }

    // 目标易伤：伤害 *1.5
    for (const auto& b : target->m_buffs) {
        if (b.type == Buff::Vulnerable && b.amount > 0) {
            dmg *= 1.5;
            break;
        }
    }

    return static_cast<int>(std::floor(dmg));
}