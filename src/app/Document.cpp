#include "app/Document.h"
#include "viewport/OcctViewport.h"
#include "model/PartModel.h"
#include "model/Feature.h"
#include "model/FeatureFactory.h"
#include "app/UndoStack.h"
#include "ui/ParameterDialog.h"

#include <QVBoxLayout>
#include <QUndoStack>
#include <QMessageBox>
#include <QDebug>

#include <BRepMesh_IncrementalMesh.hxx>
#include <StlAPI_Writer.hxx>
#include <Standard_Failure.hxx>

Document::Document(const QString& name, QWidget* parent)
    : QWidget(parent)
    , m_name(name)
    , m_viewport(new OcctViewport(this))
    , m_model(new PartModel(this))
    , m_undoStack(new QUndoStack(this))
    , m_currentWorkspace(WorkspaceType::PartDesign)
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_viewport);

    setMinimumSize(400, 300);
    setWindowTitle(m_name);

    connect(m_model, &PartModel::recomputed, this, &Document::refreshDisplay);
}

bool Document::isEmpty() const
{
    return m_model->isEmpty();
}

bool Document::hasShape() const
{
    return !m_model->resultShape().IsNull();
}

void Document::insertFeatureInteractive(const QString& typeName, QWidget* dialogParent)
{
    Feature* feature = FeatureFactory::create(typeName);
    if (!feature)
    {
        qWarning() << "Unknown feature type" << typeName;
        return;
    }

    // Sensible default: the first feature starts a body, later ones fuse.
    feature->booleanOp = isEmpty() ? BooleanOp::NewBody : BooleanOp::Add;

    ParameterDialog dialog(feature, dialogParent ? dialogParent : this);
    if (dialog.exec() != QDialog::Accepted)
    {
        delete feature;
        return;
    }

    m_undoStack->push(new AddFeatureCommand(m_model, feature));

    // Failed features stay in the tree, marked red (CATIA behavior) -
    // the user edits, deletes, or undoes. Just say why it failed.
    if (m_model->failedFeature() == feature)
    {
        QMessageBox::warning(this, QStringLiteral("Feature failed"),
            QStringLiteral("The feature could not be built:\n\n%1\n\n"
                           "It is marked red in the specification tree - "
                           "edit its definition, delete it, or undo.")
                .arg(m_model->lastError()));
    }
}

void Document::editFeature(Feature* feature)
{
    const QJsonObject before = feature->toJson();

    ParameterDialog dialog(feature, this);
    if (dialog.exec() != QDialog::Accepted)
        return;  // dialog writes only on OK, so nothing changed

    const QJsonObject after = feature->toJson();

    // Restore, then let the command apply - so redo/undo are symmetric and
    // the recompute happens exactly once, inside the command.
    feature->fromJson(before);
    m_undoStack->push(new ChangeFeatureCommand(m_model, feature, before, after,
        QStringLiteral("Edit %1").arg(feature->name)));
}

void Document::toggleSuppress(Feature* feature)
{
    const QJsonObject before = feature->toJson();
    QJsonObject after = before;
    after[QStringLiteral("suppressed")] = !feature->suppressed;

    m_undoStack->push(new ChangeFeatureCommand(m_model, feature, before, after,
        QStringLiteral("%1 %2")
            .arg(feature->suppressed ? QStringLiteral("Unsuppress") : QStringLiteral("Suppress"),
                 feature->name)));
}

void Document::deleteFeature(Feature* feature)
{
    m_undoStack->push(new DeleteFeatureCommand(m_model, feature));
}

void Document::refreshDisplay()
{
    m_viewport->setShape(m_model->resultShape());
}

bool Document::exportStl(const QString& filePath) const
{
    if (!hasShape())
    {
        qWarning() << "exportStl: document" << m_name << "has no shape";
        return false;
    }

    try
    {
        // STL is triangles; the exact B-rep must be tessellated first.
        BRepMesh_IncrementalMesh mesh(m_model->resultShape(), 0.1);

        StlAPI_Writer writer;
        const bool ok = writer.Write(m_model->resultShape(), filePath.toStdString().c_str());
        qDebug() << "exportStl to" << filePath << "->" << (ok ? "ok" : "FAILED");
        return ok;
    }
    catch (const Standard_Failure& failure)
    {
        qCritical() << "OCCT exception in exportStl:" << failure.GetMessageString();
        return false;
    }
}
