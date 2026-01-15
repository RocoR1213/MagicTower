#include "GameWidget.h"
#include <QDebug>

GameWidget::GameWidget(Data* data, Config* config, QWidget *parent)
    : QWidget(parent)
    , gameData(data)
    , gameConfig(config)
{
    // 从配置读取渲染参数
    blockSize = config->getBlockSize();
    drawGridBorder = config->getDrawGridBorder();
    
    // 加载图片资源
    imageManager.loadResources();
    
    // 创建游戏逻辑处理器
    game = new Game(data, this);
    
    // 连接游戏逻辑信号
    connect(game, &Game::heroStatusChanged, this, &GameWidget::heroStatusChanged);
    connect(game, &Game::floorChanged, this, &GameWidget::floorChanged);
    connect(game, &Game::mapUpdated, this, &GameWidget::onMapUpdated);
    
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
    return game->getHeroData();
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
    if (!floorPixmap.isNull()) {
        // 使用图片绘制地板，缩放到格子大小
        painter.drawPixmap(targetRect, floorPixmap);
    } else {
        // 回退：使用颜色填充
        QColor floorColor = getFloorColor(block.floorId);
        painter.fillRect(targetRect, floorColor);
    }
    
    // 绘制实体（填充整格，无边距）
    if (!block.entityId.isEmpty() && block.entityId != "air") {
        QPixmap entityPixmap = imageManager.getEntityImage(block.entityId);
        if (!entityPixmap.isNull()) {
            // 使用图片绘制实体，缩放到格子大小
            painter.drawPixmap(targetRect, entityPixmap);
        } else {
            // 回退：使用颜色填充整格
            QColor entityColor = getEntityColor(block.entityId);
            painter.fillRect(targetRect, entityColor);
            
            // 在格子中央显示实体id的首字母（临时标识）
            painter.setPen(Qt::white);
            painter.setFont(QFont("Arial", blockSize / 4, QFont::Bold));
            QString label = block.entityId.left(1).toUpper();
            painter.drawText(targetRect, Qt::AlignCenter, label);
        }
    }
    
    // 根据配置决定是否绘制格子边框
    if (drawGridBorder) {
        painter.setPen(QColor(50, 50, 50));
        painter.drawRect(targetRect);
    }
}

void GameWidget::drawHero(QPainter &painter)
{
    auto hero = getHeroData();
    if (!hero) return;
    
    int px = hero->posx * blockSize;
    int py = hero->posy * blockSize;
    QRect targetRect(px, py, blockSize, blockSize);
    
    // 尝试使用图片绘制英雄
    QPixmap heroPixmap = imageManager.getHeroImage(hero->face, 0);
    if (!heroPixmap.isNull()) {
        painter.drawPixmap(targetRect, heroPixmap);
    } else {
        // 回退：使用蓝色圆形表示英雄
        int margin = blockSize / 8;
        painter.setBrush(QColor(0, 100, 255));
        painter.setPen(QPen(Qt::white, 2));
        painter.drawEllipse(px + margin, py + margin, 
                           blockSize - 2*margin, blockSize - 2*margin);
        
        // 根据朝向绘制一个小三角形表示方向
        painter.setBrush(Qt::white);
        int cx = px + blockSize / 2;
        int cy = py + blockSize / 2;
        int arrowSize = blockSize / 6;
        
        QPolygon arrow;
        switch (hero->face) {
            case 0:  // 左
                arrow << QPoint(cx - arrowSize, cy)
                      << QPoint(cx, cy - arrowSize/2)
                      << QPoint(cx, cy + arrowSize/2);
                break;
            case 1:  // 上
                arrow << QPoint(cx, cy - arrowSize)
                      << QPoint(cx - arrowSize/2, cy)
                      << QPoint(cx + arrowSize/2, cy);
                break;
            case 2:  // 右
                arrow << QPoint(cx + arrowSize, cy)
                      << QPoint(cx, cy - arrowSize/2)
                      << QPoint(cx, cy + arrowSize/2);
                break;
            case 3:  // 下
                arrow << QPoint(cx, cy + arrowSize)
                      << QPoint(cx - arrowSize/2, cy)
                      << QPoint(cx + arrowSize/2, cy);
                break;
        }
        painter.drawPolygon(arrow);
    }
}

QColor GameWidget::getEntityColor(const QString &entityId)
{
    // 临时颜色映射，后续替换为图片
    if (entityId == "wall") return QColor(100, 100, 100);      // 墙壁 - 灰色
    if (entityId == "air") return Qt::transparent;              // 空气 - 透明
    if (entityId.startsWith("door_yellow")) return QColor(255, 215, 0);  // 黄门 - 金色
    if (entityId.startsWith("door_blue")) return QColor(65, 105, 225);   // 蓝门 - 皇家蓝
    if (entityId.startsWith("door_red")) return QColor(220, 20, 60);     // 红门 - 深红
    if (entityId.startsWith("door")) return QColor(139, 69, 19);         // 其他门 - 棕色
    if (entityId.startsWith("monster")) return QColor(255, 0, 0);        // 怪物 - 红色
    if (entityId.startsWith("key_yellow")) return QColor(255, 255, 0);   // 黄钥匙 - 黄色
    if (entityId.startsWith("key_blue")) return QColor(0, 191, 255);     // 蓝钥匙 - 深天蓝
    if (entityId.startsWith("key_red")) return QColor(255, 99, 71);      // 红钥匙 - 番茄红
    if (entityId.startsWith("item")) return QColor(255, 215, 0);         // 物品 - 金色
    if (entityId.startsWith("potion_red")) return QColor(255, 0, 0);     // 红药水 - 红色
    if (entityId.startsWith("potion_blue")) return QColor(0, 0, 255);    // 蓝药水 - 蓝色
    if (entityId.startsWith("gem_red")) return QColor(255, 0, 128);      // 红宝石 - 玫红
    if (entityId.startsWith("gem_blue")) return QColor(0, 128, 255);     // 蓝宝石 - 天蓝
    if (entityId.startsWith("npc")) return QColor(0, 255, 0);            // NPC - 绿色
    if (entityId.startsWith("merchant")) return QColor(255, 0, 255);     // 商人 - 紫色
    if (entityId.startsWith("stair_up")) return QColor(144, 238, 144);   // 上楼梯 - 浅绿
    if (entityId.startsWith("stair_down")) return QColor(255, 182, 193); // 下楼梯 - 浅粉
    
    return QColor(128, 128, 128);  // 默认灰色
}

QColor GameWidget::getFloorColor(int floorId)
{
    // 临时地板颜色
    switch (floorId) {
        case 0: return QColor(60, 60, 80);     // 默认地板 - 深蓝灰
        case 1: return QColor(50, 80, 50);     // 草地 - 深绿
        case 2: return QColor(80, 60, 40);     // 泥土 - 棕色
        default: return QColor(60, 60, 80);
    }
}

InputAction GameWidget::keyToAction(int key)
{
    switch (key) {
        case Qt::Key_Left:
        case Qt::Key_A:
            return InputAction::MoveLeft;
        case Qt::Key_Up:
        case Qt::Key_W:
            return InputAction::MoveUp;
        case Qt::Key_Right:
        case Qt::Key_D:
            return InputAction::MoveRight;
        case Qt::Key_Down:
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

void GameWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        int x = event->pos().x() / blockSize;
        int y = event->pos().y() / blockSize;
        
        if (x >= 0 && x < gameData->map.len && y >= 0 && y < gameData->map.wid) {
            Floor& floor = gameData->map.getFloor(game->getCurrentFloor());
            Block& block = floor.getBlock(x, y);
            
            if (!block.entityId.isEmpty() && block.entityId != "air") {
                auto entity = gameData->getEntity(block.entityId);
                if (entity && entity->type == "MONSTER") {
                    emit monsterClicked(block.entityId);
                }
            }
        }
    }
    QWidget::mousePressEvent(event);
}
