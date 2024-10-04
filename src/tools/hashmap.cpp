#include "../include/hashmap.h"

template <typename Key, typename T>
void Mutex_Hashmap<Key, T>::insert(const Key &key, const T &value)
{
    std::lock_guard<std::mutex> lock(mapMutex);
    map[key] = value;
}

template <typename Key, typename T>
typename std::unordered_map<Key, T>::const_iterator Mutex_Hashmap<Key, T>::find(const Key &key) const
{
    std::lock_guard<std::mutex> lock(mapMutex);
    return map.find(key);
}

template <typename Key, typename T>
void Mutex_Hashmap<Key, T>::erase(const Key &key)
{
    std::lock_guard<std::mutex> lock(mapMutex);
    map.erase(key);
}

template <typename Key, typename T>
std::unordered_map<Key, T> Mutex_Hashmap<Key, T>::getCopy() const
{
    std::lock_guard<std::mutex> lock(mapMutex);
    return map;
}

template <typename Key, typename T>
size_t Mutex_Hashmap<Key, T>::size() const
{
    std::lock_guard<std::mutex> lock(mapMutex);
    return map.size();
}
