//====================
//游戏逻辑处理
//====================
#pragma once
#include <QObject>
#include <QString>
#include <QPoint>
#include "DataManager.h"

// 定义输入动作枚举
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
    

    
    // 获取游戏数据
    Data* getGameData() const { return gameData; }

signals:
    // 英雄状态改变信号
    void heroStatusChanged();
    // 楼层改变信号
    void floorChanged(int floor);
    // 地图更新信号（实体被移除等）
    void mapUpdated();
    // 消息信号
    void messageLogged(const QString& message);
    // 游戏结束信号
    void gameOver();
    // 游戏胜利信号
    void gameSuccess();

private:
    // 方向常量 (对应 HeroData.face)
    static const int DIR_LEFT = 0;
    static const int DIR_UP = 1;
    static const int DIR_RIGHT = 2;
    static const int DIR_DOWN = 3;

    // 处理移动逻辑
    bool processMove(int dx, int dy);
    
    // 特定实体类型的处理函数
    bool handleDoorInteraction(int x, int y, const QString& entityId);
    bool handleItemInteraction(int x, int y, const QString& entityId);
    bool handleMonsterInteraction(int x, int y, const QString& entityId);
    bool handleStairInteraction(int x, int y, const QString& entityId);
    
    // 辅助函数：在指定层寻找特定类型的实体坐标
    QPoint findEntityPos(int layer, const QString& targetIdPart);

    // 数据管理器指针
    Data* gameData;
    // 当前楼层
    int currentFloor;
};