#ifndef LABGAME_SRC_FACTORY_H_
#define LABGAME_SRC_FACTORY_H_

#include <map>
#include <string>
#include <stdexcept>

template <class IdentifierType, class ProductType>
class DefaultFactoryError {
public:
    class Exception : public std::exception {
    public:
        Exception(const IdentifierType& unknownId) : unknownId_(unknownId) {}
        virtual const char* what() const noexcept {
            return "Unknown object type passed to Factory.";
        }
        IdentifierType GetId() const { return unknownId_; }
    private:
        IdentifierType unknownId_;
    };

protected:
    static ProductType OnUnknownType(const IdentifierType& id) {
        throw Exception(id);
    }
};

template
<
    class AbstractProduct,
    typename IdentifierType,
    typename ProductCreator = AbstractProduct (*)(),
    template<typename, class> class FactoryErrorPolicy = DefaultFactoryError
>
class Factory : public FactoryErrorPolicy<IdentifierType, AbstractProduct> {
public:
    bool Register(const IdentifierType& id, ProductCreator creator) {
        return associations_.insert(
            typename AssocMap::value_type(id, creator)).second;
    }

    bool Unregister(const IdentifierType& id) {
        return associations_.erase(id) == 1;
    }

    AbstractProduct CreateObject(const IdentifierType& id) {
        typename AssocMap::const_iterator i = associations_.find(id);
        if (i != associations_.end()) {
            return (i->second)();
        }
        return this->OnUnknownType(id);
    }

private:
    typedef std::map<IdentifierType, ProductCreator> AssocMap;
    AssocMap associations_;
};

#endif // LABGAME_SRC_FACTORY_H_
