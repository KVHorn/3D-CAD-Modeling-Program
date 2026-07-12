#ifndef FEATUREFACTORY_H
#define FEATUREFACTORY_H

#include <QString>
#include <QStringList>

class Feature;

// Maps stable type names ("Box", "Fillet", ...) to fresh instances. Used by
// the UI to create features and by the .infinity loader to reconstruct a
// saved history.
namespace FeatureFactory
{
    Feature* create(const QString& typeName);
    QStringList primitiveTypes();
    QStringList dressUpTypes();
}

#endif // FEATUREFACTORY_H
