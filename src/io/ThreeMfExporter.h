#ifndef THREEMFEXPORTER_H
#define THREEMFEXPORTER_H

#include <QString>

class TopoDS_Shape;

// 3MF export. A .3mf file is an OPC package: a ZIP archive containing an
// XML mesh description. The ZIP container is written by a small built-in
// writer (stored entries, no compression) so no external zip library is
// needed - every 3MF consumer accepts stored entries.
namespace ThreeMfExporter
{
    bool exportShape(const TopoDS_Shape& shape, const QString& filePath,
                     double deflection, const QString& partName);
}

#endif // THREEMFEXPORTER_H
