#include "app/AppSettings.h"

namespace
{
    const auto KEY_UNIT = QStringLiteral("units/display");
    const auto KEY_MESH = QStringLiteral("export/meshDeflection");
    const auto KEY_AUTHOR = QStringLiteral("user/authorName");
    const auto KEY_FIRSTRUN = QStringLiteral("app/firstRunDone");
    const auto KEY_RECENT = QStringLiteral("files/recent");
    constexpr int MAX_RECENT = 5;
}

AppSettings& AppSettings::instance()
{
    static AppSettings settings;
    return settings;
}

AppSettings::AppSettings()
    : m_settings(QStringLiteral("KVHorn"), QStringLiteral("InfinityCreator"))
{
}

Unit AppSettings::unit() const
{
    return Units::fromId(m_settings.value(KEY_UNIT, QStringLiteral("mm")).toString());
}

void AppSettings::setUnit(Unit unit)
{
    m_settings.setValue(KEY_UNIT, Units::id(unit));
}

double AppSettings::meshDeflection() const
{
    return m_settings.value(KEY_MESH, 0.1).toDouble();
}

void AppSettings::setMeshDeflection(double deflection)
{
    m_settings.setValue(KEY_MESH, deflection);
}

QString AppSettings::authorName() const
{
    return m_settings.value(KEY_AUTHOR).toString();
}

void AppSettings::setAuthorName(const QString& name)
{
    m_settings.setValue(KEY_AUTHOR, name);
}

bool AppSettings::firstRunDone() const
{
    return m_settings.value(KEY_FIRSTRUN, false).toBool();
}

void AppSettings::setFirstRunDone(bool done)
{
    m_settings.setValue(KEY_FIRSTRUN, done);
}

QStringList AppSettings::recentFiles() const
{
    return m_settings.value(KEY_RECENT).toStringList();
}

void AppSettings::addRecentFile(const QString& path)
{
    QStringList recent = recentFiles();
    recent.removeAll(path);
    recent.prepend(path);
    while (recent.size() > MAX_RECENT)
        recent.removeLast();
    m_settings.setValue(KEY_RECENT, recent);
}
