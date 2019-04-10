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

#include "ThreadPool.h"
using namespace core;

void ThreadPool::work_thread() {
    while (!done) {
        FunctionWrapper task;
        if (work_queue.try_pop(task)) {
            task();
        } else {
            std::this_thread::yield();
        }
    }
}

ThreadPool::ThreadPool()
        : done(false) {
    const unsigned int thread_count = std::thread::hardware_concurrency() - 1;

    for(unsigned int i = 0; i < thread_count; ++i) {
        threads.emplace_back(std::thread(&ThreadPool::work_thread, this));
    }
}

ThreadPool::~ThreadPool() {
    done = true;
    for (auto &thread : threads) {
        if(thread.joinable())
            thread.join();
    }
}
