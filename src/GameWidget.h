//====================
// 游戏画面绘制组件
//====================
#pragma once
#include <QWidget>
#include <QPainter>
#include <QKeyEvent>
#include <QMouseEvent>
#include <memory>
#include "DataManager.h"
#include "Config.h"
#include "game.h"
#include "ImageManager.h"

class GameWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GameWidget(Data* data, Config* config, QWidget *parent = nullptr);
    ~GameWidget();

    // 获取游戏逻辑处理器
    Game* getGame() const { return game; }
    
    // 获取英雄数据（用于状态面板显示）
    std::shared_ptr<HeroData> getHeroData();

signals:
    // 英雄状态改变信号（用于更新状态面板）
    void heroStatusChanged();
    // 楼层改变信号
    void floorChanged(int floor);
    // 怪物点击信号
    void monsterClicked(const QString& entityId);

protected:
    // 绑定绘图事件
    void paintEvent(QPaintEvent *event) override;
    // 绑定键盘事件
    void keyPressEvent(QKeyEvent *event) override;
    // 绑定鼠标点击事件
    void mousePressEvent(QMouseEvent *event) override;

private slots:
    // 响应游戏状态更新
    void onMapUpdated();

private:
    // 绘制地图
    void drawMap(QPainter &painter);
    // 绘制英雄
    void drawHero(QPainter &painter);
    // 绘制单个格子
    void drawBlock(QPainter &painter, int x, int y, const Block &block);

    // 获取实体颜色（无图片时的回退方案）
    QColor getEntityColor(const QString &entityId);
    // 获取地板颜色（无图片时的回退方案）
    QColor getFloorColor(int floorId);
    
    // 将键盘按键转换为输入动作
    InputAction keyToAction(int key);

    // 游戏逻辑处理器
    Game* game;
    // 数据管理器指针
    Data* gameData;
    // 配置管理器指针
    Config* gameConfig;
    // 图片资源管理器
    ImageManager imageManager;
    
    // 渲染参数（从配置读取）
    int blockSize;          // 格子大小（像素）
    bool drawGridBorder;    // 是否绘制网格边框
};
