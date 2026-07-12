#ifndef STLEXPORTER_H
#define STLEXPORTER_H

#include <QString>

class TopoDS_Shape;

namespace StlExporter
{
    // Binary STL at the given tessellation deflection (mm).
    bool exportShape(const TopoDS_Shape& shape, const QString& filePath,
                     double deflection);
}

#endif // STLEXPORTER_H
