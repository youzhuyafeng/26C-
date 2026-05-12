#include "Card.h"
#include "GameController.h"

Card::Card(const QString& name, int cost, const QString& desc)
    : m_name(name), m_cost(cost), m_description(desc) {
}

StrikeCard::StrikeCard() : Card("打击", 1, "造成6点伤害") {}
void StrikeCard::play(GameController* gc) { gc->dealDamageToEnemy(6); }
QString StrikeCard::getImagePath() const { return "res/Card/strike-ironclad.png"; }

DefendCard::DefendCard() : Card("防御", 1, "获得5点格挡") {}
void DefendCard::play(GameController* gc) { gc->addBlockToPlayer(5); }
QString DefendCard::getImagePath() const { return "res/Card/defend-ironclad.png"; }

BashCard::BashCard() : Card("重击", 2, "造成8点伤害，施加2层易伤") {}
void BashCard::play(GameController* gc) {
    gc->dealDamageToEnemy(8);
    gc->applyBuffToEnemy(Buff::Vulnerable, 2);
}
QString BashCard::getImagePath() const { return "res/Card/bash-ironclad.png"; }