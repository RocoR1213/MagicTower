#include "game.h"
#include <QDebug>
#include <cmath>

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



bool Game::handleInput(InputAction action)
{
    auto hero = gameData->getHeroData();
    if (!hero) return false;
    
    int dx = 0, dy = 0;
    int newFace = hero->face;
    
    switch (action) {
        case InputAction::MoveLeft:
            dx = -1;
            newFace = DIR_LEFT;
            break;
        case InputAction::MoveUp:
            dy = -1;
            newFace = DIR_UP;
            break;
        case InputAction::MoveRight:
            dx = 1;
            newFace = DIR_RIGHT;
            break;
        case InputAction::MoveDown:
            dy = 1;
            newFace = DIR_DOWN;
            break;
        case InputAction::None:
        default:
            return false;
    }
    
    // 更新朝向
    if (hero->face != newFace)
    {
        hero->face = newFace;
        emit heroStatusChanged();
    }
    
    // 处理移动
    return processMove(dx, dy);
}

bool Game::processMove(int dx, int dy)
{
    auto hero = gameData->getHeroData();
    if (!hero) return false;
    
    // 计算新位置
    int newX = hero->posx + dx;
    int newY = hero->posy + dy;
    
    // 边界检查
    if (newX < 0 || newX >= gameData->map.len ||
        newY < 0 || newY >= gameData->map.wid) {
        return false;
    }
    
    // 取得目标位置的实体
    Floor& floor = gameData->map.getFloor(currentFloor);
    Block& targetBlock = floor.getBlock(newX, newY);
    QString entityId = targetBlock.entityId;
    auto entity = gameData->getEntity(entityId);
    
    // 根据实体类型处理交互
    if (entityId == "air" || entityId.isEmpty()) {
        // 仅当交互对象是AIR时才移动勇者
        hero->posx = newX;
        hero->posy = newY;
        emit heroStatusChanged();
        return true;
    } else if (entity && entity->type == "WALL") {
        // 遇到WALL时保持位置不动
        return false;
    } else if (entity && entity->type == "DOOR") {
        // 与DOOR交互
        return handleDoorInteraction(newX, newY, entityId);
    } else if (entity && entity->type == "ITEM") {
        // 与ITEM交互
        return handleItemInteraction(newX, newY, entityId);
    } else if (entity && entity->type == "MONSTER") {
        // 与MONSTER交互
        return handleMonsterInteraction(newX, newY, entityId);
    } else if (entity && entity->type == "STAIR") {
        // 与STAIR交互
        return handleStairInteraction(newX, newY, entityId);
    } else if (entity && (entity->type == "NPC" || entity->type == "MERCHANT")) {
        // NPC和MERCHANT的交互留空
        return false;
    }
    
    return false;
}

bool Game::handleDoorInteraction(int x, int y, const QString& entityId)
{
    auto hero = gameData->getHeroData();
    if (!hero) return false;
    
    Floor& floor = gameData->map.getFloor(currentFloor);
    Block& block = floor.getBlock(x, y);
    
    // 根据门的ID类型消耗对应颜色的钥匙
    if (entityId.contains("yellow") || entityId.contains("door1")) {
        if (hero->yellow_key > 0) {
            hero->yellow_key--;
            block.entityId = "air"; // 成功开门，设置为AIR
            emit mapUpdated();
            emit heroStatusChanged();
            return true;
        }
    } else if (entityId.contains("blue")) {
        if (hero->blue_key > 0) {
            hero->blue_key--;
            block.entityId = "air"; // 成功开门，设置为AIR
            emit mapUpdated();
            emit heroStatusChanged();
            return true;
        }
    } else if (entityId.contains("red")) {
        if (hero->red_key > 0) {
            hero->red_key--;
            block.entityId = "air"; // 成功开门，设置为AIR
            emit mapUpdated();
            emit heroStatusChanged();
            return true;
        }
    }
    
    return false; // 钥匙不足，无法开门
}

bool Game::handleItemInteraction(int x, int y, const QString& entityId)
{
    auto hero = gameData->getHeroData();
    if (!hero) return false;
    
    Floor& floor = gameData->map.getFloor(currentFloor);
    Block& block = floor.getBlock(x, y);
    
    // 使用长if-else链处理每种物品ID
    if (entityId.contains("yellow_key")) {
        hero->yellow_key++;
    } else if (entityId.contains("blue_key")) {
        hero->blue_key++;
    } else if (entityId.contains("red_key")) {
        hero->red_key++;
    } else if (entityId.contains("hp_potion_1")) {
        hero->hp += 200;
    } else if (entityId.contains("hp_potion_2")) {
        hero->hp += 300;
    } else if (entityId.contains("hp_potion_3")) {
        hero->hp += 500;
    } else if (entityId.contains("atk_gem")) {
        hero->atk += 3;
    } else if (entityId.contains("def_gem")) {
        hero->def += 3;
    }
    
    // 物品被拾取后设置为AIR
    block.entityId = "air";
    emit mapUpdated();
    emit heroStatusChanged();
    return true;
}

bool Game::handleMonsterInteraction(int x, int y, const QString& entityId)
{
    auto hero = gameData->getHeroData();
    if (!hero) return false;
    
    auto entity = gameData->getEntity(entityId);
    auto monster = std::dynamic_pointer_cast<Monster>(entity);
    if (!monster) return false;
    
    // 进入战斗函数
    if (hero->atk <= monster->def) {
        return false; // 攻击力不足，无法破防
    }
    
    int heroDamage = hero->atk - monster->def;
    int monsterDamage = qMax(0, monster->atk - hero->def);
    
    int turns = (monster->hp + heroDamage - 1) / heroDamage;
    int totalDamage = (turns - 1) * monsterDamage;
    
    if (hero->hp > totalDamage) {
        //战斗胜利，勇者hp>0
        hero->hp -= totalDamage;
        hero->gold += monster->gold;
        
        //设置怪物位置为AIR
        Floor& floor = gameData->map.getFloor(currentFloor);
        Block& block = floor.getBlock(x, y);
        block.entityId = "air";
        
        emit mapUpdated();
        emit heroStatusChanged();
        return true;
    } else {
        //战斗失败，勇者hp<=0，进入gameover界面
        hero->hp = 0;
        emit gameOver();
        return false;
    }
}

bool Game::handleStairInteraction(int x, int y, const QString& entityId)
{
    auto hero = gameData->getHeroData();
    if (!hero) return false;
    
    int targetLayer = -1;
    QString targetStairId;
    
    if (entityId.contains("up")) {
        targetLayer = currentFloor + 1;
        targetStairId = "down";
    } else if (entityId.contains("down")) {
        targetLayer = currentFloor - 1;
        targetStairId = "up";
    } else {
        return false;
    }
    
    if (targetLayer >= 0 && targetLayer < gameData->map.layers) {
        setCurrentFloor(targetLayer);
        
        // 寻找目标楼层的对应楼梯
        QPoint targetPos = findEntityPos(targetLayer, targetStairId);
        if (targetPos != QPoint(-1, -1)) {
            hero->posx = targetPos.x();
            hero->posy = targetPos.y();
        } else {
            hero->posx = x;
            hero->posy = y;
        }
        
        emit mapUpdated();
        emit heroStatusChanged();
        return true;
    } else if (targetLayer >= gameData->map.layers && entityId.contains("up")) {
        // 最高楼层上楼，触发游戏胜利
        emit gameSuccess();
        return true;
    }
    
    return false;
}

QPoint Game::findEntityPos(int layer, const QString& targetIdPart)
{
    if (!gameData || layer < 0 || layer >= gameData->map.layers) 
        return QPoint(-1, -1);
    
    Floor& floor = gameData->map.getFloor(layer);
    for (int y = 0; y < gameData->map.wid; ++y) {
        for (int x = 0; x < gameData->map.len; ++x) {
            if (floor.getBlock(x, y).entityId.contains(targetIdPart)) {
                return QPoint(x, y);
            }
        }
    }
    return QPoint(-1, -1);
}
