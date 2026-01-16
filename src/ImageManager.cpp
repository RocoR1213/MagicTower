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
    loadAnimates();
    loadItems();
    loadEnemys();
    loadHero();
    loadLackResource();
}

void ImageManager::loadTerrains()
{
    // 加载地形精灵图
    terrainsSheet = QPixmap(":/images/terrains.png");
    if (terrainsSheet.isNull()) {
        qWarning() << "无法加载地形精灵图: :/images/terrains.png";
        return;
    }
    
    // 地板映射
    floorSpriteMap[0] = {1, 0};  // 默认地板
    floorSpriteMap[1] = {0, 0};  // 草地
    floorSpriteMap[2] = {2, 0};  // 墙壁作为特殊地板
    
    // 预切割并缓存地板图片
    for (auto it = floorSpriteMap.begin(); it != floorSpriteMap.end(); ++it) {
        floorCache[it.key()] = cropSprite(terrainsSheet, it.value().row, it.value().col);
    }
    
    // 预切割并缓存实体图片
    entityCache["up_stair"] = cropSprite(terrainsSheet, 6, 0);     // 上楼梯 - 第7行（索引6）
    entityCache["down_stair"] = cropSprite(terrainsSheet, 5, 0);   // 下楼梯 - 第6行（索引5）
}

void ImageManager::loadHero()
{
    // 加载英雄精灵图
    heroSheet = QPixmap(":/images/brave.png");
    if (heroSheet.isNull()) {
        qWarning() << "无法加载英雄精灵图: :/images/brave.png";
        return;
    }
    
    // 预切割并缓存所有英雄帧 (4行4列)
    for (int face = 0; face < 4; ++face) {
        for (int frame = 0; frame < 4; ++frame) {
            int key = face * 4 + frame;
            heroCache[key] = cropSprite(heroSheet, face, frame);
        }
    }
}

void ImageManager::loadAnimates()
{
    // 加载动画精灵图
    animatesSheet = QPixmap(":/images/animates.png");
    if (animatesSheet.isNull()) {
        qWarning() << "无法加载动画精灵图: :/images/animates.png";
        return;
    }
    
    // 处理墙和门的材质
    entityCache["wall"] = cropSprite(animatesSheet, 10, 0);   // 墙 - 第11行第1列（行索引10，列索引0）
    entityCache["yellow_door"] = cropSprite(animatesSheet, 4, 0);  // 黄门 - 第五行第1列（行索引4，列索引0）
    entityCache["blue_door"] = cropSprite(animatesSheet, 5, 0);     // 蓝门 - 第六行第1列（行索引5，列索引0）
    entityCache["red_door"] = cropSprite(animatesSheet, 6, 0);      // 红门 - 第七行第1列（行索引6，列索引0）
}

void ImageManager::loadEnemys()
{
    // 加载敌人精灵图
    enemysSheet = QPixmap(":/images/enemys.png");
    if (enemysSheet.isNull()) {
        qWarning() << "无法加载敌人精灵图: :/images/enemys.png";
        return;
    }
    
    // 预切割并缓存怪物图片
    entityCache["green_slime"] = cropSprite(enemysSheet, 0, 0);  // 绿史莱姆 - 第一行第一列
    entityCache["red_slime"] = cropSprite(enemysSheet, 1, 0);     // 红史莱姆 - 第二行第一列
    entityCache["black_slime"] = cropSprite(enemysSheet, 2, 0);   // 黑史莱姆 - 第三行第一列
    entityCache["skeleton"] = cropSprite(enemysSheet, 9, 0);      // 骷髅 - 第十行第一列
}

void ImageManager::loadItems()
{
    // 加载物品精灵图
    itemsSheet = QPixmap(":/images/items.png");
    if (itemsSheet.isNull()) {
        qWarning() << "无法加载物品精灵图: :/images/items.png";
        return;
    }
    
    // 预切割并缓存物品图片（多行1列结构）
    entityCache["yellow_key"] = cropSprite(itemsSheet, 0, 0);    // 黄钥匙 - 第一行
    entityCache["blue_key"] = cropSprite(itemsSheet, 1, 0);       // 蓝钥匙 - 第二行
    entityCache["red_key"] = cropSprite(itemsSheet, 2, 0);        // 红钥匙 - 第三行
    entityCache["atk_gem"] = cropSprite(itemsSheet, 16, 0);        // 攻击宝石 - 第十七行
    entityCache["def_gem"] = cropSprite(itemsSheet, 17, 0);        // 防御宝石 - 第十八行
    entityCache["hp_potion_1"] = cropSprite(itemsSheet, 20, 0);    // 生命药水1 - 第二十一行
    entityCache["hp_potion_2"] = cropSprite(itemsSheet, 21, 0);    // 生命药水2 - 第二十二行
    entityCache["hp_potion_3"] = cropSprite(itemsSheet, 22, 0);    // 生命药水3 - 第二十三行
}

QPixmap ImageManager::cropSprite(const QPixmap& spriteSheet, int row, int col) const
{
    int x = col * SPRITE_SIZE;
    int y = row * SPRITE_SIZE;
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
    
    // 无法找到材质，使用缺失材质替代
    return lackResource;
}

QPixmap ImageManager::getFloorImage(int floorId) const
{
    if (floorCache.contains(floorId)) {
        return floorCache[floorId];
    }
    // 默认返回地板0，如果地板0不存在则使用缺失材质
    QPixmap floorImage = floorCache.value(0);
    return floorImage.isNull() ? lackResource : floorImage;
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
    QPixmap heroImage = heroCache.value(key);
    return heroImage.isNull() ? lackResource : heroImage;
}

void ImageManager::loadLackResource()
{
    // 加载缺失材质
    lackResource = QPixmap(":/images/lack_resource.png");
    if (lackResource.isNull()) {
        qFatal("无法加载缺失材质: :/images/lack_resource.png");
    }
}
