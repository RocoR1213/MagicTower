//====================
//实体数据
//====================
#pragma once
#include <QString>
#include <QMap>
#include <QVariant>
#include <QVector>

// 实体基类
class Entity
{
public:
    virtual ~Entity() = default;
    Entity(const QString &type) : type(type){}
    QString id;
    QString type;
};

//==============================
// 具体实体类型声明 - 预定义类型
//==============================
class Air : public Entity
{
public:
    Air() : Entity("AIR"){}
};

class HeroData : public Entity
{
public:
    //pos
    int posx;
    int posy;
    int face;   //0左1上2右3下
    //statu
    int hp;
    int atk;
    int def;
    int gold;
    int yellow_key;
    int blue_key;
    int red_key;
    HeroData() : Entity("HERODATA"){}
};

class Wall : public Entity
{
public:
    Wall() : Entity("WALL"){}
};

class Door : public Entity
{
public:
    Door() : Entity("DOOR"){}
};

class Item : public Entity
{
public:
    Item() : Entity("ITEM"){}
};

class Monster : public Entity
{
public:
    int hp;
    int atk;
    int def;
    int gold;
    QString traitID;
    Monster() : Entity("MONSTER"){}
};

class NPC : public Entity
{
public:
    NPC() : Entity("NPC"){}
};

class Merchant : public Entity
{
public:
    Merchant() : Entity("MERCHANT"){}
};