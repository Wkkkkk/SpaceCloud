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

#ifndef HELLOWORLD_THREAD_POOL_H
#define HELLOWORLD_THREAD_POOL_H

#include <atomic>
#include <thread>
#include <vector>

#define BOOST_THREAD_PROVIDES_FUTURE
#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION

#include <boost/thread/future.hpp>

#include "ThreadsafeQueue.h"
#include "FunctionWrapper.h"

namespace core {

class ThreadPool {
private:
    std::atomic_bool done;
    ThreadsafeQueue<FunctionWrapper> work_queue;
    std::vector<std::thread> threads;

    void work_thread();

public:
    ThreadPool();

    ~ThreadPool();

    template <typename FunctionType>
    auto submit(FunctionType &&f)
    {
        using result_type = typename std::result_of<FunctionType()>::type;

        boost::packaged_task<result_type> task(std::forward<FunctionType>(f));
        boost::future<result_type> res(task.get_future());
        work_queue.push(std::move(task));

        return res;
    }
};
}

#endif //HELLOWORLD_THREAD_POOL_H
