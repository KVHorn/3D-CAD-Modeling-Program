#ifndef FEATUREPARAMETER_H
#define FEATUREPARAMETER_H

#include <QString>

// One editable value on a feature: "dx" / "Length" / 50.0. The key is the
// stable identifier used in code and in .infinity files; the label is what
// the definition dialog shows. Units are millimeters for lengths and
// degrees for angles.
struct FeatureParameter
{
    QString key;
    QString label;
    double value = 0.0;
};

#endif // FEATUREPARAMETER_H
