//====================
//主窗口控制
//====================
#pragma once
#include <QMainWindow>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "DataManager.h"
#include "Config.h"

class GameWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(Data* data, Config* config, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // 更新状态面板
    void updateStatusPanel();
    // 楼层变化
    void onFloorChanged(int floor);
private:
    // 初始化UI
    void setupUI();
    // 创建状态面板
    QWidget* createStatusPanel();

    // 数据管理器
    Data* gameData;
    // 配置管理器
    Config* gameConfig;
    // 游戏组件
    GameWidget* gameWidget;

    // 状态面板标签
    QLabel* floorLabel;
    QLabel* hpLabel;
    QLabel* atkLabel;
    QLabel* defLabel;
    QLabel* yellowKeyLabel;
    QLabel* blueKeyLabel;
    QLabel* redKeyLabel;
};
