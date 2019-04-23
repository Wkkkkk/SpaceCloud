/*
 * Copyright (c) 2018 Ally of Intelligence Technology Co., Ltd. All rights reserved.
 *
 * Created by WuKun on 2/27/19.
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

#ifndef HELLOWORLD_THREADSAFE_QUEUE_H
#define HELLOWORLD_THREADSAFE_QUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>
namespace core {

/**
 * a thread-safe queue plus a free function
 */
template <typename T>
class ThreadsafeQueue {
private:
    bool free = false;
    mutable std::mutex mut;
    std::queue<T> data_queue;
    std::condition_variable data_cond;
public:
    ThreadsafeQueue() = default;

    /**
     * @brief receive a object to queue
     * @param generic type which has a default constructor
     */
    void push(T data) {
        std::lock_guard<std::mutex> lk(mut);
        data_queue.push(std::move(data));
        data_cond.notify_one();
    }

    /**
     * @brief a block function which return task_package for example
     * @return a function-type object which could be null
     */
    T take() {
        std::unique_lock<std::mutex> lk(mut);
        data_cond.wait(lk, [this] { return !data_queue.empty() || free; });

        if (data_queue.empty()) return T();
        T value = std::move(data_queue.front());
        data_queue.pop();
        return value;
    }

    /**
     * @brief relieve all conditional variable
     */
    void free_all() {
        std::lock_guard<std::mutex> lk(mut);
        free = true;
        data_cond.notify_all();
    }

    /**
     * a block query function
     * @warning can not be called in other member funcitons
     */
    bool empty() const {
        std::lock_guard<std::mutex> lk(mut);
        return data_queue.empty();
    }
};
}

#endif //HELLOWORLD_THREADSAFE_QUEUE_H
