#include "viewport/OcctViewport.h"

#include <QDebug>
#include <QMouseEvent>
#include <QWheelEvent>

#include <Aspect_DisplayConnection.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <WNT_Window.hxx>
#include <Quantity_Color.hxx>
#include <Standard_Failure.hxx>
#include <AIS_Shape.hxx>
#include <Aspect_TypeOfTriedronPosition.hxx>

namespace
{
    // One graphic driver shared by every viewport in the app - the standard
    // OCCT multi-view pattern for MDI.
    Handle(OpenGl_GraphicDriver)& sharedGraphicDriver()
    {
        static Handle(OpenGl_GraphicDriver) driver;
        if (driver.IsNull())
        {
            Handle(Aspect_DisplayConnection) displayConnection = new Aspect_DisplayConnection();
            driver = new OpenGl_GraphicDriver(displayConnection);
        }
        return driver;
    }
}

OcctViewport::OcctViewport(QWidget* parent)
    : QWidget(parent)
    , m_initialized(false)
{
    // Tell Qt to keep its hands off this widget's surface. Do NOT combine
    // these with setAutoFillBackground / QPalette code - the legacy palette
    // path conflicts with WA_PaintOnScreen.
    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_NoSystemBackground);
    setFocusPolicy(Qt::StrongFocus);
}

QPaintEngine* OcctViewport::paintEngine() const
{
    // nullptr = "Qt, you have no paint engine here" - stops Qt's raster
    // painter from fighting OCCT's OpenGL for the window surface.
    return nullptr;
}

void OcctViewport::initViewer()
{
    if (m_initialized)
        return;

    qDebug() << "OcctViewport::initViewer starting, winId =" << winId();

    // Wrapped so OCCT exceptions (Standard_Failure descendants like
    // Aspect_GraphicDeviceDefinitionError) land in debug.log instead of
    // failing silently as a black viewport.
    try
    {
        m_viewer = new V3d_Viewer(sharedGraphicDriver());
        m_viewer->SetDefaultLights();
        m_viewer->SetLightOn();

        m_view = m_viewer->CreateView();

        Handle(WNT_Window) wntWindow = new WNT_Window(reinterpret_cast<Aspect_Handle>(winId()));
        m_view->SetWindow(wntWindow);
        if (!wntWindow->IsMapped())
            wntWindow->Map();

        m_context = new AIS_InteractiveContext(m_viewer);

        // Dark blue background: OCCT wants normalized 0.0-1.0 components.
        m_view->SetBackgroundColor(Quantity_Color(0.0, 0.098, 0.333, Quantity_TOC_sRGB));
        m_view->TriedronDisplay(Aspect_TOTP_LEFT_LOWER, Quantity_NOC_WHITE, 0.1, V3d_ZBUFFER);
        m_view->MustBeResized();

        m_initialized = true;
        qDebug() << "OcctViewport::initViewer completed. view null?" << m_view.IsNull();

        // A shape may have been set before the viewer existed (e.g. a
        // document loaded from disk) - show it now.
        if (!m_shape.IsNull())
            refreshDisplayedShape();
    }
    catch (const Standard_Failure& failure)
    {
        qCritical() << "OCCT exception in initViewer:" << failure.GetMessageString();
    }
}

void OcctViewport::setShape(const TopoDS_Shape& shape)
{
    m_shape = shape;
    if (m_initialized)
        refreshDisplayedShape();
}

void OcctViewport::refreshDisplayedShape()
{
    if (m_context.IsNull())
        return;

    m_context->RemoveAll(Standard_False);

    if (!m_shape.IsNull())
    {
        Handle(AIS_Shape) aisShape = new AIS_Shape(m_shape);
        m_context->Display(aisShape, Standard_False);
        m_view->FitAll();
    }

    m_view->Redraw();
}

void OcctViewport::paintEvent(QPaintEvent* /*event*/)
{
    if (!m_initialized)
        initViewer();

    if (!m_view.IsNull())
        m_view->Redraw();
}

void OcctViewport::resizeEvent(QResizeEvent* /*event*/)
{
    if (!m_view.IsNull())
        m_view->MustBeResized();
}

void OcctViewport::mousePressEvent(QMouseEvent* event)
{
    m_lastMousePos = event->pos();

    if (event->buttons() & Qt::LeftButton && !m_view.IsNull())
        m_view->StartRotation(event->pos().x(), event->pos().y());
}

void OcctViewport::mouseMoveEvent(QMouseEvent* event)
{
    if (m_view.IsNull())
        return;

    if (event->buttons() & Qt::LeftButton)
    {
        m_view->Rotation(event->pos().x(), event->pos().y());
    }
    else if (event->buttons() & Qt::MiddleButton)
    {
        const QPoint delta = event->pos() - m_lastMousePos;
        m_view->Pan(delta.x(), -delta.y());
    }

    m_lastMousePos = event->pos();
}

void OcctViewport::wheelEvent(QWheelEvent* event)
{
    if (m_view.IsNull())
        return;

    const double factor = (event->angleDelta().y() > 0) ? 1.1 : 0.9;
    m_view->SetZoom(factor);
}
