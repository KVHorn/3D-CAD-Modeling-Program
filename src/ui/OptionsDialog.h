#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QDialog>

class QComboBox;
class QDoubleSpinBox;
class QLineEdit;

// Edit > Options: application-wide settings, persisted via AppSettings.
// Per-document overrides live in File > Document Properties instead.
class OptionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OptionsDialog(QWidget* parent = nullptr);

    void accept() override;

private:
    QComboBox* m_unitCombo;
    QDoubleSpinBox* m_meshSpin;
    QLineEdit* m_authorEdit;
};

#endif // OPTIONSDIALOG_H
