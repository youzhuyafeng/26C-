#pragma once
// ==================== Buff.h ====================
#ifndef BUFF_H
#define BUFF_H

struct Buff {
    enum Type { Vulnerable, Weak, Strength };
    Type type;
    int amount; // 层数：对于易伤/脆弱表示剩余回合，对于力量表示数值

    Buff(Type t, int amt) : type(t), amount(amt) {}
};

#endif // BUFF_H
