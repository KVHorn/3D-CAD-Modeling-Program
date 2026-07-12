#include "io/InfinityFormat.h"
#include "model/PartModel.h"
#include "model/Feature.h"
#include "model/FeatureFactory.h"
#include "model/DocumentSettings.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

namespace
{
    const auto FORMAT_ID = QStringLiteral("infinity-creator-part");
    constexpr int FORMAT_VERSION = 1;

    void setError(QString* errorMessage, const QString& text)
    {
        if (errorMessage)
            *errorMessage = text;
        qWarning() << "InfinityFormat:" << text;
    }
}

namespace InfinityFormat
{

bool save(const PartModel& model, const DocumentSettings& settings,
          const QString& filePath, QString* errorMessage)
{
    QJsonArray featureArray;
    for (const Feature* feature : model.features())
        featureArray.append(feature->toJson());

    QJsonObject root;
    root[QStringLiteral("format")] = FORMAT_ID;
    root[QStringLiteral("version")] = FORMAT_VERSION;
    root[QStringLiteral("settings")] = settings.toJson();
    root[QStringLiteral("features")] = featureArray;

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        setError(errorMessage, QStringLiteral("Cannot write %1: %2")
                                   .arg(filePath, file.errorString()));
        return false;
    }

    file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
    qDebug() << "Saved" << model.features().size() << "features to" << filePath;
    return true;
}

bool load(PartModel& model, DocumentSettings& settings,
          const QString& filePath, QString* errorMessage)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        setError(errorMessage, QStringLiteral("Cannot read %1: %2")
                                   .arg(filePath, file.errorString()));
        return false;
    }

    QJsonParseError parseError;
    const QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseError);
    if (doc.isNull() || !doc.isObject())
    {
        setError(errorMessage, QStringLiteral("Not a valid .infinity file: %1")
                                   .arg(parseError.errorString()));
        return false;
    }

    const QJsonObject root = doc.object();
    if (root[QStringLiteral("format")].toString() != FORMAT_ID)
    {
        setError(errorMessage, QStringLiteral("Not an Infinity Creator part file."));
        return false;
    }

    const int version = root[QStringLiteral("version")].toInt(0);
    if (version > FORMAT_VERSION)
    {
        setError(errorMessage,
                 QStringLiteral("This file was saved by a newer version of "
                                "Infinity Creator (format v%1).").arg(version));
        return false;
    }

    settings.fromJson(root[QStringLiteral("settings")].toObject());

    QList<Feature*> features;
    for (const QJsonValue& value : root[QStringLiteral("features")].toArray())
    {
        const QJsonObject featureJson = value.toObject();
        const QString type = featureJson[QStringLiteral("type")].toString();

        Feature* feature = FeatureFactory::create(type);
        if (!feature)
        {
            qDeleteAll(features);
            setError(errorMessage,
                     QStringLiteral("Unknown feature type \"%1\" in file.").arg(type));
            return false;
        }
        feature->fromJson(featureJson);
        features.append(feature);
    }

    model.setFeatures(features);  // takes ownership, single recompute
    qDebug() << "Loaded" << features.size() << "features from" << filePath;
    return true;
}

} // namespace InfinityFormat
