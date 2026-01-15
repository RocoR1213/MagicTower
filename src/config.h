//====================
//游戏配置管理
//====================
#pragma once
#include <QString>
#include <QStringList>
#include <QMap>
//Config类用于读取和存储游戏设置
class Config
{
public:
    Config();
    void readConfig();
    QMap<QString, QString> config;
    
    //重载[]运算符，方便访问配置项
    QString& operator[](const QString& key)
    {
        return config[key];
    }
    
    int getInt(const QString& key) const
    {
        return config.value(key).toInt();
    }
    
    // 获取渲染参数的便捷方法
    int getBlockSize() const { return getInt("blockSize"); }
    int getStatusPanelWidth() const { return getInt("statusPanelWidth"); }
    bool getDrawGridBorder() const { return config.value("drawGridBorder") == "1"; }
};
