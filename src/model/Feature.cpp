#include "model/Feature.h"

Feature::Feature(const QString& typeName, const QString& name)
    : name(name)
    , m_typeName(typeName)
{
}

double Feature::value(const QString& key) const
{
    for (const FeatureParameter& p : m_parameters)
    {
        if (p.key == key)
            return p.value;
    }
    return 0.0;
}

void Feature::setValue(const QString& key, double v)
{
    for (FeatureParameter& p : m_parameters)
    {
        if (p.key == key)
        {
            p.value = v;
            return;
        }
    }
}

void Feature::addParameter(const QString& key, const QString& label, double initialValue)
{
    m_parameters.append(FeatureParameter{ key, label, initialValue });
}

QJsonObject Feature::toJson() const
{
    QJsonObject json;
    json[QStringLiteral("type")] = m_typeName;
    json[QStringLiteral("name")] = name;
    json[QStringLiteral("suppressed")] = suppressed;
    json[QStringLiteral("booleanOp")] = static_cast<int>(booleanOp);

    QJsonObject params;
    for (const FeatureParameter& p : m_parameters)
        params[p.key] = p.value;
    json[QStringLiteral("parameters")] = params;

    return json;
}

void Feature::fromJson(const QJsonObject& json)
{
    name = json[QStringLiteral("name")].toString(name);
    suppressed = json[QStringLiteral("suppressed")].toBool(false);
    booleanOp = static_cast<BooleanOp>(json[QStringLiteral("booleanOp")].toInt(1));

    const QJsonObject params = json[QStringLiteral("parameters")].toObject();
    for (FeatureParameter& p : m_parameters)
    {
        if (params.contains(p.key))
            p.value = params[p.key].toDouble(p.value);
    }
}
