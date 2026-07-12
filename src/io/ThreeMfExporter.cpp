#include "io/ThreeMfExporter.h"
#include "io/MeshUtils.h"

#include <QFile>
#include <QByteArray>
#include <QDataStream>
#include <QDateTime>
#include <QDebug>

#include <TopoDS_Shape.hxx>

namespace
{

// ---- Minimal ZIP writer (stored entries only) ----
//
// The ZIP format for uncompressed entries is simple: per file a local
// header + raw bytes, then a central directory repeating the metadata,
// then an end-of-central-directory record. CRC-32 is the only computed
// field. Method 0 (stored) is accepted by every ZIP reader, including
// all 3MF consumers - compression buys little for these small XML files.

quint32 crc32(const QByteArray& data)
{
    static quint32 table[256];
    static bool initialized = false;
    if (!initialized)
    {
        for (quint32 i = 0; i < 256; ++i)
        {
            quint32 c = i;
            for (int k = 0; k < 8; ++k)
                c = (c & 1) ? (0xEDB88320u ^ (c >> 1)) : (c >> 1);
            table[i] = c;
        }
        initialized = true;
    }

    quint32 crc = 0xFFFFFFFFu;
    for (const char byte : data)
        crc = table[(crc ^ static_cast<quint8>(byte)) & 0xFFu] ^ (crc >> 8);
    return crc ^ 0xFFFFFFFFu;
}

struct ZipEntry
{
    QByteArray name;      // forward-slash path, no leading slash
    quint32 crc = 0;
    quint32 size = 0;
    quint32 offset = 0;   // of the local header
};

class ZipWriter
{
public:
    explicit ZipWriter(const QString& filePath)
        : m_file(filePath)
    {
        m_ok = m_file.open(QIODevice::WriteOnly | QIODevice::Truncate);
    }

    bool isOpen() const { return m_ok; }

    void addFile(const QByteArray& name, const QByteArray& content)
    {
        if (!m_ok)
            return;

        ZipEntry entry;
        entry.name = name;
        entry.crc = crc32(content);
        entry.size = static_cast<quint32>(content.size());
        entry.offset = static_cast<quint32>(m_file.pos());

        QDataStream out(&m_file);
        out.setByteOrder(QDataStream::LittleEndian);

        // Local file header
        out << quint32(0x04034b50);
        out << quint16(20);                 // version needed
        out << quint16(0);                  // flags
        out << quint16(0);                  // method: stored
        out << quint16(0) << quint16(0);    // dos time/date (zeroed)
        out << entry.crc;
        out << entry.size;                  // compressed size (== raw)
        out << entry.size;                  // uncompressed size
        out << quint16(entry.name.size());
        out << quint16(0);                  // extra length
        m_file.write(entry.name);
        m_file.write(content);

        m_entries.append(entry);
    }

    bool finish()
    {
        if (!m_ok)
            return false;

        const quint32 centralStart = static_cast<quint32>(m_file.pos());

        QDataStream out(&m_file);
        out.setByteOrder(QDataStream::LittleEndian);

        for (const ZipEntry& entry : m_entries)
        {
            // Central directory file header
            out << quint32(0x02014b50);
            out << quint16(20);             // version made by
            out << quint16(20);             // version needed
            out << quint16(0);              // flags
            out << quint16(0);              // method: stored
            out << quint16(0) << quint16(0);// dos time/date
            out << entry.crc;
            out << entry.size;
            out << entry.size;
            out << quint16(entry.name.size());
            out << quint16(0);              // extra
            out << quint16(0);              // comment
            out << quint16(0);              // disk number
            out << quint16(0);              // internal attrs
            out << quint32(0);              // external attrs
            out << entry.offset;
            m_file.write(entry.name);
        }

        const quint32 centralSize = static_cast<quint32>(m_file.pos()) - centralStart;

        // End of central directory
        out << quint32(0x06054b50);
        out << quint16(0) << quint16(0);    // disk numbers
        out << quint16(m_entries.size());
        out << quint16(m_entries.size());
        out << centralSize;
        out << centralStart;
        out << quint16(0);                  // comment length

        m_file.close();
        return true;
    }

private:
    QFile m_file;
    QList<ZipEntry> m_entries;
    bool m_ok = false;
};

// ---- 3MF package parts ----

QByteArray contentTypesXml()
{
    return
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        "<Types xmlns=\"http://schemas.openxmlformats.org/package/2006/content-types\">\n"
        " <Default Extension=\"rels\" ContentType=\"application/vnd.openxmlformats-package.relationships+xml\"/>\n"
        " <Default Extension=\"model\" ContentType=\"application/vnd.ms-package.3dmanufacturing-3dmodel+xml\"/>\n"
        "</Types>\n";
}

QByteArray relsXml()
{
    return
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        "<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">\n"
        " <Relationship Target=\"/3D/3dmodel.model\" Id=\"rel-1\" "
        "Type=\"http://schemas.microsoft.com/3dmanufacturing/2013/01/3dmodel\"/>\n"
        "</Relationships>\n";
}

QByteArray modelXml(const MeshUtils::Mesh& mesh, const QString& partName)
{
    QByteArray xml;
    xml.reserve(mesh.vertexCount() * 60 + mesh.triangleCount() * 40 + 1024);

    xml += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
           "<model unit=\"millimeter\" xml:lang=\"en-US\" "
           "xmlns=\"http://schemas.microsoft.com/3dmanufacturing/core/2015/02\">\n"
           " <metadata name=\"Title\">";
    xml += partName.toHtmlEscaped().toUtf8();
    xml += "</metadata>\n"
           " <metadata name=\"Application\">Infinity Creator</metadata>\n"
           " <resources>\n"
           "  <object id=\"1\" type=\"model\">\n"
           "   <mesh>\n"
           "    <vertices>\n";

    for (int i = 0; i < mesh.vertexCount(); ++i)
    {
        xml += "     <vertex x=\"";
        xml += QByteArray::number(mesh.vertices[i * 3], 'g', 12);
        xml += "\" y=\"";
        xml += QByteArray::number(mesh.vertices[i * 3 + 1], 'g', 12);
        xml += "\" z=\"";
        xml += QByteArray::number(mesh.vertices[i * 3 + 2], 'g', 12);
        xml += "\"/>\n";
    }

    xml += "    </vertices>\n"
           "    <triangles>\n";

    for (int i = 0; i < mesh.triangleCount(); ++i)
    {
        xml += "     <triangle v1=\"";
        xml += QByteArray::number(mesh.triangles[i * 3]);
        xml += "\" v2=\"";
        xml += QByteArray::number(mesh.triangles[i * 3 + 1]);
        xml += "\" v3=\"";
        xml += QByteArray::number(mesh.triangles[i * 3 + 2]);
        xml += "\"/>\n";
    }

    xml += "    </triangles>\n"
           "   </mesh>\n"
           "  </object>\n"
           " </resources>\n"
           " <build>\n"
           "  <item objectid=\"1\"/>\n"
           " </build>\n"
           "</model>\n";

    return xml;
}

} // namespace

namespace ThreeMfExporter
{

bool exportShape(const TopoDS_Shape& shape, const QString& filePath,
                 double deflection, const QString& partName)
{
    const MeshUtils::Mesh mesh = MeshUtils::buildMesh(shape, deflection);
    if (mesh.triangleCount() == 0)
    {
        qWarning() << "3MF export: empty mesh, aborting";
        return false;
    }

    ZipWriter zip(filePath);
    if (!zip.isOpen())
    {
        qWarning() << "3MF export: cannot open" << filePath;
        return false;
    }

    zip.addFile("[Content_Types].xml", contentTypesXml());
    zip.addFile("_rels/.rels", relsXml());
    zip.addFile("3D/3dmodel.model", modelXml(mesh, partName));

    const bool ok = zip.finish();
    qDebug() << "3MF export to" << filePath << "->" << (ok ? "ok" : "FAILED");
    return ok;
}

} // namespace ThreeMfExporter
