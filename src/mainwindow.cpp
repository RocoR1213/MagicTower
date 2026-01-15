#include "mainwindow.h"
#include "GameWidget.h"
#include <QFrame>

MainWindow::MainWindow(Data* data, Config* config, QWidget *parent)
    : QMainWindow(parent)
    , gameData(data)
    , gameConfig(config)
    , gameWidget(nullptr)
{
    setupUI();
    updateStatusPanel();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    // 设置窗口标题
    setWindowTitle("魔塔");
    
    // 创建中央部件
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    // 水平布局：游戏区域 + 状态面板
    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    
    // 创建游戏组件（传递配置参数）
    gameWidget = new GameWidget(gameData, gameConfig, this);
    mainLayout->addWidget(gameWidget);
    
    // 连接信号
    connect(gameWidget, &GameWidget::heroStatusChanged, 
            this, &MainWindow::updateStatusPanel);
    connect(gameWidget, &GameWidget::floorChanged, 
            this, &MainWindow::onFloorChanged);
    
    // 创建状态面板（使用配置中的宽度）
    QWidget* statusPanel = createStatusPanel();
    mainLayout->addWidget(statusPanel);
    statusPanel->setFixedWidth(gameConfig->getStatusPanelWidth());
    
    // 设置窗口背景
    setStyleSheet("QMainWindow { background-color: #2d2d2d; }");
    
    // 使用配置中的窗口大小
    int windowWidth = gameConfig->getInt("windowWidth");
    int windowHeight = gameConfig->getInt("windowHeight");
    
    // 调整窗口大小 - 优先使用配置中的大小，如果配置大小小于内容大小，则使用内容大小
    adjustSize();
    QSize currentSize = size();
    int finalWidth = qMax(currentSize.width(), windowWidth);
    int finalHeight = qMax(currentSize.height(), windowHeight);
    setFixedSize(finalWidth, finalHeight);
}

QWidget* MainWindow::createStatusPanel()
{
    QFrame* panel = new QFrame(this);
    panel->setFrameStyle(QFrame::Box | QFrame::Raised);
    panel->setStyleSheet(
        "QFrame { "
        "  background-color: #3d3d3d; "
        "  border: 2px solid #555; "
        "  border-radius: 5px; "
        "}"
        "QLabel { "
        "  color: white; "
        "  font-size: 14px; "
        "  padding: 5px; "
        "}"
    );
    // 状态面板宽度将在setupUI中通过配置设置
    
    QVBoxLayout* layout = new QVBoxLayout(panel);
    layout->setSpacing(5);
    layout->setContentsMargins(10, 10, 10, 10);
    
    // 标题
    QLabel* titleLabel = new QLabel("勇者状态", panel);
    titleLabel->setStyleSheet(
        "font-size: 18px; "
        "font-weight: bold; "
        "color: #FFD700; "
        "border-bottom: 1px solid #555; "
        "padding-bottom: 10px;"
    );
    titleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(titleLabel);
    
    // 楼层
    floorLabel = new QLabel("楼层: 1F", panel);
    floorLabel->setStyleSheet("color: #87CEEB;");
    layout->addWidget(floorLabel);
    
    layout->addSpacing(10);
    
    // 生命值
    hpLabel = new QLabel("HP: 0", panel);
    hpLabel->setStyleSheet("color: #FF6B6B;");
    layout->addWidget(hpLabel);
    
    // 攻击力
    atkLabel = new QLabel("攻击: 0", panel);
    atkLabel->setStyleSheet("color: #FFA07A;");
    layout->addWidget(atkLabel);
    
    // 防御力
    defLabel = new QLabel("防御: 0", panel);
    defLabel->setStyleSheet("color: #98D8C8;");
    layout->addWidget(defLabel);
    
    // 金币
    goldLabel = new QLabel("金币: 0", panel);
    goldLabel->setStyleSheet("color: #FFD700;");
    layout->addWidget(goldLabel);
    
    layout->addSpacing(10);
    
    // 钥匙标题
    QLabel* keyTitle = new QLabel("钥匙", panel);
    keyTitle->setStyleSheet("color: #CCC; font-weight: bold;");
    layout->addWidget(keyTitle);
    
    // 黄钥匙
    yellowKeyLabel = new QLabel("黄钥匙: 0", panel);
    yellowKeyLabel->setStyleSheet("color: #FFD700;");
    layout->addWidget(yellowKeyLabel);
    
    // 蓝钥匙
    blueKeyLabel = new QLabel("蓝钥匙: 0", panel);
    blueKeyLabel->setStyleSheet("color: #4169E1;");
    layout->addWidget(blueKeyLabel);
    
    // 红钥匙
    redKeyLabel = new QLabel("红钥匙: 0", panel);
    redKeyLabel->setStyleSheet("color: #DC143C;");
    layout->addWidget(redKeyLabel);
    
    // 添加弹性空间
    layout->addStretch();
    
    // 操作提示
    QLabel* hintLabel = new QLabel("方向键/WASD移动", panel);
    hintLabel->setStyleSheet("color: #888; font-size: 11px;");
    hintLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(hintLabel);
    
    return panel;
}

void MainWindow::updateStatusPanel()
{
    auto hero = gameWidget->getHeroData();
    if (!hero) return;
    
    hpLabel->setText(QString("HP: %1").arg(hero->hp));
    atkLabel->setText(QString("攻击: %1").arg(hero->atk));
    defLabel->setText(QString("防御: %1").arg(hero->def));
    goldLabel->setText(QString("金币: %1").arg(hero->gold));
    yellowKeyLabel->setText(QString("黄钥匙: %1").arg(hero->yellow_key));
    blueKeyLabel->setText(QString("蓝钥匙: %1").arg(hero->blue_key));
    redKeyLabel->setText(QString("红钥匙: %1").arg(hero->red_key));
}

void MainWindow::onFloorChanged(int floor)
{
    floorLabel->setText(QString("楼层: %1F").arg(floor + 1));
}
