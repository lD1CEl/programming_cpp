#ifndef LABGAME_SRC_MODEL_ICREATURE_H_
#define LABGAME_SRC_MODEL_ICREATURE_H_

#include "IGameObject.h"


class ICreature : public IGameObject {
public:
    virtual ~ICreature() = default;
};

#endif  // LABGAME_SRC_MODEL_ICREATURE_H_
