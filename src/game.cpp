#include "game.h"
#include <QDebug>
#include <cmath>

Game::Game(Data* data) : gameData(data) {}

bool Game::handleInteraction(int x, int y, int layer, HeroData* hero, QString& message) {
    if (!gameData || !hero) return false;

    // 直接获取地图块上的 Entity ID
    // 这样做是为了处理像楼梯这样可能没有对应 Entity 对象定义的特殊物体
    QString entityId = gameData->map.getFloor(layer).getBlock(x, y).entityId;

    // 空气可以直接通过
    if (entityId == "air" || entityId.isEmpty()) return true;

    // 优先处理楼梯（基于 ID 字符串）
    if (entityId.startsWith("stair")) {
        return interactWithStair(entityId, layer, message);
    }

    // 获取实体对象进行类型判断
    std::shared_ptr<Entity> entity = gameData->getEntity(entityId);
    
    // 如果没有实体定义且不是楼梯，默认视为可通过（或者可以视情况改为不可通过）
    if (!entity) return true;
    
    // 墙壁阻塞
    if (entity->type == "WALL") return false;

    // 根据类型分发处理
    bool success = false;
    if (entity->type == "MONSTER") {
        Monster* monster = dynamic_cast<Monster*>(entity.get());
        if (monster) {
            success = interactWithMonster(monster, hero, message);
        }
    } else if (entity->type == "ITEM") {
        Item* item = dynamic_cast<Item*>(entity.get());
        if (item) {
            success = interactWithItem(item, hero, message);
        }
    } else if (entity->type == "DOOR") {
        Door* door = dynamic_cast<Door*>(entity.get());
        if (door) {
            success = interactWithDoor(door, hero, message);
        }
    } else if (entity->type == "NPC" || entity->type == "MERCHANT") {
        message = "对话功能暂未开放";
        return false; // NPC 和商人不可穿过
    } else {
        // 其他类型默认可通过
        return true;
    }

    if (success) {
        // 交互成功后，将该位置变为空气
        changeBuffer.push_back({x, y, layer, "air"});
    }

    return success;
}

void Game::applyMapChanges() {
    if (!gameData) return;

    for (const auto& change : changeBuffer) {
        gameData->map.getFloor(change.layer).getBlock(change.x, change.y).entityId = change.newEntityId;
    }
    changeBuffer.clear();
}

bool Game::interactWithMonster(Monster* monster, HeroData* hero, QString& message) {
    // 检查是否破防
    if (hero->atk <= monster->def) {
        message = "攻击力不足，无法破防！无法进行战斗。";
        return false; // 禁止移动和战斗
    }

    int heroDamage = hero->atk - monster->def;
    int monsterDamage = monster->atk - hero->def;
    if (monsterDamage < 0) monsterDamage = 0;

    // 回合制伤害计算
    int turns = (monster->hp + heroDamage - 1) / heroDamage;
    int totalDamage = (turns - 1) * monsterDamage;
    if (totalDamage < 0) totalDamage = 0;

    if (hero->hp > totalDamage) {
        hero->hp -= totalDamage;
        hero->gold += monster->gold;
        message = QString("战斗胜利！损失%1点生命，获得%2金币").arg(totalDamage).arg(monster->gold);
        return true;
    } else {
        message = QString("打不过！预计损失%1点生命").arg(totalDamage);
        return false;
    }
}

bool Game::interactWithItem(Item* item, HeroData* hero, QString& message) {
    if (item->id.contains("key_yellow")) {
        hero->yellow_key++;
        message = "获得黄钥匙";
    } else if (item->id.contains("key_blue")) {
        hero->blue_key++;
        message = "获得蓝钥匙";
    } else if (item->id.contains("key_red")) {
        hero->red_key++;
        message = "获得红钥匙";
    } else if (item->id.contains("potion_red")) {
        int val = 200; 
        hero->hp += val;
        message = QString("获得红药水，生命+%1").arg(val);
    } else if (item->id.contains("potion_blue")) {
        int val = 500;
        hero->hp += val;
        message = QString("获得蓝药水，生命+%1").arg(val);
    } else if (item->id.contains("gem_red")) {
        int val = 3;
        hero->atk += val;
        message = QString("获得红宝石，攻击+%1").arg(val);
    } else if (item->id.contains("gem_blue")) {
        int val = 3;
        hero->def += val;
        message = QString("获得蓝宝石，防御+%1").arg(val);
    } else {
        message = "获得物品";
    }
    return true;
}

bool Game::interactWithDoor(Door* door, HeroData* hero, QString& message) {
    if (door->id.contains("yellow") || door->id.contains("door1")) { 
        if (hero->yellow_key > 0) {
            hero->yellow_key--;
            message = "打开了黄门";
            return true;
        }
        message = "需要黄钥匙";
    } else if (door->id.contains("blue")) {
        if (hero->blue_key > 0) {
            hero->blue_key--;
            message = "打开了蓝门";
            return true;
        }
        message = "需要蓝钥匙";
    } else if (door->id.contains("red")) {
        if (hero->red_key > 0) {
            hero->red_key--;
            message = "打开了红门";
            return true;
        }
        message = "需要红钥匙";
    }
    return false;
}

bool Game::interactWithStair(const QString& entityId, int currentLayer, QString& message) {
    // 楼梯处理逻辑
    // 实际上楼梯通常是允许移动上去的，然后在移动后触发楼层切换
    // 这里我们设置 pendingLayerChange 供外部检测
    
    if (entityId.contains("up")) {
        // 上楼
        if (currentLayer + 1 < gameData->map.layers) {
            pendingLayerChange = currentLayer + 1;
            message = "上楼";
            return true;
        } else {
            message = "已经是最高层了";
            return false;
        }
    } else if (entityId.contains("down")) {
        // 下楼
        if (currentLayer - 1 >= 0) {
            pendingLayerChange = currentLayer - 1;
            message = "下楼";
            return true;
        } else {
            message = "已经是底层了";
            return false;
        }
    }
    
    return true;
}
