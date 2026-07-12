#ifndef MESHUTILS_H
#define MESHUTILS_H

#include <QVector>

class TopoDS_Shape;

// Shared tessellation for the mesh exporters (STL, 3MF): triangulate the
// exact B-rep and collect an indexed triangle mesh in model coordinates.
namespace MeshUtils
{
    struct Mesh
    {
        // Flat xyz triples, millimeters.
        QVector<double> vertices;
        // Flat index triples into the vertex list, counter-clockwise
        // (outward normals).
        QVector<int> triangles;

        int vertexCount() const { return vertices.size() / 3; }
        int triangleCount() const { return triangles.size() / 3; }
    };

    // deflection = max distance the mesh may deviate from the true surface
    // (mm). Returns an empty mesh on failure.
    Mesh buildMesh(const TopoDS_Shape& shape, double deflection);
}

#endif // MESHUTILS_H
