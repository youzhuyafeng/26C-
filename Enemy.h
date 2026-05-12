#pragma once

#ifndef ENEMY_H
#define ENEMY_H

#include "Entity.h"
#include "Buff.h"
#include <QVector>

struct Intent {
    int damage;         
    Buff::Type buffType; 
    int buffAmount;      
    bool hasBuff() const { return buffAmount > 0; }
    int blockSelf;       

    Intent(int dmg = 0, Buff::Type type = Buff::Weak, int amt = 0, int block = 0)
        : damage(dmg), buffType(type), buffAmount(amt), blockSelf(block) {
    }
};

class Enemy : public Entity {
public:
    Enemy();
    const Intent& currentIntent() const { return m_currentIntent; }
    void determineIntent(); 
    void executeTurn(class GameController* gc);
    virtual void setupActionList(); 

protected:
    QVector<Intent> m_actionList; 
    int m_actionIndex;   
    Intent m_currentIntent;
};

#endif
