#include "mainwindow.h"
#include "GameWidget.h"
#include <QFrame>
#include <QApplication>

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
    setWindowTitle(gameConfig->config["windowTitle"]);
    
    QPixmap iconPixmap(":/images/animates.png");
    if (!iconPixmap.isNull()) {
        QPixmap icon = iconPixmap.copy(0, 10 * 32, 32, 32);
        setWindowIcon(icon);
    }
    
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    
    QWidget* statusPanel = createStatusPanel();
    mainLayout->addWidget(statusPanel);
    statusPanel->setFixedWidth(gameConfig->getInt("statusPanelWidth"));
    
    gameWidget = new GameWidget(gameData, gameConfig, this);
    mainLayout->addWidget(gameWidget);
    
    connect(gameWidget, &GameWidget::heroStatusChanged, 
            this, &MainWindow::updateStatusPanel);
    connect(gameWidget, &GameWidget::floorChanged, 
            this, &MainWindow::onFloorChanged);
    
    setStyleSheet("QMainWindow { background-color: #2d2d2d; }");
    
    // 使用config配置设置窗口大小
    int windowWidth = gameConfig->getInt("windowWidth");
    int windowHeight = gameConfig->getInt("windowHeight");
    
    adjustSize();
    QSize currentSize = size();
    // 固定窗口大小，使用config配置，但不小于当前大小
    int finalWidth = qMax(currentSize.width(), windowWidth / 2); // 移除右侧面板后，宽度减半
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
    
    layout->addSpacing(10);
    
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
    
    return panel;
}

void MainWindow::updateStatusPanel()
{
    auto hero = gameWidget->getHeroData();
    if (!hero) return;
    
    hpLabel->setText(QString("HP: %1").arg(hero->hp));
    atkLabel->setText(QString("攻击: %1").arg(hero->atk));
    defLabel->setText(QString("防御: %1").arg(hero->def));
    yellowKeyLabel->setText(QString("黄钥匙: %1").arg(hero->yellow_key));
    blueKeyLabel->setText(QString("蓝钥匙: %1").arg(hero->blue_key));
    redKeyLabel->setText(QString("红钥匙: %1").arg(hero->red_key));
}

void MainWindow::onFloorChanged(int floor)
{
    floorLabel->setText(QString("楼层: %1F").arg(floor + 1));
}
