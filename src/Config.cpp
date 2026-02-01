#include "Config.h"
#include <QFile>
#include <QTextStream>
#include <QCoreApplication>
#include <stdexcept>
#include <utility>

//定义配置项列表 - 仅在本文件中使用
static QStringList configKey = 
{
    //视频设置
    "windowTitle",      // 窗口标题
    "windowHeight",     // 窗口高度
    "windowWidth",      // 窗口宽度
    //地图设置
    "mapLen",           // 地图长度（列数）
    "mapWid",           // 地图宽度（行数）
    "mapLayers",        // 地图层数
    //渲染设置
    "blockSize",        // 格子大小（像素）
    "statusPanelWidth", // 状态面板宽度
};

//Config构造函数实现
Config::Config()
{
    //初始化config映射，设置默认值
    config["windowTitle"] = "魔塔";
    config["windowHeight"] = "800";
    config["windowWidth"] = "1000";
    config["mapLen"] = "12";
    config["mapWid"] = "12";
    config["mapLayers"] = "3";
    //渲染参数默认值
    config["blockSize"] = "64";         //格子大小64像素
    config["statusPanelWidth"] = "180"; //状态面板宽度180像素
}

void Config::readConfig()
{
    //QCoreApplication::applicationDirPath()返回程序可执行文件所在目录路径
    QString filePath = QCoreApplication::applicationDirPath() + "/config.txt";
    QFile file(filePath);

    //打开文件读取，同时处理读取失败
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        throw std::runtime_error("无法打开配置文件" + filePath.toStdString());
    }

    //读取文件内容到in流直到文件末尾
    QTextStream in(&file);
    while (!in.atEnd())
    {
        //读取一行
        QString line = in.readLine();
        //跳过空行和注释行
        if (line.isEmpty() || line.startsWith("#"))
            continue;
        //解析key=value格式
        QStringList parts = line.split("=");
        if (parts.size() != 2)
            throw std::runtime_error("配置文件项格式错误" + line.toStdString());

        //分隔读取
        QString key = parts[0];
        QString value = parts[1];

        //检索读取配置项
        if (configKey.contains(key))
        {
            config[key] = value;
        }
        else
        {
            throw std::runtime_error("未知的配置项" + key.toStdString());
        }
    }

    //检查是否读取到所有配置项
    for (const QString &validKey : std::as_const(configKey))    //将遍历容器设置为只读防止出现写时复制容器分离现象造成意料之外的性能问题
    {
        if (config.find(validKey) == config.end())
        {
            throw std::runtime_error("未读取到配置项" + validKey.toStdString());
        }
    }   

    //关闭文件
    file.close();
}