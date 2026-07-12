#ifndef PARTMODEL_H
#define PARTMODEL_H

#include <QObject>
#include <QList>
#include <QString>

#include <TopoDS_Shape.hxx>

class Feature;

// The parametric heart of a document: an ordered feature history and the
// recompute engine that replays it - the in-memory contents of the
// specification tree. Owns its features (except while a feature is "taken"
// by an undo command).
class PartModel : public QObject
{
    Q_OBJECT

public:
    explicit PartModel(QObject* parent = nullptr);
    ~PartModel() override;

    // Append (takes ownership) and recompute.
    void addFeature(Feature* feature);

    // Insert at a specific history position (takes ownership) and recompute.
    // Used by undo/redo to restore a deleted feature exactly where it was.
    void insertFeature(int index, Feature* feature);

    // Remove WITHOUT deleting and recompute; returns the history index it
    // occupied (-1 if absent). Ownership passes to the caller (undo stack).
    int takeFeature(Feature* feature);

    // Remove AND delete, then recompute.
    void removeFeature(Feature* feature);

    const QList<Feature*>& features() const { return m_features; }
    int indexOf(Feature* feature) const { return m_features.indexOf(feature); }
    bool isEmpty() const { return m_features.isEmpty(); }

    const TopoDS_Shape& resultShape() const { return m_result; }

    // Failure state: instead of rolling failed features back, the model
    // keeps them, marks them (shown red in the tree), and builds the part
    // up to the failure - CATIA's behavior. The user edits or deletes the
    // failing feature, or undoes.
    Feature* failedFeature() const { return m_failedFeature; }
    const QString& lastError() const { return m_lastError; }

    // Replay the entire history in order, skipping suppressed features.
    bool recompute();

signals:
    void recomputed();

private:
    QList<Feature*> m_features;
    TopoDS_Shape m_result;
    Feature* m_failedFeature = nullptr;
    QString m_lastError;
};

#endif // PARTMODEL_H
