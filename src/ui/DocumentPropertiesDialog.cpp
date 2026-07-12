#include "ui/DocumentPropertiesDialog.h"
#include "model/DocumentSettings.h"
#include "app/AppSettings.h"

#include <QFormLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QDialogButtonBox>

DocumentPropertiesDialog::DocumentPropertiesDialog(DocumentSettings* settings,
                                                   const QString& partName,
                                                   QWidget* parent)
    : QDialog(parent)
    , m_settings(settings)
{
    setWindowTitle(QStringLiteral("Document Properties - %1").arg(partName));
    setModal(true);

    auto* mainLayout = new QVBoxLayout(this);
    auto* form = new QFormLayout();
    mainLayout->addLayout(form);

    m_descriptionEdit = new QLineEdit(settings->description, this);
    form->addRow(QStringLiteral("Description"), m_descriptionEdit);

    m_materialEdit = new QLineEdit(settings->material, this);
    m_materialEdit->setToolTip(QStringLiteral("Free text (e.g. PLA, PETG, 6061 aluminum)."));
    form->addRow(QStringLiteral("Material"), m_materialEdit);

    const QString appUnit = Units::name(AppSettings::instance().unit());
    m_unitCombo = new QComboBox(this);
    m_unitCombo->addItems({ QStringLiteral("Use application default (%1)").arg(appUnit),
                            QStringLiteral("Millimeters"),
                            QStringLiteral("Inches") });
    m_unitCombo->setCurrentIndex(
        settings->hasUnitOverride ? (settings->unitOverride == Unit::Inches ? 2 : 1) : 0);
    form->addRow(QStringLiteral("Units (this file)"), m_unitCombo);

    m_meshOverrideCheck = new QCheckBox(QStringLiteral("Override export mesh quality"), this);
    m_meshOverrideCheck->setChecked(settings->hasMeshDeflectionOverride);
    form->addRow(QString(), m_meshOverrideCheck);

    m_meshSpin = new QDoubleSpinBox(this);
    m_meshSpin->setRange(0.001, 5.0);
    m_meshSpin->setDecimals(3);
    m_meshSpin->setSingleStep(0.05);
    m_meshSpin->setSuffix(QStringLiteral(" mm"));
    m_meshSpin->setValue(settings->hasMeshDeflectionOverride
                             ? settings->meshDeflectionOverride
                             : AppSettings::instance().meshDeflection());
    m_meshSpin->setEnabled(m_meshOverrideCheck->isChecked());
    connect(m_meshOverrideCheck, &QCheckBox::toggled, m_meshSpin, &QWidget::setEnabled);
    form->addRow(QStringLiteral("Mesh quality (this file)"), m_meshSpin);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(buttons);
}

void DocumentPropertiesDialog::accept()
{
    m_settings->description = m_descriptionEdit->text();
    m_settings->material = m_materialEdit->text();

    m_settings->hasUnitOverride = (m_unitCombo->currentIndex() != 0);
    if (m_settings->hasUnitOverride)
        m_settings->unitOverride =
            (m_unitCombo->currentIndex() == 2) ? Unit::Inches : Unit::Millimeters;

    m_settings->hasMeshDeflectionOverride = m_meshOverrideCheck->isChecked();
    if (m_settings->hasMeshDeflectionOverride)
        m_settings->meshDeflectionOverride = m_meshSpin->value();

    QDialog::accept();
}
