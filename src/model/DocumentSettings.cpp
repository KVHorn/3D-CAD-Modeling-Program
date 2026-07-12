#include "model/DocumentSettings.h"
#include "app/AppSettings.h"

#include <QJsonValue>

Unit DocumentSettings::effectiveUnit() const
{
    return hasUnitOverride ? unitOverride : AppSettings::instance().unit();
}

double DocumentSettings::effectiveMeshDeflection() const
{
    return hasMeshDeflectionOverride ? meshDeflectionOverride
                                     : AppSettings::instance().meshDeflection();
}

QJsonObject DocumentSettings::toJson() const
{
    QJsonObject json;
    json[QStringLiteral("description")] = description;
    json[QStringLiteral("material")] = material;

    if (hasUnitOverride)
        json[QStringLiteral("unitOverride")] = Units::id(unitOverride);
    if (hasMeshDeflectionOverride)
        json[QStringLiteral("meshDeflectionOverride")] = meshDeflectionOverride;

    return json;
}

void DocumentSettings::fromJson(const QJsonObject& json)
{
    description = json[QStringLiteral("description")].toString();
    material = json[QStringLiteral("material")].toString();

    hasUnitOverride = json.contains(QStringLiteral("unitOverride"));
    if (hasUnitOverride)
        unitOverride = Units::fromId(json[QStringLiteral("unitOverride")].toString());

    hasMeshDeflectionOverride = json.contains(QStringLiteral("meshDeflectionOverride"));
    if (hasMeshDeflectionOverride)
        meshDeflectionOverride = json[QStringLiteral("meshDeflectionOverride")].toDouble(0.1);
}
