#include "model/FeatureFactory.h"
#include "model/PrimitiveFeatures.h"
#include "model/DressUpFeatures.h"

namespace FeatureFactory
{

Feature* create(const QString& typeName)
{
    if (typeName == QStringLiteral("Box"))      return new BoxFeature();
    if (typeName == QStringLiteral("Cylinder")) return new CylinderFeature();
    if (typeName == QStringLiteral("Sphere"))   return new SphereFeature();
    if (typeName == QStringLiteral("Cone"))     return new ConeFeature();
    if (typeName == QStringLiteral("Torus"))    return new TorusFeature();
    if (typeName == QStringLiteral("Fillet"))   return new FilletFeature();
    if (typeName == QStringLiteral("Chamfer"))  return new ChamferFeature();
    return nullptr;
}

QStringList primitiveTypes()
{
    return { QStringLiteral("Box"),
             QStringLiteral("Cylinder"),
             QStringLiteral("Sphere"),
             QStringLiteral("Cone"),
             QStringLiteral("Torus") };
}

QStringList dressUpTypes()
{
    return { QStringLiteral("Fillet"),
             QStringLiteral("Chamfer") };
}

} // namespace FeatureFactory
