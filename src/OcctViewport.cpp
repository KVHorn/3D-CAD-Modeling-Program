#include "OcctViewport.h"

#include <QPalette>
#include <QColor>

OcctViewport::OcctViewport(QWidget *parent)
    : QWidget(parent)
{
    // Temporary: a visible background so we can confirm the widget is wired in.
    // This goes away in Step 2, once OCCT takes over drawing the surface.
    setAutoFillBackground(true);

    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(43, 43, 43)); // charcoal #2b2b2b
    setPalette(pal);

    setMinimumSize(400, 300);
}