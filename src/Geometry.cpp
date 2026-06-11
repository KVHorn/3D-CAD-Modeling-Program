#include "Geometry.h"

#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Face.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <StlAPI_Writer.hxx>

namespace Geometry
{

    TopoDS_Shape extrudeRectangle(double width, double height, double depth)
    {
        // Four corners of the rectangle, lying flat in the XY plane (Z = 0).
        gp_Pnt p1(0.0, 0.0, 0.0);
        gp_Pnt p2(width, 0.0, 0.0);
        gp_Pnt p3(width, height, 0.0);
        gp_Pnt p4(0.0, height, 0.0);

        // Four edges joining the corners into a closed loop.
        TopoDS_Edge e1 = BRepBuilderAPI_MakeEdge(p1, p2);
        TopoDS_Edge e2 = BRepBuilderAPI_MakeEdge(p2, p3);
        TopoDS_Edge e3 = BRepBuilderAPI_MakeEdge(p3, p4);
        TopoDS_Edge e4 = BRepBuilderAPI_MakeEdge(p4, p1);

        // Wire (closed boundary) -> face filling it.
        TopoDS_Wire wire = BRepBuilderAPI_MakeWire(e1, e2, e3, e4);
        TopoDS_Face face = BRepBuilderAPI_MakeFace(wire);

        // Extrude the face along +Z to produce a solid.
        gp_Vec extrusion(0.0, 0.0, depth);
        return BRepPrimAPI_MakePrism(face, extrusion).Shape();
    }

    bool exportStl(const TopoDS_Shape &shape, const std::string &filePath)
    {
        // Tessellate the precise B-rep into triangles. 0.1 is the deflection —
        // the max gap allowed between the mesh and the true surface; smaller = finer.
        BRepMesh_IncrementalMesh mesh(shape, 0.1);
        mesh.Perform();

        StlAPI_Writer writer;
        return writer.Write(shape, filePath.c_str());
    }

} // namespace Geometry