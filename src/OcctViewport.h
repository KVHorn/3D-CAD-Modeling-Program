#pragma once

#include <QWidget>

// The following included files use the .hxx file extensions to match with OpenCASCADE
#include <V3d_Viewer.hxx>
#include <V3d_View.hxx>
#include <AIS_InteractiveContext.hxx>

class OcctViewport : public QWidget
{
    Q_OBJECT

public:
    explicit OcctViewport(QWidget *parent = nullptr); // asdfaposdifha

protected:
    QPaintEngine *paintEngine() const override;   // "Qt, dont paint here"
    void paintEvent(QPaintEvent *event) override; // Draw the occt view

private:
    Handle(V3d_Viewer);
    Handle(V3d_View);
    Handle(AIS_InteractiveContext);
    bool m_initialized = false;

protected:
};
