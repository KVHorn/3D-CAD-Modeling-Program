#ifndef DRESSUPFEATURES_H
#define DRESSUPFEATURES_H

#include "model/Feature.h"

// Dress-up features modify the existing solid rather than adding new bodies,
// so they have no boolean operation. Phase 2 versions apply to ALL edges of
// the solid; edge selection arrives with viewport picking in Phase 4.
class FilletFeature : public Feature
{
public:
    FilletFeature();
    bool supportsBooleanOp() const override { return false; }
    TopoDS_Shape apply(const TopoDS_Shape& current) const override;
};

class ChamferFeature : public Feature
{
public:
    ChamferFeature();
    bool supportsBooleanOp() const override { return false; }
    TopoDS_Shape apply(const TopoDS_Shape& current) const override;
};

#endif // DRESSUPFEATURES_H
