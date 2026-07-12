#include "app/Document.h"
#include "viewport/OcctViewport.h"
#include "model/PartModel.h"
#include "model/Feature.h"
#include "model/FeatureFactory.h"
#include "app/UndoStack.h"
#include "ui/ParameterDialog.h"
#include "io/StlExporter.h"
#include "io/ThreeMfExporter.h"

#include <QVBoxLayout>
#include <QUndoStack>
#include <QMessageBox>
#include <QCloseEvent>
#include <QDebug>

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
    connect(m_undoStack, &QUndoStack::cleanChanged, this,
            [this](bool) { emit dirtyStateChanged(); });
}

void Document::setName(const QString& name)
{
    m_name = name;
    setWindowTitle(name);
}

bool Document::isDirty() const
{
    return !m_undoStack->isClean() || m_settingsDirty;
}

void Document::markSettingsDirty()
{
    m_settingsDirty = true;
    emit dirtyStateChanged();
}

void Document::markSaved()
{
    m_undoStack->setClean();
    m_settingsDirty = false;
    emit dirtyStateChanged();
}

void Document::setSaveHandler(std::function<bool(Document*)> handler)
{
    m_saveHandler = std::move(handler);
}

void Document::closeEvent(QCloseEvent* event)
{
    if (!isDirty())
    {
        event->accept();
        return;
    }

    const auto choice = QMessageBox::warning(this, QStringLiteral("Unsaved changes"),
        QStringLiteral("\"%1\" has unsaved changes.\n\nSave before closing?").arg(m_name),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
        QMessageBox::Save);

    if (choice == QMessageBox::Cancel)
    {
        event->ignore();
        return;
    }
    if (choice == QMessageBox::Save)
    {
        if (!m_saveHandler || !m_saveHandler(this))
        {
            event->ignore();  // save cancelled or failed - keep the window
            return;
        }
    }
    event->accept();
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

    feature->booleanOp = isEmpty() ? BooleanOp::NewBody : BooleanOp::Add;

    ParameterDialog dialog(feature, m_settings.effectiveUnit(),
                           dialogParent ? dialogParent : this);
    if (dialog.exec() != QDialog::Accepted)
    {
        delete feature;
        return;
    }

    m_undoStack->push(new AddFeatureCommand(m_model, feature));

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

    ParameterDialog dialog(feature, m_settings.effectiveUnit(), this);
    if (dialog.exec() != QDialog::Accepted)
        return;

    const QJsonObject after = feature->toJson();

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
    return StlExporter::exportShape(m_model->resultShape(), filePath,
                                    m_settings.effectiveMeshDeflection());
}

bool Document::export3mf(const QString& filePath) const
{
    return ThreeMfExporter::exportShape(m_model->resultShape(), filePath,
                                        m_settings.effectiveMeshDeflection(), m_name);
}
