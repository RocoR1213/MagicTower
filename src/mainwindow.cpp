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
    connect(gameWidget, &GameWidget::monsterClicked, 
            this, &MainWindow::showMonsterInfo);
    
    QWidget* infoPanel = createInfoPanel();
    mainLayout->addWidget(infoPanel);
    infoPanel->setFixedWidth(gameConfig->getInt("statusPanelWidth"));
    
    setStyleSheet("QMainWindow { background-color: #2d2d2d; }");
    
    int windowWidth = gameConfig->getInt("windowWidth");
    int windowHeight = gameConfig->getInt("windowHeight");
    
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

QWidget* MainWindow::createInfoPanel()
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
    
    QVBoxLayout* layout = new QVBoxLayout(panel);
    layout->setSpacing(5);
    layout->setContentsMargins(10, 10, 10, 10);
    
    infoTitleLabel = new QLabel("信息面板", panel);
    infoTitleLabel->setStyleSheet(
        "font-size: 18px; "
        "font-weight: bold; "
        "color: #FFD700; "
        "border-bottom: 1px solid #555; "
        "padding-bottom: 10px;"
    );
    infoTitleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(infoTitleLabel);
    
    infoNameLabel = new QLabel("名称: -", panel);
    infoNameLabel->setStyleSheet("color: #87CEEB;");
    layout->addWidget(infoNameLabel);
    
    infoTypeLabel = new QLabel("类型: -", panel);
    infoTypeLabel->setStyleSheet("color: #98D8C8;");
    layout->addWidget(infoTypeLabel);
    
    layout->addSpacing(10);
    
    infoHpLabel = new QLabel("生命: -", panel);
    infoHpLabel->setStyleSheet("color: #FF6B6B;");
    layout->addWidget(infoHpLabel);
    
    infoAtkLabel = new QLabel("攻击: -", panel);
    infoAtkLabel->setStyleSheet("color: #FFA07A;");
    layout->addWidget(infoAtkLabel);
    
    infoDefLabel = new QLabel("防御: -", panel);
    infoDefLabel->setStyleSheet("color: #98D8C8;");
    layout->addWidget(infoDefLabel);
    
    infoGoldLabel = new QLabel("金币: -", panel);
    infoGoldLabel->setStyleSheet("color: #FFD700;");
    layout->addWidget(infoGoldLabel);
    
    layout->addSpacing(10);
    
    infoDamageLabel = new QLabel("HP损失: 0", panel);
    infoDamageLabel->setStyleSheet("color: #FF4500; font-weight: bold;");
    layout->addWidget(infoDamageLabel);
    
    layout->addStretch();
    
    QLabel* hintLabel = new QLabel("点击怪物查看信息", panel);
    hintLabel->setStyleSheet("color: #888; font-size: 11px;");
    hintLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(hintLabel);
    
    return panel;
}

void MainWindow::showMonsterInfo(const QString& entityId)
{
    auto entity = gameData->getEntity(entityId);
    if (!entity) {
        infoNameLabel->setText("名称: -");
        infoTypeLabel->setText("类型: -");
        infoHpLabel->setText("生命: -");
        infoAtkLabel->setText("攻击: -");
        infoDefLabel->setText("防御: -");
        infoGoldLabel->setText("金币: -");
        infoDamageLabel->setText("HP损失: 0");
        return;
    }
    
    if (entity->type == "MONSTER") {
        Monster* monster = static_cast<Monster*>(entity.get());
        if (monster) {
            infoNameLabel->setText(QString("名称: %1").arg(entityId));
            infoTypeLabel->setText("类型: 怪物");
            infoHpLabel->setText(QString("生命: %1").arg(monster->hp));
            infoAtkLabel->setText(QString("攻击: %1").arg(monster->atk));
            infoDefLabel->setText(QString("防御: %1").arg(monster->def));
            infoGoldLabel->setText(QString("金币: %1").arg(monster->gold));
            infoDamageLabel->setText("HP损失: 0");
            return;
        }
    }
    
    infoNameLabel->setText(QString("名称: %1").arg(entityId));
    infoTypeLabel->setText(QString("类型: %1").arg(entity->type));
    infoHpLabel->setText("生命: -");
    infoAtkLabel->setText("攻击: -");
    infoDefLabel->setText("防御: -");
    infoGoldLabel->setText("金币: -");
    infoDamageLabel->setText("HP损失: 0");
}
