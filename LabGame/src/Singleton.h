#ifndef LABGAME_SRC_SINGLETON_H_
#define LABGAME_SRC_SINGLETON_H_

template <class T>
class Singleton {
public:
    static T& Instance() {
        static T instance;
        return instance;
    }

protected:
    Singleton() {}
    ~Singleton() {}

private:
    Singleton(const Singleton&);
    Singleton& operator=(const Singleton&);
};

#endif // LABGAME_SRC_SINGLETON_H_
