#ifndef LABGAME_SRC_MODEL_CREATUREFACTORY_H_
#define LABGAME_SRC_MODEL_CREATUREFACTORY_H_

#include "../Factory.h"
#include "../Singleton.h"
#include "ICreature.h"
#include <memory>

typedef std::shared_ptr<ICreature> (*CreatureCreator)();

typedef Factory<std::shared_ptr<ICreature>, char, CreatureCreator> CreatureFactoryImpl;

typedef Singleton<CreatureFactoryImpl> CreatureFactory;

#endif // LABGAME_SRC_MODEL_CREATUREFACTORY_H_
