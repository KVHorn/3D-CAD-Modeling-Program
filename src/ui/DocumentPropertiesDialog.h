#ifndef DOCUMENTPROPERTIESDIALOG_H
#define DOCUMENTPROPERTIESDIALOG_H

#include <QDialog>

class QLineEdit;
class QComboBox;
class QCheckBox;
class QDoubleSpinBox;
class DocumentSettings;

// File > Document Properties: per-file settings with inherit-or-override
// semantics. Every override defaults to "Use application default"; chosen
// overrides are stored inside the .infinity file and travel with it.
class DocumentPropertiesDialog : public QDialog
{
    Q_OBJECT

public:
    DocumentPropertiesDialog(DocumentSettings* settings, const QString& partName,
                             QWidget* parent = nullptr);

    void accept() override;

private:
    DocumentSettings* m_settings;
    QLineEdit* m_descriptionEdit;
    QLineEdit* m_materialEdit;
    QComboBox* m_unitCombo;        // 0 = inherit, 1 = mm, 2 = inch
    QCheckBox* m_meshOverrideCheck;
    QDoubleSpinBox* m_meshSpin;
};

#endif // DOCUMENTPROPERTIESDIALOG_H
