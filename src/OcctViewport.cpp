#include "OcctViewport.h"
#include <QPalette>
#include <QColor>

OcctViewport::OcctViewport(QWidget *parent)
    : QWidget(parent)
{
    // This is the viewport for displaying the workspace and the sketches, shapes, models, etc. that the user will be working with.

    setAutoFillBackground(true);

    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(0, 25, 85)); // charcoal rgb(0, 24, 83)
    setPalette(pal);

    setMinimumSize(400, 300);
}