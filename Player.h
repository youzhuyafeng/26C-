#pragma once
#ifndef PLAYER_H
#define PLAYER_H

#include "Entity.h"

class Player : public Entity {
public:
    Player();
    int energy() const { return m_energy; }
    int maxEnergy() const { return m_maxEnergy; }
    void setEnergy(int e) { m_energy = e; }
    void resetEnergy() { m_energy = m_maxEnergy; }

private:
    int m_energy;
    int m_maxEnergy;
};

#endif
