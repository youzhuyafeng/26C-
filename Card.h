#pragma once
#ifndef CARD_H
#define CARD_H

#include <QString>

class GameController;

class Card {
public:
    Card(const QString& name, int cost, const QString& desc);
    virtual ~Card() = default;

    QString name() const { return m_name; }
    int cost() const { return m_cost; }
    QString description() const { return m_description; }

    virtual void play(GameController* gc) = 0;
    virtual QString getImagePath() const = 0; // 新增：获取卡牌图片路径

protected:
    QString m_name;
    int m_cost;
    QString m_description;
};

// 具体卡牌
class StrikeCard : public Card {
public:
    StrikeCard();
    void play(GameController* gc) override;
    QString getImagePath() const override;
};

class DefendCard : public Card {
public:
    DefendCard();
    void play(GameController* gc) override;
    QString getImagePath() const override;
};

class BashCard : public Card {
public:
    BashCard();
    void play(GameController* gc) override;
    QString getImagePath() const override;
};

#endif // CARD_H
