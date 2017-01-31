/************************************************************************************
*                                                                                   *
*   Copyright (c) 2014, 2015 - 2016 Axel Menzel <info@rttr.org>                     *
*                                                                                   *
*   This file is part of RTTR (Run Time Type Reflection)                            *
*   License: MIT License                                                            *
*                                                                                   *
*   Permission is hereby granted, free of charge, to any person obtaining           *
*   a copy of this software and associated documentation files (the "Software"),    *
*   to deal in the Software without restriction, including without limitation       *
*   the rights to use, copy, modify, merge, publish, distribute, sublicense,        *
*   and/or sell copies of the Software, and to permit persons to whom the           *
*   Software is furnished to do so, subject to the following conditions:            *
*                                                                                   *
*   The above copyright notice and this permission notice shall be included in      *
*   all copies or substantial portions of the Software.                             *
*                                                                                   *
*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR      *
*   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,        *
*   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE     *
*   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER          *
*   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,   *
*   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE   *
*   SOFTWARE.                                                                       *
*                                                                                   *
*************************************************************************************/

#ifndef RTTR_ASSOCIATIVE_MAPPER_IMPL_H_
#define RTTR_ASSOCIATIVE_MAPPER_IMPL_H_

#include "rttr/detail/base/core_prerequisites.h"
#include "rttr/detail/misc/iterator_wrapper.h"

#include "rttr/variant.h"
#include <type_traits>

#include <set>
#include <map>
#include <unordered_map>
#include <unordered_set>

namespace rttr
{
namespace detail
{

template<typename T, typename ConstType, typename Tp = typename std::conditional<std::is_const<ConstType>::value,
                                                                                 typename T::const_iterator,
                                                                                 typename T::iterator>::type>
struct associative_container_base : detail::iterator_wrapper_associative_container<Tp>
{
    static void begin(void* container, detail::iterator_data& itr)
    {
        associative_container_mapper<T, ConstType>::create(itr, reinterpret_cast<ConstType*>(container)->begin());
    }

    static void end(void* container, detail::iterator_data& itr)
    {
        associative_container_mapper<T, ConstType>::create(itr, reinterpret_cast<ConstType*>(container)->end());
    }

    static std::size_t get_size(void* container)
    {
        return (reinterpret_cast<ConstType*>(container)->size());
    }

    static void find(void* container, detail::iterator_data& itr, argument& key)
    {
        using key_t = typename T::key_type;
        if (key.get_type() == ::rttr::type::get<key_t>())
            associative_container_mapper<T, ConstType>::create(itr, reinterpret_cast<ConstType*>(container)->find(key.get_value<key_t>()));
        else
            end(container, itr);
    }

    static void equal_range(void* container, argument& key,
                            detail::iterator_data& itr_begin, detail::iterator_data& itr_end)
    {
        using key_t = typename T::key_type;
        if (key.get_type() == ::rttr::type::get<key_t>())
        {
            auto ret = reinterpret_cast<ConstType*>(container)->equal_range(key.get_value<key_t>());
            associative_container_mapper<T, ConstType>::create(itr_begin, ret.first);
            associative_container_mapper<T, ConstType>::create(itr_end, ret.second);
        }
        else
        {
            end(container, itr_begin);
            end(container, itr_end);
        }
    }
};

} // end namespace detail

//////////////////////////////////////////////////////////////////////////////////////

template<typename K, typename...Args>
struct associative_container_mapper<std::set<K>, Args...> : detail::associative_container_base<std::set<K>,  Args...>
{
    static variant get_key(const detail::iterator_data& itr)
    {
        return variant();
    }

    static variant get_value(const detail::iterator_data& itr)
    {
        auto& it = detail::associative_container_base<std::set<K>,  Args...>::get_iterator(itr);
        return variant(std::ref(*it));
    }
};

//////////////////////////////////////////////////////////////////////////////////////

template<typename K, typename...Args>
struct associative_container_mapper<std::multiset<K>, Args...> : detail::associative_container_base<std::multiset<K>,  Args...>
{
    static variant get_key(const detail::iterator_data& itr)
    {
        return variant();
    }

    static variant get_value(const detail::iterator_data& itr)
    {
        auto& it = detail::associative_container_base<std::multiset<K>,  Args...>::get_iterator(itr);
        return variant(std::ref(*it));
    }
};

//////////////////////////////////////////////////////////////////////////////////////

template<typename K, typename T, typename...Args>
struct associative_container_mapper<std::map<K, T>, Args...> : detail::associative_container_base<std::map<K, T>, Args...> { };

//////////////////////////////////////////////////////////////////////////////////////

template<typename K, typename T, typename...Args>
struct associative_container_mapper<std::multimap<K, T>, Args...> : detail::associative_container_base<std::multimap<K, T>,  Args...> {};

//////////////////////////////////////////////////////////////////////////////////////

template<typename K, typename...Args>
struct associative_container_mapper<std::unordered_set<K>, Args...> : detail::associative_container_base<std::unordered_set<K>,  Args...>
{
    static variant get_key(const detail::iterator_data& itr)
    {
        return variant();
    }

    static variant get_value(const detail::iterator_data& itr)
    {
        auto& it = detail::associative_container_base<std::unordered_set<K>,  Args...>::get_iterator(itr);
        return variant(std::ref(*it));
    }
};


//////////////////////////////////////////////////////////////////////////////////////

template<typename K, typename T, typename...Args>
struct associative_container_mapper<std::unordered_map<K, T>, Args...> : detail::associative_container_base<std::unordered_map<K, T>,  Args...> {};

//////////////////////////////////////////////////////////////////////////////////////

template<typename K, typename...Args>
struct associative_container_mapper<std::unordered_multiset<K>, Args...> : detail::associative_container_base<std::unordered_multiset<K>,  Args...>
{
    static variant get_key(const detail::iterator_data& itr)
    {
        return variant();
    }

    static variant get_value(const detail::iterator_data& itr)
    {
        auto& it = detail::associative_container_base<std::unordered_multiset<K>,  Args...>::get_iterator(itr);
        return variant(std::ref(*it));
    }
};

//////////////////////////////////////////////////////////////////////////////////////

template<typename K, typename T, typename...Args>
struct associative_container_mapper<std::unordered_multimap<K, T>, Args...> : detail::associative_container_base<std::unordered_multimap<K, T>,  Args...> {};

//////////////////////////////////////////////////////////////////////////////////////

namespace detail
{

struct associative_container_empty
{
    static void create(iterator_data& itr_tgt, const iterator_data& src)
    {
    }

    static void advance(iterator_data& itr, std::ptrdiff_t idx)
    {
    }

    static void destroy(iterator_data& itr)
    {
    }

    static bool equal(const iterator_data& lhs_itr, const iterator_data& rhs_itr) RTTR_NOEXCEPT
    {
        return false;
    }

    static variant get_key(const iterator_data& itr)
    {
        return variant();
    }

    static variant get_value(const iterator_data& itr)
    {
        return variant();
    }

    static void begin(void* container, iterator_data& itr)
    {

    }

    static std::size_t get_size(void* container)
    {
        return 0;
    }

    static void find(void* container, detail::iterator_data& itr, argument& arg)
    {
    }
    static void equal_range(void* container, argument& key,
                            detail::iterator_data& itr_begin, detail::iterator_data& itr_end)
    {

    }
};

} // end namespace detail
} // end namespace rttr

#endif // RTTR_ASSOCIATIVE_MAPPER_H_
