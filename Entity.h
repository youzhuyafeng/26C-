#pragma once
// ==================== Entity.h ====================
#ifndef ENTITY_H
#define ENTITY_H

#include <QVector>
#include "Buff.h"

class Entity {
public:
    Entity(int maxHp);
    virtual ~Entity() = default;

    int hp() const { return m_hp; }
    int maxHp() const { return m_maxHp; }
    int block() const { return m_block; }
    QVector<Buff>& buffs() { return m_buffs; }
    const QVector<Buff>& buffs() const { return m_buffs; }

    bool isAlive() const;
    void takeDamage(int damage);
    void addBlock(int amount);
    void addBuff(Buff::Type type, int amount);
    void tickBuffs(); // 回合结束时减少层数并移除0层的（仅易伤/脆弱）

    // 计算本实体对目标造成的最终伤害
    int calculateDamage(int baseDamage, const Entity* target) const;
    void clearBlock() { m_block = 0; }

protected:
    int m_hp, m_maxHp;
    int m_block;
    QVector<Buff> m_buffs;
};

#endif // ENTITY_H
