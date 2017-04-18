#include "SpriteDesc.h"
#include "DataParser.h"

DX_DEF_STRUCT_P(SpriteDesc)
DX_ADD_FIELD_P(SpriteDesc, filename);
DX_ADD_FIELD_P(SpriteDesc, name);
DX_ADD_OPT_FIELD_P(SpriteDesc, zOrder);
DX_ADD_OPT_FIELD_P(SpriteDesc, position);
DX_ADD_OPT_FIELD_P(SpriteDesc, anchorPoint);

using namespace cocos2d;
DX_DEF_STRUCT_P(Vec2)
DX_ADD_OPT_FIELD_P(Vec2, x);
DX_ADD_OPT_FIELD_P(Vec2, y);

using namespace cocos2d;
DX_DEF_STRUCT_P(Size)
DX_ADD_OPT_FIELD_P(Size, width);
DX_ADD_OPT_FIELD_P(Size, height);

