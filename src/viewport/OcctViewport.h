#ifndef OCCTVIEWPORT_H
#define OCCTVIEWPORT_H

#include <QWidget>
#include <QPoint>

#include <V3d_Viewer.hxx>
#include <V3d_View.hxx>
#include <AIS_InteractiveContext.hxx>
#include <TopoDS_Shape.hxx>

// A QWidget that hands its native window surface over to OCCT's OpenGL
// renderer. Qt owns the widget shell; OCCT owns every pixel inside it.
class OcctViewport : public QWidget
{
    Q_OBJECT

public:
    explicit OcctViewport(QWidget* parent = nullptr);

    // Display exactly this shape (clearing whatever was shown before) and
    // fit the camera. Pass a null shape to clear the scene. Safe to call
    // before the viewer is initialized - the shape is displayed on init.
    void setShape(const TopoDS_Shape& shape);

    Handle(AIS_InteractiveContext) context() const { return m_context; }
    Handle(V3d_View) view() const { return m_view; }

protected:
    // Return nullptr so Qt's raster painter never touches our surface.
    QPaintEngine* paintEngine() const override;

    // First paint triggers initViewer() (winId() is only valid once the
    // widget is actually shown); every paint after that just redraws.
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

    // Mouse navigation: LMB rotate, MMB pan, wheel zoom.
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:
    void initViewer();
    void refreshDisplayedShape();

    Handle(V3d_Viewer) m_viewer;
    Handle(V3d_View) m_view;
    Handle(AIS_InteractiveContext) m_context;
    bool m_initialized;

    TopoDS_Shape m_shape;  // what should currently be displayed
    QPoint m_lastMousePos;
};

#endif // OCCTVIEWPORT_H
