#ifndef PRIMITIVEFEATURES_H
#define PRIMITIVEFEATURES_H

#include "model/Feature.h"

// Common base for solid primitives. Every primitive carries a placement
// (translation x/y/z in mm, rotation rx/ry/rz in degrees) and a boolean
// operation; apply() builds the primitive at the origin, transforms it into
// place, then combines it with the part built so far.
class PrimitiveFeature : public Feature
{
public:
    TopoDS_Shape apply(const TopoDS_Shape& current) const override;

protected:
    PrimitiveFeature(const QString& typeName, const QString& name);

    // Build the raw primitive at the origin. Dimension parameters are
    // validated by OCCT itself - bad values throw Standard_Failure
    // descendants, which recompute reports per feature.
    virtual TopoDS_Shape buildPrimitive() const = 0;

private:
    TopoDS_Shape placed() const;
};

class BoxFeature : public PrimitiveFeature
{
public:
    BoxFeature();
protected:
    TopoDS_Shape buildPrimitive() const override;
};

class CylinderFeature : public PrimitiveFeature
{
public:
    CylinderFeature();
protected:
    TopoDS_Shape buildPrimitive() const override;
};

class SphereFeature : public PrimitiveFeature
{
public:
    SphereFeature();
protected:
    TopoDS_Shape buildPrimitive() const override;
};

class ConeFeature : public PrimitiveFeature
{
public:
    ConeFeature();
protected:
    TopoDS_Shape buildPrimitive() const override;
};

class TorusFeature : public PrimitiveFeature
{
public:
    TorusFeature();
protected:
    TopoDS_Shape buildPrimitive() const override;
};

#endif // PRIMITIVEFEATURES_H
