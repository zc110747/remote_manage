//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      base.hpp
//
//  Purpose:
//
// Author:
//      @听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      12/19/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
_Pragma("once")

template<typename T>
class singleton
{
protected:
    singleton() = default;
    ~singleton() = default;

public:
    // 禁用拷贝和赋值
    singleton(const singleton&) = delete;
    singleton& operator=(const singleton&) = delete;

    /// \brief get_instance
    /// - This method is used to get the pattern of the class.
    /// \return the singleton pattern point of the object.
    static T* get_instance()
    {
        static T instance;
        return &instance;
    }
};
