#ifndef SPRITEPCGDESC_H
#define SPRITEPCGDESC_H

#include "cocos2d.h"
#include "DataMacros.h"

size_t PixelFormat2BytePerPixel(cocos2d::Texture2D::PixelFormat fmt);

struct PixelDesc {
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
    uint8_t a = 0;
};

class SpritePCGLayerDesc : public cocos2d::Ref {
public:
    enum class CellType {
        EMPTY,
        SOLID,
        LAYER,
        INVALID = -1
    };
    typedef std::function<PixelDesc(CellType, size_t, size_t)> LayerColorF;
    typedef std::function<CellType(size_t, size_t)> LayerDrawF;
    DX_CREATE_FROM_LUA_FUNC(SpritePCGLayerDesc)
    SpritePCGLayerDesc() {}
    SpritePCGLayerDesc(LayerDrawF& drawf);
    SpritePCGLayerDesc(LayerDrawF &drawf, LayerColorF& colorf);
    static SpritePCGLayerDesc* create(LayerDrawF& drawf);
    static SpritePCGLayerDesc* create(LayerDrawF &drawf, LayerColorF& colorf);
    size_t getWidth() const;
    size_t getHeight() const;
    typedef cocos2d::Vec2 CellT;
    typedef std::vector<CellType> CellTypeVec;
    void generateData(std::vector<uint8_t>& data, cocos2d::Texture2D::PixelFormat fmt);
    std::vector<uint8_t> generateData(cocos2d::Texture2D::PixelFormat fmt);
    cocos2d::Texture2D* createTexture();
private:
    bool initFromLua(lua_State* L, int idx);
    CellTypeVec generateLayerData();
    void generateEdges(CellTypeVec& cells);
    CellType getCellType(CellTypeVec& data, size_t x, size_t y);
    void setCellType(CellTypeVec& data, size_t x, size_t y, CellType);
    std::string m_name;
    cocos2d::Size m_size;
    cocos2d::Vec2 m_position;
    cocos2d::Vec2 m_anchorPoint;
    std::vector<CellT> m_cells;
    LayerDrawF m_drawF;
    LayerColorF m_colorF;
};

class SpritePCGDesc : public cocos2d::Ref {
public:
    typedef std::function<PixelDesc(
            cocos2d::Texture2D::PixelFormat,
            const PixelDesc&,
            const PixelDesc&)> MergeLayerF;
    cocos2d::Texture2D* createTexture();
private:
    std::string m_name;
    cocos2d::Size m_size;
    cocos2d::Vector<SpritePCGLayerDesc*> m_layers;
    std::vector<uint8_t> generateData(cocos2d::Texture2D::PixelFormat fmt);
    MergeLayerF m_mergeF;
};

#endif // SPRITEPCGDESC_H
