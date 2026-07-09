#include "OcctViewport.h"

#include <Aspect_DisplayConnection.hxx>
#include <OpenGL_GraphicDriver.hxx>
#include <V3d_Viewer.hxx>
#include <V3d_View.hxx>
#include <AIS_InteractiveContext.hxx>
#include <WNT_Window.hxx>

#include <QResizeEvent>

#include <QPalette>
#include <QColor>

/* ---------------------------------------------------------------------
Notes:

The WA_PaintOnScreen attribute plus the paintEngine() returning nullptr are a matched pair. Together they tell Qt: "Dont manage this widget's pixels, someone else owns the surface."
    - Without both, Qt's own paint engine fights OCCT's OpenGL context and can result in flicker or a black rectangle


(Aspect_Handle)winId() is the actual bridge between the two worlds.
    - winId() hands you the native Win32 HWND for this widget.
    - WNT_Window wraps that handle so OCCT can render into it.
    ***** This is the one line that will most likeyl need a tweak if there are any errors returned by the compiler. If there are errors, try: reinterpret_cast<Aspect_Handle>(winId())

MustBeResized() in resizeEvent is what keeps the view's aspect ratio correct when the window changes size.

The initViewer() call living inside paintEvent(rather than the constructor) is deliberate: the native window handle from winId() isn't reliable until the widget is actually shown so you must defer setup to the first paint.




 ---------------------------------------------------------------------*/
OcctViewport::OcctViewport(QWidget *parent)
    : QWidget(parent), m_initialized(false)
{

    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
    setMinimumSize(400, 300);
}

// ---------------------------------------------------------------------
QPaintEngine *OcctViewport::paintEngine() const
{
    // Returning nullptr stops Qt from drawing over the OpenGL surface.
    return nullptr;
}

// ---------------------------------------------------------------------
void OcctViewport::initViewer()
{
    if (m_initialized)
        return;

    Handle(Aspect_DisplayConnection) displayConnection = new Aspect_DisplayConnection();
    Handle(OpenGl_GraphicDriver) graphicDriver = new OpenGl_GraphicDriver(displayConnection);

    m_viewer = new V3d_Viewer(graphicDriver);
    m_viewer->SetDefaultLights();
    m_viewer->SetLightOn();

    m_view = m_viewer->CreateView();

    Handle(WNT_Window) window = new WNT_Window((Aspect_Handle)winId());
    m_view->SetWindow(window);
    if (!window->IsMapped())
        window->Map();

    m_context = new AIS_InteractiveContext(m_viewer);
    // m_view->SetBackgroundColor(Quantity_NOC_GRAY30);
    m_view->SetBackgroundColor(Quantity_Color(0.0, 0.098, 0.333, Quantity_TOC_RGB));
    m_view->MustBeResized();

    m_initialized = true;
}

// ---------------------------------------------------------------------
void OcctViewport::paintEvent(QPaintEvent *)
{
    // ensure the viewer is up (first paint triggers init)
    initViewer();     // Make sure the Occt viewer exists.
    m_view->Redraw(); // Tell Occt  to render the scene now.
}

// ---------------------------------------------------------------------
void OcctViewport::resizeEvent(QResizeEvent *)
{
    /*
    This if statement is a safety guard. A resize can fire before the first paint has run initViewer, so m_view might not exist.
    This checks for two things:
    - That init has happened (m_initialized)
    - That the handle actually points at something (!m_view.IsNull()) where .IsNull() is the OCCT Handle way of asking "is this pointing at nothing?", like a null-pointer check.
    NOTE: Both must be true to continue.


    m_view->MustBeResized() is OCCT's 're-read your window's dimension's and adjust'. This keeps the viewport matched to the widgets new size.

    */
    if (m_initialized && !m_view.IsNull())
        m_view->MustBeResized();
}