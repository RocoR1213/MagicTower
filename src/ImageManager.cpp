#include "ImageManager.h"
#include <QDebug>

ImageManager::ImageManager()
{
}

ImageManager::~ImageManager()
{
}

void ImageManager::loadResources()
{
    loadTerrains();
    loadHero();
}

void ImageManager::loadTerrains()
{
    // 加载地形精灵图
    terrainsSheet = QPixmap(":/images/terrains.png");
    if (terrainsSheet.isNull()) {
        qWarning() << "无法加载地形精灵图: :/images/terrains.png";
        return;
    }
    
    // 根据terrains.png的布局设置映射
    // terrains.png 是 32像素宽，纵向排列多个32x32的精灵
    // 行0: 草地
    // 行1: 地板
    // 行2: 墙壁
    // 行3: 楼梯上
    // 行4-5: 楼梯下
    // 行6-9: 箭头
    // 行10-11: 门
    // 行12-13: 宝箱/木箱
    // 行14-17: 空白/边框
    
    // 地板映射
    floorSpriteMap[0] = {1, 0};  // 默认地板
    floorSpriteMap[1] = {0, 0};  // 草地
    floorSpriteMap[2] = {2, 0};  // 墙壁作为特殊地板
    
    // 实体映射
    entitySpriteMap["wall"] = {2, 0};           // 墙壁
    entitySpriteMap["stair_up"] = {3, 0};       // 上楼梯
    entitySpriteMap["stair_down"] = {4, 0};     // 下楼梯
    
    // 预切割并缓存地板图片
    for (auto it = floorSpriteMap.begin(); it != floorSpriteMap.end(); ++it) {
        floorCache[it.key()] = cropSprite(terrainsSheet, it.value().row, it.value().col);
    }
    
    // 预切割并缓存实体图片
    for (auto it = entitySpriteMap.begin(); it != entitySpriteMap.end(); ++it) {
        entityCache[it.key()] = cropSprite(terrainsSheet, it.value().row, it.value().col);
    }
    
    qDebug() << "地形精灵图加载完成，尺寸:" << terrainsSheet.size();
}

void ImageManager::loadHero()
{
    // 加载英雄精灵图
    heroSheet = QPixmap(":/images/brave.png");
    if (heroSheet.isNull()) {
        qWarning() << "无法加载英雄精灵图: :/images/brave.png";
        return;
    }
    
    // brave.png 是 4列 x 4行 的精灵图
    // 行0: 朝下
    // 行1: 朝左
    // 行2: 朝右
    // 行3: 朝上
    // 每行4帧动画
    
    // 预切割并缓存所有英雄帧
    for (int face = 0; face < 4; ++face) {
        for (int frame = 0; frame < 4; ++frame) {
            int key = face * 4 + frame;
            heroCache[key] = cropSprite(heroSheet, face, frame);
        }
    }
    
    qDebug() << "英雄精灵图加载完成，尺寸:" << heroSheet.size();
}

QPixmap ImageManager::cropSprite(const QPixmap& spriteSheet, int row, int col) const
{
    if (spriteSheet.isNull()) {
        return QPixmap();
    }
    
    int x = col * SPRITE_SIZE;
    int y = row * SPRITE_SIZE;
    
    // 边界检查
    if (x + SPRITE_SIZE > spriteSheet.width() || y + SPRITE_SIZE > spriteSheet.height()) {
        qWarning() << "精灵图切割越界: row=" << row << ", col=" << col;
        return QPixmap();
    }
    
    return spriteSheet.copy(x, y, SPRITE_SIZE, SPRITE_SIZE);
}

QPixmap ImageManager::getEntityImage(const QString& entityId) const
{
    // 先尝试直接查找
    if (entityCache.contains(entityId)) {
        return entityCache[entityId];
    }
    
    // 尝试通过前缀匹配
    for (auto it = entityCache.begin(); it != entityCache.end(); ++it) {
        if (entityId.startsWith(it.key())) {
            return it.value();
        }
    }
    
    return QPixmap();  // 返回空图片
}

QPixmap ImageManager::getFloorImage(int floorId) const
{
    if (floorCache.contains(floorId)) {
        return floorCache[floorId];
    }
    // 默认返回地板0
    return floorCache.value(0, QPixmap());
}

QPixmap ImageManager::getHeroImage(int face, int frame) const
{
    // 将游戏中的face转换为精灵图的行
    // 游戏: 0=左, 1=上, 2=右, 3=下
    // 精灵图: 0=下, 1=左, 2=右, 3=上
    int spriteRow;
    switch (face) {
        case 0: spriteRow = 1; break;  // 左 -> 行1
        case 1: spriteRow = 3; break;  // 上 -> 行3
        case 2: spriteRow = 2; break;  // 右 -> 行2
        case 3: spriteRow = 0; break;  // 下 -> 行0
        default: spriteRow = 0; break;
    }
    
    int key = spriteRow * 4 + (frame % 4);
    return heroCache.value(key, QPixmap());
}

bool ImageManager::hasEntityImage(const QString& entityId) const
{
    if (entityCache.contains(entityId)) {
        return true;
    }
    
    // 尝试通过前缀匹配
    for (auto it = entityCache.begin(); it != entityCache.end(); ++it) {
        if (entityId.startsWith(it.key())) {
            return true;
        }
    }
    
    return false;
}
