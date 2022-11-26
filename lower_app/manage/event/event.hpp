//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      event.hpp
//
//  Purpose:
//     Interface for buffer envet translate.
//
// Author:
//      Alva Zhange
//
//  Assumptions:
//
//  Revision History:
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
    size_t size(){
        return sizeof(*this);
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

struct Buffer
{
    uint8_t buffer[256];
};

using EventU8Message = EventExtend<uint8_t>;
using EventU16Message = EventExtend<uint16_t>;
using EventU32Message = EventExtend<uint32_t>;
using EventBufMessage = EventExtend<Buffer>;