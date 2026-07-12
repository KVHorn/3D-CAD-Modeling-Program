#include "app/UndoStack.h"
#include "model/PartModel.h"
#include "model/Feature.h"

// ---- AddFeatureCommand ----

AddFeatureCommand::AddFeatureCommand(PartModel* model, Feature* feature)
    : QUndoCommand(QStringLiteral("Add %1").arg(feature->name))
    , m_model(model)
    , m_feature(feature)
    , m_ownsFeature(true)  // not yet in the model until first redo()
{
}

AddFeatureCommand::~AddFeatureCommand()
{
    if (m_ownsFeature)
        delete m_feature;
}

void AddFeatureCommand::redo()
{
    m_model->addFeature(m_feature);
    m_ownsFeature = false;
}

void AddFeatureCommand::undo()
{
    m_model->takeFeature(m_feature);
    m_ownsFeature = true;
}

// ---- DeleteFeatureCommand ----

DeleteFeatureCommand::DeleteFeatureCommand(PartModel* model, Feature* feature)
    : QUndoCommand(QStringLiteral("Delete %1").arg(feature->name))
    , m_model(model)
    , m_feature(feature)
    , m_ownsFeature(false)  // still in the model until first redo()
{
}

DeleteFeatureCommand::~DeleteFeatureCommand()
{
    if (m_ownsFeature)
        delete m_feature;
}

void DeleteFeatureCommand::redo()
{
    m_index = m_model->takeFeature(m_feature);
    m_ownsFeature = true;
}

void DeleteFeatureCommand::undo()
{
    m_model->insertFeature(m_index, m_feature);
    m_ownsFeature = false;
}

// ---- ChangeFeatureCommand ----

ChangeFeatureCommand::ChangeFeatureCommand(PartModel* model, Feature* feature,
                                           const QJsonObject& before,
                                           const QJsonObject& after,
                                           const QString& text)
    : QUndoCommand(text)
    , m_model(model)
    , m_feature(feature)
    , m_before(before)
    , m_after(after)
{
}

void ChangeFeatureCommand::redo()
{
    m_feature->fromJson(m_after);
    m_model->recompute();
}

void ChangeFeatureCommand::undo()
{
    m_feature->fromJson(m_before);
    m_model->recompute();
}
