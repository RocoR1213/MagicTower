#include "DataManager.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QDir>
#include <QCoreApplication>
#include <memory>

static QStringList EntityType = 
{
    "AIR",
    "HERODATA",
    "WALL",
    "DOOR",
    "ITEM",
    "MONSTER",
    "NPC",
    "MERCHANT"
};

void Data::LoadMap(int mapLen, int mapWid, int mapLayers)
{
    //遍历所有层数
    for (int layer = 0; layer < mapLayers; ++layer) 
    {
        //拼接地图文件路径，关联路径，通过操作file来操作文件
        QDir appDirPath(QCoreApplication::applicationDirPath());
        QString filePath = appDirPath.filePath(QString("gamedata/map/map%1.txt").arg(layer));
        QFile file(filePath);

        //打开文件读取，同时处理读取失败
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            throw QString("无法打开地图文件:" + filePath);

        //读取文件内容到in流
        QTextStream in(&file);
        QString line;
        int row = 0;
        //分区标记
        //0:未开始,1:entity部分,2: floor部分
        int section = 0;
        int state = 0;   //计数已读取的部分
        //一直读到文件结束
        while (!in.atEnd()) 
        {
            line = in.readLine();
            if (line.isEmpty()) continue;

            //检查类型标志
            if (line == "entity")
            {
                ++state;
                section = 1;
                row = 0;
                continue;
            } else if (line == "floor")
            {
                ++state;
                section = 2;
                row = 0;
                continue;
            }

            //entity部分
            if (section == 1)
            { 
                //分割行数据
                QStringList ids = line.split(" ", Qt::SkipEmptyParts);

                //检验数据合法性
                if (row >= mapWid)
                    throw QString("地图文件%1的entity部分行数超过配置:%2行").arg(filePath).arg(mapWid);
                if (ids.size() != mapLen)
                    throw QString("地图文件%1的entity部分第%2行列数不符合预期:%3列").arg(filePath).arg(row + 1).arg(ids.size());

                //存储entityId
                for (int col = 0; col < mapLen; ++col)
                {
                    map.map[layer].floor[row][col].entityId = ids[col];
                }
                ++row;
            }// floor部分
            else if (section == 2)
            {
                //分割行数据
                QStringList ids = line.split(" ", Qt::SkipEmptyParts);

                //检验数据合法性
                if (row >= mapWid)
                    throw QString("地图文件%1的floor部分行数超过配置:%2行").arg(filePath).arg(mapWid);
                if (ids.size() != mapLen)
                    throw QString("地图文件%1的floor部分第%2行列数不符合预期:%3列").arg(filePath).arg(row + 1).arg(ids.size());

                //存储floorId到Block中
                for (int col = 0; col < mapLen; ++col)
                {
                    map.map[layer].floor[row][col].floorId = ids[col].toInt();
                }
                ++row;
            }
            else
            {
                throw QString("地图文件%1的第%2行类型不明:%3").arg(filePath).arg(row + 1).arg(line);
            }
        }

        // 检查是否读取了完整的entity和floor部分
        if (state != 2)
            throw QString("地图文件%1的结构错误").arg(filePath);

        // 关闭文件
        file.close();
    }
}

void Data::LoadEntity()
{
    //获取应用程序目录
    QDir appDirPath(QCoreApplication::applicationDirPath());

    // 遍历所有实体类型
    for (const QString& type : EntityType)
    {
        //将实体类型转换为小写并拼接成文件名
        QString filename = type.toLower() + ".txt";
        QString filePath = appDirPath.filePath(QString("gamedata/entity/%1").arg(filename));
        QFile file(filePath);

        //打开文件读取，同时处理读取失败
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            throw QString("无法打开实体文件:" + filePath);

        //逐行读取直到文件末尾
        QTextStream in(&file);
        QString line;
        std::shared_ptr<Entity> entityObj = nullptr;
        // 逐行读取文件内容
        while (!in.atEnd())
        {
            line = in.readLine();
            //空行保存当前实体并重置暂存实体指针
            if (line.isEmpty())
            {
                if (entityObj)
                {
                    entity[entityObj->id] = entityObj;
                    entityObj = nullptr;
                }
                continue;
            }

            //解析实体标识
            if (!entityObj)
            {
                QStringList parts = line.split(" ", Qt::SkipEmptyParts);
                if (parts.size() != 2)
                    throw QString("实体文件格式错误:" + filePath + " 行:" + line);
                QString entityId = parts[0];
                QString entityType = parts[1];

                // 根据实体类型创建对象
                if (entityType == "AIR")
                    entityObj = std::make_shared<Air>();
                else if (entityType == "HERODATA")
                    entityObj = std::make_shared<HeroData>();
                else if (entityType == "WALL")
                    entityObj = std::make_shared<Wall>();
                else if (entityType == "DOOR")
                    entityObj = std::make_shared<Door>();
                else if (entityType == "ITEM")
                    entityObj = std::make_shared<Item>();
                else if (entityType == "MONSTER")
                    entityObj = std::make_shared<Monster>();
                else if (entityType == "NPC")
                    entityObj = std::make_shared<NPC>();
                else if (entityType == "MERCHANT")
                    entityObj = std::make_shared<Merchant>();
                else
                    throw QString("未知实体类型:" + entityType + " 行:" + line);
                entityObj->id = entityId;
            }
            else    //已读实体标识，解析属性
            {
                QStringList keyValue = line.split("=", Qt::SkipEmptyParts);
                if (keyValue.size() != 2)
                    throw QString("属性格式错误:" + filePath + " 行:" + line);
                QString key = keyValue[0];
                QString value = keyValue[1];
                // 根据实体类型设置属性
                if (entityObj->type == "HERODATA")
                {
                    HeroData* hero = dynamic_cast<HeroData*>(entityObj.get());
                    if (hero)
                    {
                        if (key == "posx") hero->posx = value.toInt();
                        else if (key == "posy") hero->posy = value.toInt();
                        else if (key == "face") hero->face = value.toInt();
                        else if (key == "hp") hero->hp = value.toInt();
                        else if (key == "atk") hero->atk = value.toInt();
                        else if (key == "def") hero->def = value.toInt();
                        else if (key == "gold") hero->gold = value.toInt();
                        else if (key == "yellow_key") hero->yellow_key = value.toInt();
                        else if (key == "blue_key") hero->blue_key = value.toInt();
                        else if (key == "red_key") hero->red_key = value.toInt();
                    }
                }
                else if (entityObj->type == "MONSTER")
                {
                    Monster* monster = static_cast<Monster*>(entityObj.get());
                    if (monster)
                    {
                        if (key == "hp") monster->hp = value.toInt();
                        else if (key == "atk") monster->atk = value.toInt();
                        else if (key == "def") monster->def = value.toInt();
                        else if (key == "gold") monster->gold = value.toInt();
                        else if (key == "traitID") monster->traitID = value;
                    }
                }
                //拓展实体
            }
        }
        //保存文件最后的实体
        if (entityObj != nullptr)
            entity[entityObj->id] = entityObj;

        // 关闭文件
        file.close();
    }
}

std::shared_ptr<Entity> Data::getHeroData()
{
    return entity.value("hero", nullptr);
}

std::shared_ptr<Entity> Data::getXY(int x, int y,int layer)
{
    //输入数据不合法时返回nullptr
    if (x < 0 || x >= map.len || y < 0 || y >= map.wid || layer < 0 || layer >= map.map.size())
        return nullptr;
    // 默认获取第layer层坐标X,Y的实体
    QString entityId = map.map[layer].floor[x][y].entityId;
    return entity.value(entityId, nullptr);
}