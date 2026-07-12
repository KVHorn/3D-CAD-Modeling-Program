#include "io/StlExporter.h"

#include <QDebug>

#include <TopoDS_Shape.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <StlAPI_Writer.hxx>
#include <Standard_Failure.hxx>

namespace StlExporter
{

bool exportShape(const TopoDS_Shape& shape, const QString& filePath,
                 double deflection)
{
    if (shape.IsNull())
        return false;

    try
    {
        BRepMesh_IncrementalMesh mesh(shape, deflection);

        StlAPI_Writer writer;
        writer.ASCIIMode() = Standard_False;  // binary: smaller, universal
        const bool ok = writer.Write(shape, filePath.toStdString().c_str());
        qDebug() << "STL export to" << filePath << "->" << (ok ? "ok" : "FAILED");
        return ok;
    }
    catch (const Standard_Failure& failure)
    {
        qCritical() << "STL export failed:" << failure.GetMessageString();
        return false;
    }
}

} // namespace StlExporter
