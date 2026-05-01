#include "GameWindow.h"
#include "GameController.h"
#include "CharacterItem.h"
#include "CardItem.h"
#include <QLinearGradient>
#include <QBrush>
#include <QMessageBox>
#include <QGraphicsProxyWidget>
#include <QDebug>
#include <QFont>
#include <QGraphicsSceneMouseEvent>

// ---------- EndTurnButton 实现 ----------
EndTurnButton::EndTurnButton(const QPixmap& pixmap, QGraphicsItem* parent)
    : QGraphicsPixmapItem(pixmap, parent)
{
    setAcceptHoverEvents(true);
    setCursor(Qt::PointingHandCursor);
}

void EndTurnButton::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        emit clicked();
    }
    QGraphicsPixmapItem::mousePressEvent(event);
}

// ---------- GameWindow 实现 ----------
GameWindow::GameWindow(QWidget* parent)
    : QMainWindow(parent), m_controller(new GameController(this)), m_isDragging(false),
    m_energyIcon(nullptr), m_energyText(nullptr), m_endTurnBtn(nullptr)
{
    setWindowTitle("杀戮尖塔 - 图形化版");
    resize(1200, 700);
    setupScene();
    connect(m_controller, &GameController::stateChanged, this, &GameWindow::onGameStateChanged);
    m_controller->startGame();
}

GameWindow::~GameWindow()
{
    delete m_controller;
}

void GameWindow::setupScene()
{
    m_view = new QGraphicsView(this);
    m_view->setRenderHint(QPainter::Antialiasing);
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setCentralWidget(m_view);

    m_scene = new QGraphicsScene(this);
    m_scene->setSceneRect(0, 0, 1200, 800);
    m_view->setScene(m_scene);

    // 背景图片
    QPixmap bgPixmap(":res/background.png");
    if (!bgPixmap.isNull()) {
        QPixmap scaledBg = bgPixmap.scaled(1200, 800, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        m_scene->setBackgroundBrush(QBrush(scaledBg));
    }
    else {
        QLinearGradient gradient(0, 0, 0, 800);
        gradient.setColorAt(0, QColor(30, 30, 60));
        gradient.setColorAt(1, QColor(10, 10, 20));
        m_scene->setBackgroundBrush(QBrush(gradient));
    }

    // 玩家（铁甲战士）
    m_playerItem = new CharacterItem(m_controller->player(), "铁甲战士",
        ":res/Entity/ironclad.png", QSize(180, 270));
    m_playerItem->setPos(200, 250);
    m_scene->addItem(m_playerItem);

    // 怪物（颚虫）
    m_enemyItem = new CharacterItem(m_controller->enemy(), "哈气",
        ":res/Entity/nibbit.png", QSize(200, 100));
    m_enemyItem->setPos(800, 420);
    m_scene->addItem(m_enemyItem);

    connect(m_enemyItem, &CharacterItem::cardDropped, this, &GameWindow::onCardDroppedOnEnemy);

    // ========== 新增：结束回合按钮（图片+文字） ==========
    QPixmap endBtnPixmap(":res/combat_ui/end_turn_button.png");
    if (endBtnPixmap.isNull()) {
        qWarning() << "Failed to load end button image: res/combat_ui/end_turn_button.png";
        // 备用：创建一个灰色矩形
        endBtnPixmap = QPixmap(300, 100);
        endBtnPixmap.fill(Qt::gray);
    }
    else {
        // 缩放到合适大小，例如 120x40
        endBtnPixmap = endBtnPixmap.scaled(300, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    m_endTurnBtn = new EndTurnButton(endBtnPixmap);
    // 位置：手牌右侧，X = 1000，Y = 580（与手牌同一水平线）
    m_endTurnBtn->setPos(1000, 580);
    m_scene->addItem(m_endTurnBtn);
    connect(m_endTurnBtn, &EndTurnButton::clicked, this, &GameWindow::onEndTurnClicked);

    // 在按钮图片上添加“结束回合”文字
    QGraphicsSimpleTextItem* btnText = new QGraphicsSimpleTextItem("结束回合", m_endTurnBtn);
    QFont btnFont("微软雅黑", 12, QFont::Bold);
    btnText->setFont(btnFont);
    btnText->setBrush(Qt::white);
    // 文字居中在按钮图片内
    QRectF btnRect = m_endTurnBtn->boundingRect();
    QRectF textRect = btnText->boundingRect();
    btnText->setPos((btnRect.width() - textRect.width()) / 2,
        (btnRect.height() - textRect.height()) / 2);
    btnText->setZValue(1);  // 确保文字在图片上方

    // ========== 能量显示UI ==========
    QPixmap energyPixmap(":res/otherIcon/ironclad_energy_icon.png");
    if (energyPixmap.isNull()) {
        qWarning() << "Failed to load energy icon: res/otherIcon/ironclad_energy_icon.png";
        energyPixmap = QPixmap(40, 40);
        energyPixmap.fill(Qt::darkBlue);
    }
    else {
        energyPixmap = energyPixmap.scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    m_energyIcon = new QGraphicsPixmapItem(energyPixmap);
    m_energyIcon->setPos(150, 590);
    m_scene->addItem(m_energyIcon);

    m_energyText = new QGraphicsSimpleTextItem("0/3");
    QFont font("微软雅黑", 12, QFont::Bold);
    m_energyText->setFont(font);
    m_energyText->setBrush(Qt::white);
    m_energyText->setPos(150 + 45, 590 + 10);
    m_scene->addItem(m_energyText);
}

void GameWindow::refreshHand()
{
    // 拖拽过程中也允许刷新，保证卡牌及时消失
    for (CardItem* item : m_handItems) {
        m_scene->removeItem(item);
        delete item;
    }
    m_handItems.clear();

    const QVector<Card*>& hand = m_controller->handCards();
    Player* player = m_controller->player();
    m_currentEnergy = player->energy();

    if (hand.isEmpty()) return;

    const int cardWidth = 130;
    const int spacing = 0;
    const int totalWidth = hand.size() * cardWidth + (hand.size() - 1) * spacing;
    const int startX = (1200 - totalWidth) / 2;
    const int y = 580;

    for (int i = 0; i < hand.size(); ++i) {
        CardItem* cardItem = new CardItem(hand[i]);
        int x = startX + i * (cardWidth + spacing);
        cardItem->setPos(x, y);
        m_scene->addItem(cardItem);
        m_handItems.append(cardItem);

        if (hand[i]->cost() > m_currentEnergy) {
            cardItem->setEnabled(false);
            cardItem->setOpacity(0.5);
        }
        else {
            cardItem->setEnabled(true);
            cardItem->setOpacity(1.0);
        }
    }
}

void GameWindow::updateUI()
{
    m_playerItem->updateData();
    m_enemyItem->updateData();
    refreshHand();

    // 更新能量显示
    if (m_energyText && m_controller->player()) {
        Player* player = m_controller->player();
        QString energyStr = QString("%1/%2").arg(player->energy()).arg(player->maxEnergy());
        m_energyText->setText(energyStr);
    }

    if (!m_controller->player()->isAlive()) {
        QMessageBox::information(this, "游戏结束", "你被击败了！");
        close();
    }
    else if (!m_controller->enemy()->isAlive()) {
        QMessageBox::information(this, "胜利", "你击败了敌人！");
        close();
    }
}

void GameWindow::onCardDroppedOnEnemy(Card* card)
{
    m_controller->playCard(card);
}

void GameWindow::onGameStateChanged()
{
    updateUI();
}

void GameWindow::onEndTurnClicked()
{
    m_controller->endPlayerTurn();
}

