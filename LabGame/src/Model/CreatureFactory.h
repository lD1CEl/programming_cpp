#ifndef LABGAME_SRC_MODEL_CREATUREFACTORY_H_
#define LABGAME_SRC_MODEL_CREATUREFACTORY_H_

#include "../Factory.h"
#include "../Singleton.h"
#include "IGameObject.h"
#include <memory>

typedef std::shared_ptr<IGameObject> (*CreatureCreator)();

typedef Factory<std::shared_ptr<IGameObject>, char, CreatureCreator> CreatureFactoryImpl;

typedef Singleton<CreatureFactoryImpl> CreatureFactory;

#endif // LABGAME_SRC_MODEL_CREATUREFACTORY_H_
