#include "model/DressUpFeatures.h"

#include <BRepFilletAPI_MakeFillet.hxx>
#include <BRepFilletAPI_MakeChamfer.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <Standard_Failure.hxx>

FilletFeature::FilletFeature()
    : Feature(QStringLiteral("Fillet"), QStringLiteral("Fillet"))
{
    addParameter(QStringLiteral("radius"), QStringLiteral("Radius"), 2.0);
}

TopoDS_Shape FilletFeature::apply(const TopoDS_Shape& current) const
{
    if (current.IsNull())
        throw Standard_Failure("Fillet requires existing geometry");

    BRepFilletAPI_MakeFillet fillet(current);
    const double radius = value(QStringLiteral("radius"));

    for (TopExp_Explorer explorer(current, TopAbs_EDGE); explorer.More(); explorer.Next())
        fillet.Add(radius, TopoDS::Edge(explorer.Current()));

    fillet.Build();
    if (!fillet.IsDone())
        throw Standard_Failure("Fillet failed - radius may be too large for the geometry");

    return fillet.Shape();
}

ChamferFeature::ChamferFeature()
    : Feature(QStringLiteral("Chamfer"), QStringLiteral("Chamfer"))
{
    addParameter(QStringLiteral("distance"), QStringLiteral("Distance"), 2.0);
}

TopoDS_Shape ChamferFeature::apply(const TopoDS_Shape& current) const
{
    if (current.IsNull())
        throw Standard_Failure("Chamfer requires existing geometry");

    BRepFilletAPI_MakeChamfer chamfer(current);
    const double distance = value(QStringLiteral("distance"));

    for (TopExp_Explorer explorer(current, TopAbs_EDGE); explorer.More(); explorer.Next())
        chamfer.Add(distance, TopoDS::Edge(explorer.Current()));

    chamfer.Build();
    if (!chamfer.IsDone())
        throw Standard_Failure("Chamfer failed - distance may be too large for the geometry");

    return chamfer.Shape();
}
