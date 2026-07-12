#ifndef UNITS_H
#define UNITS_H

#include <QString>

// Unit policy: the model and the .infinity format store millimeters,
// always. Inches are a display/entry conversion at the dialog boundary
// only - so files are portable between users with different settings.
enum class Unit
{
    Millimeters = 0,
    Inches = 1
};

namespace Units
{
    constexpr double MM_PER_INCH = 25.4;

    inline double toDisplay(double mm, Unit unit)
    {
        return unit == Unit::Inches ? mm / MM_PER_INCH : mm;
    }

    inline double fromDisplay(double value, Unit unit)
    {
        return unit == Unit::Inches ? value * MM_PER_INCH : value;
    }

    inline QString suffix(Unit unit)
    {
        return unit == Unit::Inches ? QStringLiteral(" in") : QStringLiteral(" mm");
    }

    inline QString name(Unit unit)
    {
        return unit == Unit::Inches ? QStringLiteral("Inches") : QStringLiteral("Millimeters");
    }

    inline QString id(Unit unit)
    {
        return unit == Unit::Inches ? QStringLiteral("inch") : QStringLiteral("mm");
    }

    inline Unit fromId(const QString& id, Unit fallback = Unit::Millimeters)
    {
        if (id == QStringLiteral("inch")) return Unit::Inches;
        if (id == QStringLiteral("mm"))   return Unit::Millimeters;
        return fallback;
    }

    // Sensible spin-box precision per unit (0.001 mm vs 0.0001 in).
    inline int decimals(Unit unit) { return unit == Unit::Inches ? 4 : 3; }
}

#endif // UNITS_H
