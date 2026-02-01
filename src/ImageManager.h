//====================
// 图片资源管理器
//====================
#pragma once
#include <QPixmap>
#include <QMap>
#include <QString>

// 精灵图切割信息
struct SpriteInfo {
    int row;    // 行索引（从0开始）
    int col;    // 列索引（从0开始）
};

class ImageManager
{
public:
    // 加载所有资源
    void loadResources();
    
    // 获取实体图片（根据实体ID）
    QPixmap getEntityImage(const QString& entityId) const;
    
    // 获取地板图片（根据地板ID）
    QPixmap getFloorImage(int floorId) const;
    
    // 获取英雄图片（根据朝向和动画帧）
    // face: 0=下, 1=左, 2=右, 3=上 (brave.png的行顺序)
    // frame: 0-3 动画帧
    QPixmap getHeroImage(int face, int frame = 0) const;
    
    // 原始精灵图尺寸
    static const int SPRITE_SIZE = 32;

private:
    // 从精灵图切割单个图片
    QPixmap cropSprite(const QPixmap& spriteSheet, int row, int col) const;
    
    // 加载地形精灵图
    void loadTerrains();
    
    // 加载动画精灵图
    void loadAnimates();
    
    // 加载敌人精灵图
    void loadEnemys();
    
    // 加载物品精灵图
    void loadItems();
    
    // 加载英雄精灵图
    void loadHero();
    
    // 加载缺失材质
    void loadLackResource();
    
    // 地形精灵图
    QPixmap terrainsSheet;
    // 动画精灵图
    QPixmap animatesSheet;
    // 敌人精灵图
    QPixmap enemysSheet;
    // 英雄精灵图
    QPixmap heroSheet;
    // 物品精灵图
    QPixmap itemsSheet;
    // 缺失材质
    QPixmap lackResource;
    
    // 实体ID到精灵图位置的映射
    QMap<QString, SpriteInfo> entitySpriteMap;
    // 地板ID到精灵图位置的映射
    QMap<int, SpriteInfo> floorSpriteMap;
    
    // 预切割的实体图片缓存
    QMap<QString, QPixmap> entityCache;
    // 预切割的地板图片缓存
    QMap<int, QPixmap> floorCache;
    // 预切割的英雄图片缓存 (face * 4 + frame)
    QMap<int, QPixmap> heroCache;
};
