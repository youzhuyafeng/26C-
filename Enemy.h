#pragma once

// ==================== Enemy.h ====================
#ifndef ENEMY_H
#define ENEMY_H

#include "Entity.h"
#include "Buff.h"
#include <QVector>

struct Intent {
    int damage;          // 基础伤害（0表示无伤害）
    Buff::Type buffType; // 附加buff类型（可为 Buff::Weak / Vulnerable / Strength）
    int buffAmount;      // buff层数（0表示无）
    bool hasBuff() const { return buffAmount > 0; }
    int blockSelf;       // 为自己增加的格挡值

    // 构造函数，方便初始化
    Intent(int dmg = 0, Buff::Type type = Buff::Weak, int amt = 0, int block = 0)
        : damage(dmg), buffType(type), buffAmount(amt), blockSelf(block) {
    }
};

class Enemy : public Entity {
public:
    Enemy();

    const Intent& currentIntent() const { return m_currentIntent; }
    void determineIntent(); // 根据动作循环决定本次意图
    void executeTurn(class GameController* gc); // 执行回合动作

    // 为了扩展性，允许子类重写动作列表
    virtual void setupActionList(); // 由构造函数调用，子类可覆盖

protected:
    QVector<Intent> m_actionList;   // 动作列表
    int m_actionIndex;              // 当前动作索引（循环使用）
    Intent m_currentIntent;
};

#endif // ENEMY_H
