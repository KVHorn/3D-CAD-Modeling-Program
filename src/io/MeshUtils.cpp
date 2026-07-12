#include "io/MeshUtils.h"

#include <QDebug>

#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <BRep_Tool.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <Poly_Triangulation.hxx>
#include <gp_Pnt.hxx>
#include <Standard_Failure.hxx>

namespace MeshUtils
{

Mesh buildMesh(const TopoDS_Shape& shape, double deflection)
{
    Mesh mesh;
    if (shape.IsNull())
        return mesh;

    try
    {
        BRepMesh_IncrementalMesh mesher(shape, deflection);

        for (TopExp_Explorer explorer(shape, TopAbs_FACE); explorer.More(); explorer.Next())
        {
            const TopoDS_Face face = TopoDS::Face(explorer.Current());

            TopLoc_Location location;
            Handle(Poly_Triangulation) triangulation =
                BRep_Tool::Triangulation(face, location);
            if (triangulation.IsNull())
                continue;

            const int vertexOffset = mesh.vertexCount();
            const gp_Trsf transform = location.Transformation();

            for (int i = 1; i <= triangulation->NbNodes(); ++i)
            {
                const gp_Pnt point = triangulation->Node(i).Transformed(transform);
                mesh.vertices.append(point.X());
                mesh.vertices.append(point.Y());
                mesh.vertices.append(point.Z());
            }

            // A REVERSED face's triangles wind the wrong way for outward
            // normals - swap two indices to flip them back.
            const bool reversed = (face.Orientation() == TopAbs_REVERSED);

            for (int i = 1; i <= triangulation->NbTriangles(); ++i)
            {
                int n1, n2, n3;
                triangulation->Triangle(i).Get(n1, n2, n3);
                if (reversed)
                    std::swap(n2, n3);

                mesh.triangles.append(vertexOffset + n1 - 1);
                mesh.triangles.append(vertexOffset + n2 - 1);
                mesh.triangles.append(vertexOffset + n3 - 1);
            }
        }
    }
    catch (const Standard_Failure& failure)
    {
        qCritical() << "MeshUtils::buildMesh failed:" << failure.GetMessageString();
        mesh = Mesh();
    }

    qDebug() << "buildMesh:" << mesh.vertexCount() << "vertices,"
             << mesh.triangleCount() << "triangles at deflection" << deflection;
    return mesh;
}

} // namespace MeshUtils
