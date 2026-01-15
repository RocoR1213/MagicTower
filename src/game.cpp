#include "game.h"
#include <QDebug>

Game::Game(Data* data, QObject *parent)
    : QObject(parent)
    , gameData(data)
    , currentFloor(0)
{
}

Game::~Game()
{
}

void Game::setCurrentFloor(int floor)
{
    if (floor >= 0 && floor < gameData->map.layers) {
        currentFloor = floor;
        emit floorChanged(floor);
    }
}

std::shared_ptr<HeroData> Game::getHeroData()
{
    auto entity = gameData->getEntity("hero");
    return std::dynamic_pointer_cast<HeroData>(entity);
}

bool Game::handleInput(InputAction action)
{
    auto hero = getHeroData();
    if (!hero) return false;
    
    int dx = 0, dy = 0;
    int newFace = hero->face;
    
    switch (action) {
        case InputAction::MoveLeft:
            dx = -1;
            newFace = static_cast<int>(Direction::Left);
            break;
        case InputAction::MoveUp:
            dy = -1;
            newFace = static_cast<int>(Direction::Up);
            break;
        case InputAction::MoveRight:
            dx = 1;
            newFace = static_cast<int>(Direction::Right);
            break;
        case InputAction::MoveDown:
            dy = 1;
            newFace = static_cast<int>(Direction::Down);
            break;
        case InputAction::None:
        default:
            return false;
    }
    
    // 更新朝向
    hero->face = newFace;
    
    // 尝试移动
    bool moved = tryMoveHero(dx, dy);
    if (moved) {
        emit heroStatusChanged();
        emit mapUpdated();
    }
    
    return true; // 即使没有移动也返回true表示处理了输入（因为朝向可能改变了）
}

bool Game::tryMoveHero(int dx, int dy)
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

bool Game::handleEntityInteraction(int x, int y)
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
