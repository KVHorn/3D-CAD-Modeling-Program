#pragma once

#include <QWidget>


class OcctViewport : public QWidget
{
    Q_OBJECT

public:
    explicit OcctViewport(QWidget* parent = nullptr);
};
