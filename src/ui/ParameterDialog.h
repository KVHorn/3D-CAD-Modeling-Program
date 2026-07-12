#ifndef PARAMETERDIALOG_H
#define PARAMETERDIALOG_H

#include <QDialog>
#include <QVector>

#include "model/Units.h"

class QLineEdit;
class QComboBox;
class QDoubleSpinBox;
class Feature;

// CATIA-style feature definition dialog, built generically from a feature's
// parameter list. Lengths are displayed/entered in the document's effective
// unit and stored in mm; angles stay in degrees. Values write back on OK.
class ParameterDialog : public QDialog
{
    Q_OBJECT

public:
    ParameterDialog(Feature* feature, Unit displayUnit, QWidget* parent = nullptr);

    void accept() override;

private:
    Feature* m_feature;
    Unit m_displayUnit;
    QLineEdit* m_nameEdit;
    QComboBox* m_booleanCombo = nullptr;
    QVector<QDoubleSpinBox*> m_spinBoxes;
};

#endif // PARAMETERDIALOG_H
