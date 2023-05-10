//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      event.hpp
//
//  Purpose:
//      提供线程/进程间通讯的数据结构，需要满足copyable
//
// Author:
//     @听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      12/19/2022   Create New Version	
/////////////////////////////////////////////////////////////////////////////
_Pragma("once")

#include <cinttypes>
#include <type_traits>

class Event
{
private:
    uint16_t _id;

public:
    Event():_id(0){
        static_assert(std::is_trivially_copyable_v<Event>, "Not Allow C memory process");
    }
    Event(uint16_t id):_id(id){
        static_assert(std::is_trivially_copyable_v<Event>, "Not Allow C memory process");
    }

    void setId(uint16_t id){
        _id = id;
    }
    uint16_t getId(){
        return _id;
    }
};

template<typename T>
class EventExtend:public Event
{
private:
    T _data;

public:
    using Event::Event;
    T &getData()
    {
        static_assert(std::is_trivially_copyable_v<EventExtend>, "Not Allow C memory process");
        return _data;
    }
};

template<int N>
struct Buffer
{
    uint8_t buffer[N];
};

using EventU8Message = EventExtend<uint8_t>;
using EventU16Message = EventExtend<uint16_t>;
using EventU32Message = EventExtend<uint32_t>;
using EventBufMessage = EventExtend<Buffer<16>>;