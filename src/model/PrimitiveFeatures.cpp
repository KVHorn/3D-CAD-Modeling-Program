#include "model/PrimitiveFeatures.h"

#include <QtMath>

#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRepPrimAPI_MakeCone.hxx>
#include <BRepPrimAPI_MakeTorus.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAlgoAPI_Common.hxx>
#include <gp_Trsf.hxx>
#include <gp_Ax1.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <gp_Vec.hxx>
#include <Standard_Failure.hxx>

PrimitiveFeature::PrimitiveFeature(const QString& typeName, const QString& name)
    : Feature(typeName, name)
{
    // Placement shared by every primitive. Lengths in mm, angles in degrees.
    addParameter(QStringLiteral("x"), QStringLiteral("Position X"), 0.0);
    addParameter(QStringLiteral("y"), QStringLiteral("Position Y"), 0.0);
    addParameter(QStringLiteral("z"), QStringLiteral("Position Z"), 0.0);
    addParameter(QStringLiteral("rx"), QStringLiteral("Rotation X (deg)"), 0.0);
    addParameter(QStringLiteral("ry"), QStringLiteral("Rotation Y (deg)"), 0.0);
    addParameter(QStringLiteral("rz"), QStringLiteral("Rotation Z (deg)"), 0.0);
}

TopoDS_Shape PrimitiveFeature::placed() const
{
    const TopoDS_Shape primitive = buildPrimitive();

    // Compose translation * Rz * Ry * Rx (rotations about the origin,
    // applied X first). gp_Trsf multiplication reads right-to-left like
    // matrix composition.
    gp_Trsf rotX, rotY, rotZ, translation;
    rotX.SetRotation(gp_Ax1(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0)), qDegreesToRadians(value(QStringLiteral("rx"))));
    rotY.SetRotation(gp_Ax1(gp_Pnt(0, 0, 0), gp_Dir(0, 1, 0)), qDegreesToRadians(value(QStringLiteral("ry"))));
    rotZ.SetRotation(gp_Ax1(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), qDegreesToRadians(value(QStringLiteral("rz"))));
    translation.SetTranslation(gp_Vec(value(QStringLiteral("x")),
                                      value(QStringLiteral("y")),
                                      value(QStringLiteral("z"))));

    const gp_Trsf total = translation * rotZ * rotY * rotX;
    return BRepBuilderAPI_Transform(primitive, total, Standard_True).Shape();
}

TopoDS_Shape PrimitiveFeature::apply(const TopoDS_Shape& current) const
{
    const TopoDS_Shape shape = placed();

    if (current.IsNull() || booleanOp == BooleanOp::NewBody)
        return shape;

    switch (booleanOp)
    {
        case BooleanOp::Add:
        {
            BRepAlgoAPI_Fuse fuse(current, shape);
            if (!fuse.IsDone())
                throw Standard_Failure("Boolean fuse failed");
            return fuse.Shape();
        }
        case BooleanOp::Subtract:
        {
            BRepAlgoAPI_Cut cut(current, shape);
            if (!cut.IsDone())
                throw Standard_Failure("Boolean cut failed");
            return cut.Shape();
        }
        case BooleanOp::Intersect:
        {
            BRepAlgoAPI_Common common(current, shape);
            if (!common.IsDone())
                throw Standard_Failure("Boolean intersect failed");
            return common.Shape();
        }
        default:
            return shape;
    }
}

// ---- Box ----

BoxFeature::BoxFeature()
    : PrimitiveFeature(QStringLiteral("Box"), QStringLiteral("Box"))
{
    addParameter(QStringLiteral("dx"), QStringLiteral("Length (X)"), 50.0);
    addParameter(QStringLiteral("dy"), QStringLiteral("Width (Y)"), 50.0);
    addParameter(QStringLiteral("dz"), QStringLiteral("Height (Z)"), 50.0);
}

TopoDS_Shape BoxFeature::buildPrimitive() const
{
    return BRepPrimAPI_MakeBox(value(QStringLiteral("dx")),
                               value(QStringLiteral("dy")),
                               value(QStringLiteral("dz"))).Shape();
}

// ---- Cylinder ----

CylinderFeature::CylinderFeature()
    : PrimitiveFeature(QStringLiteral("Cylinder"), QStringLiteral("Cylinder"))
{
    addParameter(QStringLiteral("radius"), QStringLiteral("Radius"), 25.0);
    addParameter(QStringLiteral("height"), QStringLiteral("Height"), 50.0);
}

TopoDS_Shape CylinderFeature::buildPrimitive() const
{
    return BRepPrimAPI_MakeCylinder(value(QStringLiteral("radius")),
                                    value(QStringLiteral("height"))).Shape();
}

// ---- Sphere ----

SphereFeature::SphereFeature()
    : PrimitiveFeature(QStringLiteral("Sphere"), QStringLiteral("Sphere"))
{
    addParameter(QStringLiteral("radius"), QStringLiteral("Radius"), 25.0);
}

TopoDS_Shape SphereFeature::buildPrimitive() const
{
    return BRepPrimAPI_MakeSphere(value(QStringLiteral("radius"))).Shape();
}

// ---- Cone ----

ConeFeature::ConeFeature()
    : PrimitiveFeature(QStringLiteral("Cone"), QStringLiteral("Cone"))
{
    addParameter(QStringLiteral("radius1"), QStringLiteral("Bottom radius"), 25.0);
    addParameter(QStringLiteral("radius2"), QStringLiteral("Top radius"), 0.0);
    addParameter(QStringLiteral("height"), QStringLiteral("Height"), 50.0);
}

TopoDS_Shape ConeFeature::buildPrimitive() const
{
    return BRepPrimAPI_MakeCone(value(QStringLiteral("radius1")),
                                value(QStringLiteral("radius2")),
                                value(QStringLiteral("height"))).Shape();
}

// ---- Torus ----

TorusFeature::TorusFeature()
    : PrimitiveFeature(QStringLiteral("Torus"), QStringLiteral("Torus"))
{
    addParameter(QStringLiteral("majorRadius"), QStringLiteral("Major radius"), 40.0);
    addParameter(QStringLiteral("minorRadius"), QStringLiteral("Minor radius"), 10.0);
}

TopoDS_Shape TorusFeature::buildPrimitive() const
{
    return BRepPrimAPI_MakeTorus(value(QStringLiteral("majorRadius")),
                                 value(QStringLiteral("minorRadius"))).Shape();
}
