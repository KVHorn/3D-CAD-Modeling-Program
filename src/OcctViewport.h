// -------------------------------------------------------------------------------------------------
#pragma once

#include <QWidget>

// The following included files use the .hxx file extensions to match with OpenCASCADE
#include <V3d_Viewer.hxx>
#include <V3d_View.hxx>
#include <AIS_InteractiveContext.hxx>

// -------------------------------------------------------------------------------------------------
class OcctViewport : public QWidget
{
    Q_OBJECT

    // Visible to everything outside of the class
public:
    explicit OcctViewport(QWidget *parent = nullptr); // asdfaposdifha

protected:
    QPaintEngine *paintEngine() const override;     // "Qt, dont paint here"
    void paintEvent(QPaintEvent *event) override;   // Draw the occt view
    void resizeEvent(QResizeEvent *event) override; // Occt needs to know the new size

private:
    void intiViewer();           // One-time setup (body comes in the next sub-step)
    Handle(V3d_Viewer) m_viewer; // test
    Handle(V3d_View) m_view;
    Handle(AIS_InteractiveContext) m_context;
    bool m_initialized = false;
};