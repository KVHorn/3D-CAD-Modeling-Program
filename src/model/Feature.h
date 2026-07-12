#ifndef FEATURE_H
#define FEATURE_H

#include <QString>
#include <QVector>
#include <QJsonObject>

#include <TopoDS_Shape.hxx>

#include "model/FeatureParameter.h"
#include "model/BooleanOp.h"

// One node in the part's history - the equivalent of one entry in CATIA's
// specification tree. A feature takes the part as built so far and returns
// the part with itself applied. Recompute is just replaying every feature
// in order; that replay is what makes the model parametric.
class Feature
{
public:
    Feature(const QString& typeName, const QString& name);
    virtual ~Feature() = default;

    // Stable type identifier ("Box", "Cylinder", ...) - used by the
    // FeatureFactory and by the .infinity file format.
    const QString& typeName() const { return m_typeName; }

    QString name;
    bool suppressed = false;
    BooleanOp booleanOp = BooleanOp::Add;

    // Whether the definition dialog should offer the boolean combo.
    // Primitives do; dress-up features (fillet/chamfer) won't.
    virtual bool supportsBooleanOp() const { return true; }

    QVector<FeatureParameter>& parameters() { return m_parameters; }
    const QVector<FeatureParameter>& parameters() const { return m_parameters; }

    double value(const QString& key) const;
    void setValue(const QString& key, double v);

    // Apply this feature to the part built so far and return the result.
    // Throws Standard_Failure (or a descendant) on geometric failure;
    // PartModel::recompute catches and reports it.
    virtual TopoDS_Shape apply(const TopoDS_Shape& current) const = 0;

    // Serialization for the .infinity format (used in the I/O phase, but
    // defined here so every feature carries it from birth).
    virtual QJsonObject toJson() const;
    virtual void fromJson(const QJsonObject& json);

protected:
    void addParameter(const QString& key, const QString& label, double initialValue);

private:
    QString m_typeName;
    QVector<FeatureParameter> m_parameters;
};

#endif // FEATURE_H
