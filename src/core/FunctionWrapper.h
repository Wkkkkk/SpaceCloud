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

#ifndef HELLOWORLD_FUNCTION_WRAPPER_H
#define HELLOWORLD_FUNCTION_WRAPPER_H

namespace core {
class FunctionWrapper {
    struct impl_base {
        virtual void call() = 0;
        virtual ~impl_base() = default;
    };

    std::unique_ptr<impl_base> impl;
    bool is_valid = false;
    template <typename F>
    struct impl_type : impl_base
    {
        F f;
        explicit impl_type(F&& f_) : f(std::move(f_)) {}
        void call() final { f(); }
    };

public:
    FunctionWrapper() = default;
    template <typename F>
    FunctionWrapper(F&& f)
            : impl(new impl_type<F>(std::forward<F>(f))),
              is_valid(true) {}

    void operator() () { impl->call(); }

    bool valid() const { return is_valid; }
    FunctionWrapper(FunctionWrapper&& other) noexcept
            : impl(std::move(other.impl)),
              is_valid(true) {}

    FunctionWrapper&operator=(FunctionWrapper&& other) noexcept {
        impl = std::move(other.impl);
        is_valid = other.is_valid;
        return *this;
    }

    FunctionWrapper(const FunctionWrapper&) = delete;
    FunctionWrapper(FunctionWrapper&) = delete;
    FunctionWrapper&operator=(const FunctionWrapper&) = delete;
};
}

#endif //HELLOWORLD_FUNCTION_WRAPPER_H
