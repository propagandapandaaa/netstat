#ifndef HASHMAP_H
#define HASHMAP_H

#include <unordered_map>
#include <mutex>
#include <string>

template <typename Key, typename T>
class Mutex_Hashmap
{
private:
    std::unordered_map<Key, T> map;
    mutable std::mutex mapMutex;

public:
    Mutex_Hashmap() = default;
    ~Mutex_Hashmap() = default;

    void insert(const Key &key, const T &value);

    bool find(const Key &key, T &value) const;

    void erase(const Key &key);

    std::unordered_map<Key, T> getCopy() const;

    size_t size() const;
};

#endif