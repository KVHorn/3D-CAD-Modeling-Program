#ifndef UNDOSTACK_H
#define UNDOSTACK_H

#include <QUndoCommand>
#include <QJsonObject>

class PartModel;
class Feature;

// Undo/redo of model operations, one QUndoCommand per operation type. Each
// Document owns a QUndoStack of these; MainWindow's QUndoGroup routes
// Ctrl+Z / Ctrl+Y to whichever document has focus.
//
// Ownership rule: a feature belongs to the PartModel while it is in the
// history, and to the command while it is not (e.g. an added feature after
// undo, or a deleted feature awaiting possible redo... of its deletion's
// undo). The command's destructor cleans up whatever it currently owns.

// Insert a new feature at the end of the history.
class AddFeatureCommand : public QUndoCommand
{
public:
    AddFeatureCommand(PartModel* model, Feature* feature);
    ~AddFeatureCommand() override;

    void redo() override;
    void undo() override;

private:
    PartModel* m_model;
    Feature* m_feature;
    bool m_ownsFeature;  // true whenever the feature is out of the model
};

// Remove an existing feature (remembers its position for undo).
class DeleteFeatureCommand : public QUndoCommand
{
public:
    DeleteFeatureCommand(PartModel* model, Feature* feature);
    ~DeleteFeatureCommand() override;

    void redo() override;
    void undo() override;

private:
    PartModel* m_model;
    Feature* m_feature;
    int m_index = -1;
    bool m_ownsFeature;
};

// Any change to an existing feature's definition - parameter edits, rename,
// boolean-op change, suppress/unsuppress - captured as before/after JSON
// snapshots (which is everything a feature serializes).
class ChangeFeatureCommand : public QUndoCommand
{
public:
    ChangeFeatureCommand(PartModel* model, Feature* feature,
                         const QJsonObject& before, const QJsonObject& after,
                         const QString& text);

    void redo() override;
    void undo() override;

private:
    PartModel* m_model;
    Feature* m_feature;
    QJsonObject m_before;
    QJsonObject m_after;
};

#endif // UNDOSTACK_H
