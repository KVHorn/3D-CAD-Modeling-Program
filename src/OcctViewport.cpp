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
    //     // This is the viewport for displaying the workspace and the sketches, shapes, models, etc. that the user will be working with.
    //     setAutoFillBackground(true);
    //     QPalette pal = palette();
    //     pal.setColor(QPalette::Window, QColor(0, 25, 85)); // Dark Blue
    //     setPalette(pal);
    //     setMinimumSize(400, 300);
    // }

    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
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
    m_view->SetBackgroundColor(Quantity_NOC_GRAY30);
    m_view->MustBeResized();

    m_initialized = true;
}

// ---------------------------------------------------------------------
void OcctViewport::paintEvent(QPaintEvent *)
{
    if (m_initialized && !m_view.IsNull())
        m_view->MustBeResized();
}