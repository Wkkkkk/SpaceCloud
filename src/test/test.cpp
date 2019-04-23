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
#define BOOST_TEST_MODULE threadpool_test

#include <iostream>
#include <thread>
#include <unistd.h>

#include <boost/test/included/unit_test.hpp>

#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

#include "ThreadPool.h"
#include "Singleton.h"

using namespace core;

BOOST_AUTO_TEST_SUITE(threadpool_test) // name of the test suite
    int calculate(int a, int b) {
        std::cout << "calculate" << std::endl;
        int result = a + b;
        std::this_thread::sleep_for(std::chrono::seconds(3));

        return result;
    }

    void do_some_thing() {
        std::cout << "do_some_thing" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    BOOST_AUTO_TEST_CASE(test1)
    {
        std::cout << "test1" << std::endl;
        ThreadPool pool;
    }

    BOOST_AUTO_TEST_CASE(test2)
    {
        std::cout << "test2" << std::endl;
        ThreadPool pool;

        auto task1 = std::bind(&calculate, 10, 5);
        boost::future<int> future1 = pool.submit(task1);

        auto task2 = std::bind(&do_some_thing);
        boost::future<void> future2 = pool.submit(task2);

        future1.then([](boost::future<int> future) {
            std::cout << "get result1: " << future.get() << std::endl;
        });

        std::cout << "test2 done" << std::endl;
    }

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(singleton_test) // name of the test suite

    BOOST_AUTO_TEST_CASE(test3) {
        Singleton<int>::getInstance().update(Key<int>(0), 123);
        Singleton<int>::getInstance().update(Key<int>(1), 234);
        Singleton<int>::getInstance().print();
    }

    BOOST_AUTO_TEST_CASE(test4) {
        Singleton<int>::getInstance().update(Key<int>(0), 123);
        int result = Singleton<int>::getInstance().findByID(Key<int>(0));

        Singleton<double>::getInstance().update(Key<double>(0), 1.23);
        double result2 = Singleton<double>::getInstance().findByID(Key<double>(0));
        BOOST_REQUIRE_EQUAL (123, result); // basic test
        BOOST_REQUIRE_EQUAL (1.23, result2); // basic test
    }

BOOST_AUTO_TEST_SUITE_END()