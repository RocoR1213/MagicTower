#include "mainwindow.h"
#include "Config.h"
#include "DataManager.h"
#include <QApplication>
#include <QDebug>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    try {
        //创建一个config对象，并读取config.txt文件中的数据
        Config config;
        config.readConfig();

        //根据配置创建数据管理类
        Data data(config.getInt("mapLen"), config.getInt("mapWid"), config.getInt("mapLayers"));
        
        // 创建主窗口并传入数据和配置
        MainWindow w(&data, &config);
        w.show();

        return a.exec();
    }
    catch (const std::exception& e) {
        QMessageBox::critical(nullptr, "错误 ", QString::fromStdString(e.what()));
        return -1;
    }
}
