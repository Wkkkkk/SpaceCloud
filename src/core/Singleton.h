/*
 * Copyright (c) 2018 Ally of Intelligence Technology Co., Ltd. All rights reserved.
 *
 * Created by WuKun on 3/8/19.
 * Contact with:wk707060335@gmail.com
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http: *www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/

#ifndef PROTYPE_SINGLETON_H
#define PROTYPE_SINGLETON_H


#include <map>
#include <mutex>
#include <memory>
#include <thread>
#include <vector>
#include <algorithm>
#include <functional>
#include <iostream>
namespace core {
template<class T>
class Key {
private:
    size_t id_;

public:
    Key() = default;

    explicit Key(size_t id)
            : id_(id) {
    }

    void setId(size_t id) {
        id_ = id;
    }

    size_t getId() const {
        return id_;
    }

    bool operator<(const Key<T> &right) const {
        return id_ < right.getId();
    }

    bool operator==(const Key<T> &right) const {
        return id_ == right.getId();
    }
};

template<class T>
using Filter = std::function<bool(const T &)>;

template<class T>
class Handle {
private:
    std::map<Key<T>, T> map_;

public:
    Handle() = default;

    void update(const Key<T> &key, const T &t) {
        map_[key] = t;

        //C++17
        //auto [it, inserted] = map_.insert_or_assign(key, t);
    }

    void remove(const Key<T> &key) {
        map_.erase(key);
    }

    T findByKey(const Key<T> &key) const {
        auto it = map_.find(key);
        if (it == map_.end())
            return T();
        return it->second;
    }

    std::vector<T> findByFilter(const Filter<T> &filter) const {
        std::vector<T> vector;
        for (const auto &pair : map_) {
            if (filter(pair.second))
                vector.push_back(pair.second);
        }
        return vector;
    }

    bool empty() const {
        return map_.empty();
    }

    size_t size() const {
        return map_.size();
    }

    void print() const {
        std::cout << "----------" << std::endl;
        for (const auto &pair : map_) {
            std::cout << "key: " << pair.first.getId() << " value: " << pair.second << std::endl;
        }
    }
};

template<class T>
class Singleton {
public:
    Singleton(const Singleton &) = delete;

    Singleton &operator=(const Singleton &) = delete;

    static std::shared_ptr<Singleton> getInstance() {
        static std::shared_ptr<Singleton<T>> instance(new Singleton<T>);
        return instance;
    }

    void update(const Key<T> &key, const T &obj) {
        std::lock_guard<std::mutex> lck(mtx);
        handle.update(key, obj);
    }

    T findByID(const Key<T> &key) const {
        std::lock_guard<std::mutex> lck(mtx);
        return handle.findByKey(key);
    }

    std::vector<T> findByFilter(const Filter<T> &filter) const {
        std::lock_guard<std::mutex> lck(mtx);
        return handle.findByFilter(filter);
    }

    void remove(const Key<T> &key) {
        std::lock_guard<std::mutex> lck(mtx);
        handle.remove(key);
    }

    void print() {
        std::lock_guard<std::mutex> lck(mtx);
        handle.print();
    }

private:
    Singleton() = default;

    mutable std::mutex mtx;
    Handle<T> handle;
};
}

#endif //PROTYPE_SINGLETON_H
