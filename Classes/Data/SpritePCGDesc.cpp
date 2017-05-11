#include "SpritePCGDesc.h"
#include "../Utils/RandomHelper.h"
#include "DataParser.h"
#include "DataMacros.h"

SpritePCGLayerDesc::SpritePCGLayerDesc(
        SpritePCGLayerDesc::LayerDrawF& drawf) :
    m_drawF(drawf)
{}

SpritePCGLayerDesc::SpritePCGLayerDesc(
        SpritePCGLayerDesc::LayerDrawF &drawf,
        SpritePCGLayerDesc::LayerColorF& colorf) :
    m_drawF(drawf),
    m_colorF(colorf)
{}

SpritePCGLayerDesc* SpritePCGLayerDesc::create(
        SpritePCGLayerDesc::LayerDrawF& drawf) {
    auto spcg = new (std::nothrow) SpritePCGLayerDesc(drawf);
    if (!spcg) return nullptr;
    spcg->autorelease();
    return spcg;
}

SpritePCGLayerDesc* SpritePCGLayerDesc::create(
        SpritePCGLayerDesc::LayerDrawF &drawf,
        SpritePCGLayerDesc::LayerColorF& colorf) {
    auto spcg = new (std::nothrow) SpritePCGLayerDesc(drawf, colorf);
    if (!spcg) return nullptr;
    spcg->autorelease();
    return spcg;
}

bool SpritePCGLayerDesc::initFromLua(lua_State* L, int idx) {
    using namespace cocos2d;
    DX_LUA_DESC_GET_MEMBER(L, idx, string, name);
    DX_LUA_DESC_GET_MEMBER(L, idx, Size, size);
    DX_LUA_DESC_GET_OPT_MEMBER(L, idx, LayerDrawF, drawF);
    DX_LUA_DESC_GET_OPT_MEMBER(L, idx, LayerColorF, colorF);
}

size_t PixelFormat2BytePerPixel(cocos2d::Texture2D::PixelFormat fmt) {
    switch (fmt) {
    case cocos2d::Texture2D::PixelFormat::BGRA8888 :
    case cocos2d::Texture2D::PixelFormat::RGBA8888 :
        return 4;
    case cocos2d::Texture2D::PixelFormat::RGBA4444 :
        return 2;
    default:
        return 0;
    }
}

size_t SpritePCGLayerDesc::getWidth() const {
    return m_size.width;
}

size_t SpritePCGLayerDesc::getHeight() const {
    return m_size.height;
}

SpritePCGLayerDesc::CellTypeVec SpritePCGLayerDesc::generateLayerData() {
    CellTypeVec data(int(m_size.width)*int(m_size.height), CellType::EMPTY);
    for (size_t x = 0; x < int(m_size.width); ++x) {
        for (size_t y = 0; y < int(m_size.height); ++y) {
            auto ct = m_drawF(x, y);
            setCellType(data, x, y, ct);
        }
    }
    generateEdges(data);
    return data;
}

void SpritePCGLayerDesc::generateEdges(SpritePCGLayerDesc::CellTypeVec& data) {
    for (size_t x = 0; x < m_size.width; ++x) {
        for (size_t y = 0; y < m_size.height; ++y) {
            auto ct = getCellType(data, x, y);
            if (ct == CellType::LAYER) {
                if (y > 0 && getCellType(data, x, y-1) == CellType::EMPTY) {
                    setCellType(data, x, y-1, CellType::SOLID);
                }
                if (x > 0 && getCellType(data, x-1, y) == CellType::EMPTY) {
                    setCellType(data, x-1, y, CellType::SOLID);
                }
                if (x < (m_size.width-1) && getCellType(data, x+1, y) == CellType::EMPTY) {
                    setCellType(data, x+1, y, CellType::SOLID);
                }
                if (y < m_size.height && getCellType(data, x, y+1) == CellType::EMPTY) {
                    setCellType(data, x, y+1, CellType::SOLID);
                }
            }
        }
    }
}

void SpritePCGLayerDesc::generateData(std::vector<uint8_t>& data, cocos2d::Texture2D::PixelFormat fmt) {
    auto cellTypes = generateLayerData();
    auto width = getWidth();
    auto height = getHeight();
    cocos2d::log("width=%d height=%d", int(width), int(height));
    typedef cocos2d::Texture2D::PixelFormat PixelFormat;
    size_t S = width * height;
    size_t bytesPerPixel = PixelFormat2BytePerPixel(fmt);
    size_t nb = S * bytesPerPixel;
    data.resize(nb);
    size_t x = 0;
    size_t y = 0;
    for (size_t i = 0; i < nb; i += bytesPerPixel) {
        auto ct = getCellType(cellTypes, x, y);
        auto pixel = m_colorF(ct, x, y);
        cocos2d::log("r=%d g=%d b=%d", int(pixel.r), int(pixel.g), int(pixel.b));
        switch (fmt) {
            case PixelFormat::RGBA8888 :
                data[i + 0] = pixel.r;
                data[i + 1] = pixel.g;
                data[i + 2] = pixel.b;
                data[i + 3] = pixel.a;
                break;
            default:;
        }
        ++x;
        if (x == width) {
            ++y;
            x = 0;
        }
    }
}

std::vector<uint8_t> SpritePCGLayerDesc::generateData(cocos2d::Texture2D::PixelFormat fmt) {
    std::vector<uint8_t> data;
    generateData(data, fmt);
    return data;
}

SpritePCGLayerDesc::CellType SpritePCGLayerDesc::getCellType(CellTypeVec& data, size_t x, size_t y) {
    return data.at(size_t(m_size.width) * y + x);
}

void SpritePCGLayerDesc::setCellType(CellTypeVec& data, size_t x, size_t y, CellType t) {
    data.at(size_t(m_size.width) * y + x) = t;
}

cocos2d::Texture2D* SpritePCGLayerDesc::createTexture() {
    cocos2d::Texture2D::PixelFormat fmt = cocos2d::Texture2D::PixelFormat::RGBA8888;
    auto texture = new (std::nothrow) cocos2d::Texture2D();
    auto data = generateData(fmt);
    if (texture && texture->initWithData(
            data.data(), data.size(), fmt,
            int(m_size.width), int(m_size.height), m_size)) {
        cocos2d::log("texture created");
        texture->autorelease();
        return texture;
    }
    CC_SAFE_DELETE(texture);
    return nullptr;
}

cocos2d::Sprite* SpritePCGLayerDesc::createSprite() {
    auto texture = createTexture();
    auto sp = cocos2d::Sprite::createWithTexture(texture);
    return sp;
}

cocos2d::Texture2D* SpritePCGDesc::createTexture() {
    cocos2d::Texture2D::PixelFormat fmt = cocos2d::Texture2D::PixelFormat::RGBA8888;
    auto texture = new cocos2d::Texture2D();
    auto width = size_t(m_size.width);
    auto height = size_t(m_size.height);
    size_t S = width * height;
    size_t bytesPerPixel = PixelFormat2BytePerPixel(fmt);
    size_t nb = S * bytesPerPixel;
    std::vector<uint8_t> data(nb);
    for (auto& layer : m_layers) {
        auto layerData = layer->generateData(fmt);
    }
    if (!texture->initWithData(static_cast<void*>(data.data()),
        data.size(), fmt, width, height, m_size)) {
        CC_SAFE_DELETE(texture);
        return nullptr;
    }
    return texture;
}

DX_DEF_STRUCT_P(PixelDesc)
DX_ADD_FIELD_P(PixelDesc, r)
DX_ADD_FIELD_P(PixelDesc, g)
DX_ADD_FIELD_P(PixelDesc, b)
DX_ADD_FIELD_P(PixelDesc, a)
