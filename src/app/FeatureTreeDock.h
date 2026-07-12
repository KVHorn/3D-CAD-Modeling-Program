#ifndef FEATURETREEDOCK_H
#define FEATURETREEDOCK_H

#include <QDockWidget>

class QTreeWidget;
class QTreeWidgetItem;
class Document;
class Feature;

// The specification tree - CATIA's left-hand panel. Shows the active
// document's feature history: double-click (or Edit > Properties) opens the
// feature's definition dialog, the context menu offers Edit / Suppress /
// Delete. Failed features render red, suppressed features gray italic.
// One shared instance, retargeted as document focus changes.
class FeatureTreeDock : public QDockWidget
{
    Q_OBJECT

public:
    explicit FeatureTreeDock(QWidget* parent = nullptr);

    // Point the tree at a document (nullptr clears it).
    void setDocument(Document* document);

    // The feature currently selected in the tree, if any.
    Feature* selectedFeature() const;

public slots:
    void rebuild();

private slots:
    void onItemDoubleClicked(QTreeWidgetItem* item, int column);
    void onContextMenu(const QPoint& pos);

private:
    Feature* featureForItem(QTreeWidgetItem* item) const;

    QTreeWidget* m_tree;
    Document* m_document = nullptr;
};

#endif // FEATURETREEDOCK_H
