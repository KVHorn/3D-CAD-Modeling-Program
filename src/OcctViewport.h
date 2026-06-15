#pragma once

#include <QWidget>
#include <QMainWindow>

//The following included files use the .hxx file extensions to match with OpenCASCADE
#include <V3d_Viewer.hxx>
#include <V3d_View.hxx>
#include <AIS_InteractiveContext.hxx>



class OcctViewport : public QWidget
{
    Q_OBJECT
    

public:
    explicit OcctViewport(QWidget* parent = nullptr);

private:
    Handle(V3d_Viewer) m_viewport;
    Handle(V3d_View) m_;
    Handle(AIS_InteractiveContext);
    bool m_initialized = false;

protected:

};
