#pragma once
#include <vector>
#include <memory>
#include <QString>
#include "DataManager.h"
#include "Entity.h"

// 地图变更结构体
struct MapChange {
    int x;
    int y;
    int layer;
    QString newEntityId;
};

class Game {
public:
    Game(Data* data);
    
    // 处理当前层(x, y)位置的交互
    // 返回 true 表示允许移动/交互成功
    // 返回 false 表示移动被阻止（如撞墙、打不过怪物、没钥匙）
    bool handleInteraction(int x, int y, int layer, HeroData* hero, QString& message);

    // 将缓冲区中的地图变更应用到游戏数据中
    void applyMapChanges();

    // 获取待处理的楼层切换请求（-1表示无请求）
    int getPendingLayerChange() const { return pendingLayerChange; }
    // 清除楼层切换请求
    void clearPendingLayerChange() { pendingLayerChange = -1; }

private:
    Data* gameData;
    std::vector<MapChange> changeBuffer;
    int pendingLayerChange = -1; // 存储即将切换到的楼层索引

    // 特定实体的交互辅助函数
    bool interactWithMonster(Monster* monster, HeroData* hero, QString& message);
    bool interactWithItem(Item* item, HeroData* hero, QString& message);
    bool interactWithDoor(Door* door, HeroData* hero, QString& message);
    bool interactWithStair(const QString& entityId, int currentLayer, QString& message);
};
