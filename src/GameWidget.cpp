#include "GameWidget.h"
#include <QDebug>
#include <QMessageBox>
#include <QApplication>

GameWidget::GameWidget(Data* data, Config* config, QWidget *parent)
    : QWidget(parent)
    , gameData(data)
    , gameConfig(config)
{
    // 从配置读取渲染参数
    blockSize = config->getBlockSize();
    
    // 加载图片资源
    imageManager.loadResources();
    
    // 创建游戏逻辑处理器
    game = new Game(data, this);
    
    // 连接游戏逻辑信号
    connect(game, &Game::heroStatusChanged, this, &GameWidget::heroStatusChanged);
    connect(game, &Game::floorChanged, this, &GameWidget::floorChanged);
    connect(game, &Game::mapUpdated, this, &GameWidget::onMapUpdated);
    connect(game, &Game::gameOver, this, [this]() {
        // 显示游戏结束消息框
        QMessageBox::information(this, "游戏结束", "你被怪物击败了！");
        // 关闭应用程序
        QApplication::quit();
    });
    connect(game, &Game::gameSuccess, this, [this]() {
        // 获取英雄数据，显示分数（使用HP作为分数）
        auto hero = getHeroData();
        int score = hero ? hero->hp : 0;
        // 显示游戏胜利消息框
        QMessageBox::information(this, "胜利", QString("胜利：你的分数是%1").arg(score));
        // 关闭应用程序
        QApplication::quit();
    });
    
    // 设置焦点策略，以便接收键盘事件
    setFocusPolicy(Qt::StrongFocus);
    
    // 根据地图大小和格子大小设置组件尺寸
    int width = gameData->map.len * blockSize;
    int height = gameData->map.wid * blockSize;
    setFixedSize(width, height);
    
    // 设置背景色
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, Qt::black);
    setPalette(pal);
}

GameWidget::~GameWidget()
{
}

std::shared_ptr<HeroData> GameWidget::getHeroData()
{
    return game->getGameData()->getHeroData();
}

void GameWidget::onMapUpdated()
{
    update();  // 重绘
}

void GameWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    
    // 使用平滑缩放以获得更好的图片质量
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    
    // 绘制地图（包含地板和实体）
    drawMap(painter);
    // 绘制英雄
    drawHero(painter);
}

void GameWidget::drawMap(QPainter &painter)
{
    Floor& floor = gameData->map.getFloor(game->getCurrentFloor());
    
    for (int y = 0; y < gameData->map.wid; ++y) {
        for (int x = 0; x < gameData->map.len; ++x) {
            Block block = floor.getBlock(x, y);
            drawBlock(painter, x, y, block);
        }
    }
}

void GameWidget::drawBlock(QPainter &painter, int x, int y, const Block &block)
{
    int px = x * blockSize;
    int py = y * blockSize;
    QRect targetRect(px, py, blockSize, blockSize);
    
    // 绘制地板
    QPixmap floorPixmap = imageManager.getFloorImage(block.floorId);
    painter.drawPixmap(targetRect, floorPixmap);
    
    // 绘制实体（填充整格，无边距）
    if (!block.entityId.isEmpty() && block.entityId != "air") {
        QPixmap entityPixmap = imageManager.getEntityImage(block.entityId);
        painter.drawPixmap(targetRect, entityPixmap);
        
        // 如果是怪物，显示其hp，atk，def属性
        auto entity = gameData->getEntity(block.entityId);
        if (entity && entity->type == "MONSTER") {
            Monster* monster = static_cast<Monster*>(entity.get());
            if (monster) {
                // 设置字体和颜色
                painter.setPen(Qt::white);
                painter.setFont(QFont("Arial", blockSize / 6, QFont::Bold));
                
                // 右对齐显示属性，显示在右下角
                QString hpText = QString("HP:%1").arg(monster->hp);
                QString atkText = QString("ATK:%1").arg(monster->atk);
                QString defText = QString("DEF:%1").arg(monster->def);
                
                // 计算文本位置
                int margin = 2;
                int lineHeight = blockSize / 6 + margin;
                
                // 绘制文本，右对齐
                painter.drawText(px + margin, py + blockSize - 3 * lineHeight, blockSize - 2 * margin, lineHeight, Qt::AlignRight, hpText);
                painter.drawText(px + margin, py + blockSize - 2 * lineHeight, blockSize - 2 * margin, lineHeight, Qt::AlignRight, atkText);
                painter.drawText(px + margin, py + blockSize - lineHeight, blockSize - 2 * margin, lineHeight, Qt::AlignRight, defText);
            }
        }
    }
}

void GameWidget::drawHero(QPainter &painter)
{
    auto hero = getHeroData();
    if (!hero) return;
    
    int px = hero->posx * blockSize;
    int py = hero->posy * blockSize;
    QRect targetRect(px, py, blockSize, blockSize);
    
    // 绘制英雄
    QPixmap heroPixmap = imageManager.getHeroImage(hero->face, 0);
    painter.drawPixmap(targetRect, heroPixmap);
}

InputAction GameWidget::keyToAction(int key)
{
    switch (key) {
        case Qt::Key_A:
            return InputAction::MoveLeft;
        case Qt::Key_W:
            return InputAction::MoveUp;
        case Qt::Key_D:
            return InputAction::MoveRight;
        case Qt::Key_S:
            return InputAction::MoveDown;
        default:
            return InputAction::None;
    }
}

void GameWidget::keyPressEvent(QKeyEvent *event)
{
    InputAction action = keyToAction(event->key());
    
    if (action != InputAction::None) {
        game->handleInput(action);
        update();  // 重绘
    } else {
        QWidget::keyPressEvent(event);
    }
}

