//====================
//游戏逻辑处理
//====================
#pragma once
#include <QObject>
#include <memory>
#include "DataManager.h"

// 移动方向枚举
enum class Direction {
    Left = 0,
    Up = 1,
    Right = 2,
    Down = 3
};

// 输入动作枚举
enum class InputAction {
    None,
    MoveLeft,
    MoveUp,
    MoveRight,
    MoveDown
};

class Game : public QObject
{
    Q_OBJECT

public:
    explicit Game(Data* data, QObject *parent = nullptr);
    ~Game();
    
    // 处理输入动作
    bool handleInput(InputAction action);
    
    // 设置当前楼层
    void setCurrentFloor(int floor);
    int getCurrentFloor() const { return currentFloor; }
    
    // 获取英雄数据
    std::shared_ptr<HeroData> getHeroData();
    
    // 获取游戏数据
    Data* getGameData() const { return gameData; }

signals:
    // 英雄状态改变信号
    void heroStatusChanged();
    // 楼层改变信号
    void floorChanged(int floor);
    // 地图更新信号（实体被移除等）
    void mapUpdated();

private:
    // 尝试移动英雄
    bool tryMoveHero(int dx, int dy);
    // 处理实体交互
    bool handleEntityInteraction(int x, int y);
    // 处理楼梯交互
    bool handleStairInteraction(int x, int y);
    
    // 数据管理器指针
    Data* gameData;
    // 当前楼层
    int currentFloor;
};
