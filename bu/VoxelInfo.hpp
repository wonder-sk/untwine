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

#include <array>
#include <unordered_map>

#include "../common/FileDimInfo.hpp"
#include "../common/GridKey.hpp"
#include "../common/Point.hpp"
#include "../common/VoxelKey.hpp"

#include "OctantInfo.hpp"

namespace ept2
{
namespace bu
{

class VoxelInfo
{
public:
    // This probably needs a data structure geared for sparse data as I *think* that many
    // of our lookups will fail.
    using Grid = std::unordered_map<GridKey, int>;

    VoxelInfo(const pdal::BOX3D& fullBounds, const VoxelKey& k) :
        m_fullBounds(fullBounds), m_octant(k)
    {
        //ABELL - This shouldn't be necessary. The key should be in the children
        //  when they're pulled out of the queue.
        for (int i = 0; i < 8; ++i)
            m_children[i].setKey(k.child(i));

        int cells = std::pow(2, key().level());
        m_xWidth = (fullBounds.maxx - fullBounds.minx) / cells;
        m_yWidth = (fullBounds.maxy - fullBounds.miny) / cells;
        m_zWidth = (fullBounds.maxz - fullBounds.minz) / cells;
        // Calculate the bounds of this voxel.
        m_bounds.minx = fullBounds.minx + (k.x() * m_xWidth);
        m_bounds.maxx = m_bounds.minx + m_xWidth;
        m_bounds.miny = fullBounds.miny + (k.y() * m_yWidth);
        m_bounds.maxy = m_bounds.miny + m_yWidth;
        m_bounds.minz = fullBounds.minz + (k.z() * m_zWidth);
        m_bounds.maxz = m_bounds.minz + m_zWidth;

        // Determine spacing between points.
//        m_spacing = minWidth() / 128.0;
        m_spacing = maxWidth() / 128.0;
        m_squareSpacing = m_spacing * m_spacing;

        static const double sqrt3 = std::sqrt(3);
        m_gridCellWidth = m_spacing / sqrt3;
        m_gridXCount = std::ceil((m_bounds.maxx - m_bounds.minx) / m_gridCellWidth);
        m_gridYCount = std::ceil((m_bounds.maxy - m_bounds.miny) / m_gridCellWidth);
        m_gridZCount = std::ceil((m_bounds.maxz - m_bounds.minz) / m_gridCellWidth);
    }

    VoxelKey key() const
        { return m_octant.key(); }

    OctantInfo& operator[](int dir)
        { return m_children[dir]; }

    OctantInfo& octant()
        { return m_octant; }

    size_t numPoints() const
    {
        size_t cnt = 0;
        for (const OctantInfo& oi : m_children)
            cnt += oi.numPoints();
        return cnt;
    }

    bool hasPoints() const
    {
        for (const OctantInfo& oi : m_children)
            if (oi.hasPoints())
                return true;
        return false;
    }

    double spacing() const
        { return m_spacing; }

    double squareSpacing() const
        { return m_squareSpacing; }

    double minWidth() const
        { return std::min(std::min(m_xWidth, m_yWidth), m_zWidth); }

    double maxWidth() const
        { return std::max(std::max(m_xWidth, m_yWidth), m_zWidth); }

    double xWidth() const
        { return m_xWidth; }

    double yWidth() const
        { return m_yWidth; }

    double zWidth() const
        { return m_zWidth; }

    GridKey gridKey(const Point& p) const
    {
        double x = p.x() - m_bounds.minx;
        double y = p.y() - m_bounds.miny;
        double z = p.z() - m_bounds.minz;

        return GridKey(x / m_gridCellWidth, y / m_gridCellWidth, z / m_gridCellWidth);
    }

    //ABELL - Really torn WRT making Grid its own thing.

    Grid& grid()
        { return m_grid; }

    int gridXCount() const
        { return m_gridXCount; }

    int gridYCount() const
        { return m_gridYCount; }

    int gridZCount() const
        { return m_gridZCount; }

private:
    pdal::BOX3D m_fullBounds;
    pdal::BOX3D m_bounds;
    double m_xWidth;
    double m_yWidth;
    double m_zWidth;
    double m_gridCellWidth;
    int m_gridXCount;
    int m_gridYCount;
    int m_gridZCount;
    std::array<OctantInfo, 8> m_children;
    OctantInfo m_octant;
    double m_spacing;
    double m_squareSpacing;

    Grid m_grid;
};

} // namespace bu
} // namespace ept2
