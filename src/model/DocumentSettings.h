#ifndef DOCUMENTSETTINGS_H
#define DOCUMENTSETTINGS_H

#include <QString>
#include <QJsonObject>

#include "model/Units.h"

// Per-file settings (File > Document Properties), stored inside the
// .infinity file. Every value defaults to "inherit from application
// settings" with an optional override - clearing an override resumes
// tracking the app default. See docs/SPEC.md "Settings model".
class DocumentSettings
{
public:
    // Descriptive fields (feed the drafting title block later).
    QString description;
    QString material;

    // Overrides: hasX == false means "inherit".
    bool hasUnitOverride = false;
    Unit unitOverride = Unit::Millimeters;

    bool hasMeshDeflectionOverride = false;
    double meshDeflectionOverride = 0.1;

    // Effective values = override if present, else the application setting.
    Unit effectiveUnit() const;
    double effectiveMeshDeflection() const;

    QJsonObject toJson() const;
    void fromJson(const QJsonObject& json);
};

#endif // DOCUMENTSETTINGS_H
