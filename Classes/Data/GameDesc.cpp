#include "GameDesc.h"
#include "DataParser.h"

GameDesc* GameDesc::fromJson(const std::string& fname) {
    return parseFromJson<GameDesc*>(fname);
}

DX_DEF_STRUCT_P(SceneDesc)
DX_ADD_FIELD_P(SceneDesc, name);
DX_ADD_OPT_FIELD_P(SceneDesc, filename);
DX_ADD_FIELD_P(SceneDesc, components);

DX_DEF_STRUCT_P(GameDesc)
DX_ADD_FIELD_P(GameDesc, version);
DX_ADD_FIELD_P(GameDesc, name);
DX_ADD_FIELD_P(GameDesc, scenes);
