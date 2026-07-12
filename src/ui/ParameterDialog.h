#ifndef PARAMETERDIALOG_H
#define PARAMETERDIALOG_H

#include <QDialog>
#include <QVector>

class QLineEdit;
class QComboBox;
class QDoubleSpinBox;
class Feature;

// The equivalent of CATIA's feature definition dialogs ("Pad Definition",
// "Hole Definition"...). Built generically from a feature's parameter list:
// name field, boolean-operation combo (when the feature supports it), and
// one spin box per parameter. Values are written back only on OK.
class ParameterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ParameterDialog(Feature* feature, QWidget* parent = nullptr);

    void accept() override;

private:
    Feature* m_feature;
    QLineEdit* m_nameEdit;
    QComboBox* m_booleanCombo = nullptr;
    QVector<QDoubleSpinBox*> m_spinBoxes;  // parallel to feature->parameters()
};

#endif // PARAMETERDIALOG_H
