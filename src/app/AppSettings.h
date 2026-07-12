#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QSettings>
#include <QStringList>

#include "model/Units.h"

// Application-wide settings (Edit > Options), persisted via QSettings.
// Per-document overrides live in DocumentSettings and travel inside the
// .infinity file - see docs/SPEC.md "Settings model".
class AppSettings
{
public:
    static AppSettings& instance();

    Unit unit() const;
    void setUnit(Unit unit);

    // Linear deflection for export tessellation, in mm.
    double meshDeflection() const;
    void setMeshDeflection(double deflection);

    QString authorName() const;
    void setAuthorName(const QString& name);

    bool firstRunDone() const;
    void setFirstRunDone(bool done);

    QStringList recentFiles() const;
    void addRecentFile(const QString& path);

private:
    AppSettings();
    QSettings m_settings;
};

#endif // APPSETTINGS_H
