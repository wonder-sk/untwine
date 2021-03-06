/*****************************************************************************
 *   Copyright (c) 2020, Hobu, Inc. (info@hobu.co)                           *
 *                                                                           *
 *   All rights reserved.                                                    *
 *                                                                           *
 *   This program is free software; you can redistribute it and/or modify    *
 *   it under the terms of the GNU General Public License as published by    *
 *   the Free Software Foundation; either version 3 of the License, or       *
 *   (at your option) any later version.                                     *
 *                                                                           *
 ****************************************************************************/


#pragma once

namespace ept2
{

//Utterly trivial wrapper around a pointer.
class Point
{
public:
    Point() : m_data(nullptr)
    {}

    Point(uint8_t *data) : m_data(data)
    {}
    Point(char *data) : m_data(reinterpret_cast<uint8_t *>(data))
    {}

    uint8_t *data()
        { return m_data; }
    double x() const
        { return *ddata(); }
    double y() const
        { return *(ddata() + 1); }
    double z() const
        { return *(ddata() + 2); }

    char *cdata() const
        { return reinterpret_cast<char *>(m_data); }
    double *ddata() const
        { return reinterpret_cast<double *>(m_data); }

private:
    uint8_t *m_data;
};

} // namespace ept2
