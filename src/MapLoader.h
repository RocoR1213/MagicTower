//====================
//地图管理
//====================
#pragma once
#include <QVector>
#include <QString>

//地图块结构
class Block
{
public:
    int floorId;
    QString entityId;
};

//层结构
class Floor
{
public:
    Floor(int length,int width)
    {
        floor.resize(length); // X轴方向（列数）
        for (int i = 0; i < length; ++i)
        {
            floor[i].resize(width); // Y轴方向（行数）
        }
    }
    Block& getBlock(int x,int y){return floor[x][y];}
    QVector<QVector<Block>> floor;
};

//地图管理器
class Map
{
public:
    Map(int length,int width,int layers) : len(length),wid(width),layers(layers)
    {
        for (int i = 0; i < layers; ++i)
        {
            map.append(Floor(length,width));
        }
    }

    Floor& getFloor(int layer){return map[layer];}

    QVector<Floor> map;
    int len;
    int wid;
    int layers;
};