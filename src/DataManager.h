//====================
//地图数据
//====================
#pragma once
#include <QObject>
#include <QMap>
#include <QVector>
#include <memory>
#include "Entity.h"
#include "MapLoader.h"

//====================
//获取地图数据
//Data.map
//获取某层数据
//Data.map.getFloor(int layer)
//获取某格数据
//Data.map.getFloor(int layer).getBlock(int x,int y)
//获取某格实体数据
//Data.getEntity(Data.map.getFloor(int layer).getBlock(int x,int y).entityId)
//====================

class Data
{
public:
    Data(int mapLen,int mapWid,int mapLayers) : map(mapLen,mapWid,mapLayers)
    {
        LoadMap(mapLen,mapWid,mapLayers);
        LoadEntity();
    }

    void LoadMap(int mapLen,int mapWid,int mapLayers);

    void LoadEntity();

    std::shared_ptr<Entity> getEntity(QString id){return entity[id];}

    std::shared_ptr<HeroData> getHeroData();

    std::shared_ptr<Entity> getXY(int x,int y,int layer);

    void setEntity(const QString& id, int x, int y, int layer);
    
    void removeEntity(int x, int y, int layer);

    Map map;
    QMap<QString,std::shared_ptr<Entity>> entity;
};