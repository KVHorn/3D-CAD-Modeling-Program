#include "model/PartModel.h"
#include "model/Feature.h"

#include <QDebug>

#include <Standard_Failure.hxx>

PartModel::PartModel(QObject* parent)
    : QObject(parent)
{
}

PartModel::~PartModel()
{
    qDeleteAll(m_features);
}

void PartModel::addFeature(Feature* feature)
{
    m_features.append(feature);
    recompute();
}

void PartModel::insertFeature(int index, Feature* feature)
{
    index = qBound(0, index, static_cast<int>(m_features.size()));
    m_features.insert(index, feature);
    recompute();
}

int PartModel::takeFeature(Feature* feature)
{
    const int index = m_features.indexOf(feature);
    if (index >= 0)
    {
        m_features.removeAt(index);
        recompute();
    }
    return index;
}

void PartModel::removeFeature(Feature* feature)
{
    if (m_features.removeOne(feature))
    {
        delete feature;
        recompute();
    }
}

bool PartModel::recompute()
{
    m_result.Nullify();
    m_failedFeature = nullptr;
    m_lastError.clear();
    bool ok = true;

    for (Feature* feature : m_features)
    {
        if (feature->suppressed)
            continue;

        try
        {
            m_result = feature->apply(m_result);
        }
        catch (const Standard_Failure& failure)
        {
            m_failedFeature = feature;
            m_lastError = QStringLiteral("%1: %2")
                .arg(feature->name, QString::fromUtf8(failure.GetMessageString()));
            qWarning() << "Recompute failed at" << m_lastError;
            ok = false;
            break;  // keep the result as built up to the failing feature
        }
    }

    qDebug() << "Recompute:" << m_features.size() << "features, ok =" << ok;
    emit recomputed();
    return ok;
}
