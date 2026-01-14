#include "GameWidget.h"
#include <QDebug>

GameWidget::GameWidget(Data* data, QWidget *parent)
    : QWidget(parent)
    , gameData(data)
    , currentFloor(0)
    , blockSize(40)  // 每格40像素
{
    // 设置焦点策略，以便接收键盘事件
    setFocusPolicy(Qt::StrongFocus);
    
    // 根据地图大小设置组件尺寸
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

void GameWidget::setCurrentFloor(int floor)
{
    if (floor >= 0 && floor < gameData->map.layers) {
        currentFloor = floor;
        emit floorChanged(floor);
        update();  // 重绘
    }
}

std::shared_ptr<HeroData> GameWidget::getHeroData()
{
    auto entity = gameData->getEntity("hero");
    return std::dynamic_pointer_cast<HeroData>(entity);
}

void GameWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // 绘制地图（包含地板和实体）
    drawMap(painter);
    // 绘制英雄
    drawHero(painter);
}

void GameWidget::drawMap(QPainter &painter)
{
    Floor& floor = gameData->map.getFloor(currentFloor);
    
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
    
    // 绘制地板
    QColor floorColor = getFloorColor(block.floorId);
    painter.fillRect(px, py, blockSize, blockSize, floorColor);
    
    // 绘制实体
    if (!block.entityId.isEmpty() && block.entityId != "air") {
        QColor entityColor = getEntityColor(block.entityId);
        
        // 实体绘制为圆角矩形，略小于格子
        int margin = 2;
        painter.setBrush(entityColor);
        painter.setPen(Qt::black);
        painter.drawRoundedRect(px + margin, py + margin, 
                               blockSize - 2*margin, blockSize - 2*margin, 
                               5, 5);
        
        // 在格子中央显示实体id的首字母（临时标识）
        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 10, QFont::Bold));
        QString label = block.entityId.left(1).toUpper();
        painter.drawText(px, py, blockSize, blockSize, Qt::AlignCenter, label);
    }
    
    // 绘制格子边框
    painter.setPen(QColor(50, 50, 50));
    painter.drawRect(px, py, blockSize, blockSize);
}

void GameWidget::drawHero(QPainter &painter)
{
    auto hero = getHeroData();
    if (!hero) return;
    
    int px = hero->posx * blockSize;
    int py = hero->posy * blockSize;
    
    // 英雄用蓝色圆形表示
    int margin = 4;
    painter.setBrush(QColor(0, 100, 255));
    painter.setPen(QPen(Qt::white, 2));
    painter.drawEllipse(px + margin, py + margin, 
                       blockSize - 2*margin, blockSize - 2*margin);
    
    // 根据朝向绘制一个小三角形表示方向
    painter.setBrush(Qt::white);
    int cx = px + blockSize / 2;
    int cy = py + blockSize / 2;
    int arrowSize = 6;
    
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

void GameWidget::keyPressEvent(QKeyEvent *event)
{
    auto hero = getHeroData();
    if (!hero) return;
    
    int dx = 0, dy = 0;
    int newFace = hero->face;
    
    switch (event->key()) {
        case Qt::Key_Left:
        case Qt::Key_A:
            dx = -1;
            newFace = 0;
            break;
        case Qt::Key_Up:
        case Qt::Key_W:
            dy = -1;
            newFace = 1;
            break;
        case Qt::Key_Right:
        case Qt::Key_D:
            dx = 1;
            newFace = 2;
            break;
        case Qt::Key_Down:
        case Qt::Key_S:
            dy = 1;
            newFace = 3;
            break;
        default:
            QWidget::keyPressEvent(event);
            return;
    }
    
    // 更新朝向
    hero->face = newFace;
    
    // 尝试移动
    if (tryMoveHero(dx, dy)) {
        emit heroStatusChanged();
    }
    
    update();  // 重绘
}

bool GameWidget::tryMoveHero(int dx, int dy)
{
    auto hero = getHeroData();
    if (!hero) return false;
    
    int newX = hero->posx + dx;
    int newY = hero->posy + dy;
    
    // 边界检查
    if (newX < 0 || newX >= gameData->map.len ||
        newY < 0 || newY >= gameData->map.wid) {
        return false;
    }
    
    // 获取目标格子
    Floor& floor = gameData->map.getFloor(currentFloor);
    Block& targetBlock = floor.floor[newX][newY];
    
    // 检查是否可以通过
    if (targetBlock.entityId == "wall") {
        return false;  // 墙壁不可通行
    }
    
    // 处理实体交互
    if (!handleEntityInteraction(newX, newY)) {
        return false;  // 交互失败，不能移动
    }
    
    // 移动英雄
    hero->posx = newX;
    hero->posy = newY;
    
    return true;
}

bool GameWidget::handleEntityInteraction(int x, int y)
{
    Floor& floor = gameData->map.getFloor(currentFloor);
    Block& block = floor.floor[x][y];
    QString entityId = block.entityId;
    
    // 空气可以直接通过
    if (entityId == "air" || entityId.isEmpty()) {
        return true;
    }
    
    // 获取实体数据
    auto entity = gameData->getEntity(entityId);
    if (!entity) {
        return true;  // 没有实体数据，默认可通过
    }
    
    auto hero = getHeroData();
    
    // 根据实体类型处理交互
    if (entity->type == "AIR") {
        return true;
    } else if (entity->type == "WALL") {
        return false;
    } else if (entity->type == "DOOR") {
        // TODO: 检查钥匙，开门
        // 暂时直接通过
        block.entityId = "air";
        return true;
    } else if (entity->type == "ITEM") {
        // TODO: 拾取物品，增加属性
        // 暂时直接拾取
        block.entityId = "air";
        qDebug() << "拾取物品:" << entityId;
        return true;
    } else if (entity->type == "MONSTER") {
        // TODO: 战斗逻辑
        // 暂时直接击败
        block.entityId = "air";
        qDebug() << "击败怪物:" << entityId;
        return true;
    } else if (entity->type == "NPC") {
        // TODO: 对话系统
        qDebug() << "与NPC对话:" << entityId;
        return false;  // NPC不可通过
    } else if (entity->type == "MERCHANT") {
        // TODO: 商店系统
        qDebug() << "商人:" << entityId;
        return false;  // 商人不可通过
    } else {
        return true;
    }
}
