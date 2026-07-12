#include "app/FeatureTreeDock.h"
#include "app/Document.h"
#include "model/PartModel.h"
#include "model/Feature.h"

#include <QTreeWidget>
#include <QMenu>
#include <QBrush>
#include <QFont>

FeatureTreeDock::FeatureTreeDock(QWidget* parent)
    : QDockWidget(QStringLiteral("Specification Tree"), parent)
    , m_tree(new QTreeWidget(this))
{
    setObjectName(QStringLiteral("SpecificationTree"));

    m_tree->setHeaderHidden(true);
    m_tree->setContextMenuPolicy(Qt::CustomContextMenu);
    setWidget(m_tree);

    connect(m_tree, &QTreeWidget::itemDoubleClicked,
            this, &FeatureTreeDock::onItemDoubleClicked);
    connect(m_tree, &QTreeWidget::customContextMenuRequested,
            this, &FeatureTreeDock::onContextMenu);
}

void FeatureTreeDock::setDocument(Document* document)
{
    if (m_document == document)
        return;

    if (m_document)
        disconnect(m_document->model(), &PartModel::recomputed, this, &FeatureTreeDock::rebuild);

    m_document = document;

    if (m_document)
        connect(m_document->model(), &PartModel::recomputed, this, &FeatureTreeDock::rebuild);

    rebuild();
}

void FeatureTreeDock::rebuild()
{
    m_tree->clear();

    if (!m_document)
        return;

    auto* root = new QTreeWidgetItem(m_tree, { m_document->name() });
    root->setFlags(root->flags() & ~Qt::ItemIsSelectable);

    PartModel* model = m_document->model();

    for (Feature* feature : model->features())
    {
        auto* item = new QTreeWidgetItem(root, { feature->name });
        item->setData(0, Qt::UserRole, QVariant::fromValue<void*>(feature));

        if (feature == model->failedFeature())
        {
            item->setForeground(0, QBrush(QColor(200, 0, 0)));
            item->setToolTip(0, model->lastError());
        }
        else if (feature->suppressed)
        {
            item->setForeground(0, QBrush(QColor(128, 128, 128)));
            QFont font = item->font(0);
            font.setItalic(true);
            item->setFont(0, font);
            item->setToolTip(0, QStringLiteral("Suppressed"));
        }
    }

    m_tree->expandAll();
}

Feature* FeatureTreeDock::featureForItem(QTreeWidgetItem* item) const
{
    if (!item)
        return nullptr;
    return static_cast<Feature*>(item->data(0, Qt::UserRole).value<void*>());
}

Feature* FeatureTreeDock::selectedFeature() const
{
    return featureForItem(m_tree->currentItem());
}

void FeatureTreeDock::onItemDoubleClicked(QTreeWidgetItem* item, int /*column*/)
{
    Feature* feature = featureForItem(item);
    if (feature && m_document)
        m_document->editFeature(feature);
}

void FeatureTreeDock::onContextMenu(const QPoint& pos)
{
    QTreeWidgetItem* item = m_tree->itemAt(pos);
    Feature* feature = featureForItem(item);
    if (!feature || !m_document)
        return;

    QMenu menu(this);
    QAction* editAction = menu.addAction(QStringLiteral("Edit definition..."));
    QAction* suppressAction = menu.addAction(
        feature->suppressed ? QStringLiteral("Unsuppress") : QStringLiteral("Suppress"));
    menu.addSeparator();
    QAction* deleteAction = menu.addAction(QStringLiteral("Delete"));

    QAction* chosen = menu.exec(m_tree->viewport()->mapToGlobal(pos));
    if (chosen == editAction)
        m_document->editFeature(feature);
    else if (chosen == suppressAction)
        m_document->toggleSuppress(feature);
    else if (chosen == deleteAction)
        m_document->deleteFeature(feature);
}
