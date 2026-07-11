#pragma once

#include <string>
#include <TopoDS_Shape.hxx>

// Infinity Creator — geometry layer.
// This is the ONLY place in the codebase that talks to OpenCASCADE.
// All UI and feature code calls through these functions, never OCCT directly.
namespace Geometry
{

    // Build a solid by extruding a (width x height) rectangle by 'depth' along +Z.
    TopoDS_Shape extrudeRectangle(double width, double height, double depth);

    // Tessellate a shape and write it to an STL file. Returns true on success.
    bool exportStl(const TopoDS_Shape &shape, const std::string &filePath);

} // namespace Geometry