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
public:
    /// \brief constructor
    Event():id_(0){
        static_assert(std::is_trivially_copyable_v<Event>, "Not Allow C memory process");
    }
    
    //// \brief constructor
    Event(uint16_t id):id_(id){
        static_assert(std::is_trivially_copyable_v<Event>, "Not Allow C memory process");
    }

    /// \brief set_id
    /// - This method is used to set the id
    /// \param id - the set event id
    void set_id(uint16_t id)    { id_ = id; }

    /// \brief get_id
    /// - This method is used to get the id.
    /// \return the id of the event.
    uint16_t get_id()           { return id_;}

private:
    /// \brief id_
    /// - the id of event.
    uint16_t id_;
};

template<typename T>
class EventExtend:public Event
{
public:
    /// \brief constructor
    using Event::Event;

    /// \brief get_data
    /// - This method is used to get the data.
    /// \return the data of the event.
    T &get_data()
    {
        static_assert(std::is_trivially_copyable_v<EventExtend>, "Not Allow C memory process");
        return data_;
    }

private:
    /// \brief data_
    /// - the data of event.
    T data_;
};

template<int N>
struct Buffer
{
    uint8_t buffer[N];

    int size{N};

    uint8_t *get_buffer() { return buffer; }
};

using EventU8Message = EventExtend<uint8_t>;
using EventU16Message = EventExtend<uint16_t>;
using EventU32Message = EventExtend<uint32_t>;
using EventBufMessage = EventExtend<Buffer<32>>;
