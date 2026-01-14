//====================
// 游戏画面绘制组件
//====================
#pragma once
#include <QWidget>
#include <QPainter>
#include <QKeyEvent>
#include <QTimer>
#include <memory>
#include "DataManager.h"

class GameWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GameWidget(Data* data, QWidget *parent = nullptr);
    ~GameWidget();

    // 设置当前楼层
    void setCurrentFloor(int floor);
    int getCurrentFloor() const { return currentFloor; }

    // 获取英雄数据（用于状态面板显示）
    std::shared_ptr<HeroData> getHeroData();

signals:
    // 英雄状态改变信号（用于更新状态面板）
    void heroStatusChanged();
    // 楼层改变信号
    void floorChanged(int floor);

protected:
    // 绑定绘图事件
    void paintEvent(QPaintEvent *event) override;
    // 绑定键盘事件
    void keyPressEvent(QKeyEvent *event) override;

private:
    // 绘制地图
    void drawMap(QPainter &painter);
    // 绘制实体
    void drawEntities(QPainter &painter);
    // 绘制英雄
    void drawHero(QPainter &painter);
    // 绘制单个格子
    void drawBlock(QPainter &painter, int x, int y, const Block &block);

    // 获取实体颜色（临时，后续用图片替代）
    QColor getEntityColor(const QString &entityId);
    // 获取地板颜色
    QColor getFloorColor(int floorId);

    // 尝试移动英雄
    bool tryMoveHero(int dx, int dy);
    // 处理实体交互
    bool handleEntityInteraction(int x, int y);

    // 数据管理器指针
    Data* gameData;
    // 当前楼层
    int currentFloor;
    // 格子大小（像素）
    int blockSize;
};
