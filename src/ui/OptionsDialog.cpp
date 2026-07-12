#include "ui/OptionsDialog.h"
#include "app/AppSettings.h"

#include <QFormLayout>
#include <QVBoxLayout>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QLabel>

OptionsDialog::OptionsDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral("Options"));
    setModal(true);

    AppSettings& settings = AppSettings::instance();

    auto* mainLayout = new QVBoxLayout(this);
    auto* form = new QFormLayout();
    mainLayout->addLayout(form);

    m_unitCombo = new QComboBox(this);
    m_unitCombo->addItems({ QStringLiteral("Millimeters"), QStringLiteral("Inches") });
    m_unitCombo->setCurrentIndex(static_cast<int>(settings.unit()));
    form->addRow(QStringLiteral("Display units"), m_unitCombo);

    m_meshSpin = new QDoubleSpinBox(this);
    m_meshSpin->setRange(0.001, 5.0);
    m_meshSpin->setDecimals(3);
    m_meshSpin->setSingleStep(0.05);
    m_meshSpin->setSuffix(QStringLiteral(" mm"));
    m_meshSpin->setValue(settings.meshDeflection());
    m_meshSpin->setToolTip(QStringLiteral(
        "Max deviation of exported meshes (STL/3MF) from the exact surface.\n"
        "Smaller = smoother and larger files. 0.1 mm suits most prints."));
    form->addRow(QStringLiteral("Export mesh quality"), m_meshSpin);

    m_authorEdit = new QLineEdit(settings.authorName(), this);
    m_authorEdit->setToolTip(QStringLiteral("Used in drawing title blocks later."));
    form->addRow(QStringLiteral("Author name"), m_authorEdit);

    auto* note = new QLabel(QStringLiteral(
        "These apply program-wide. Per-file overrides: File > Document Properties."), this);
    note->setWordWrap(true);
    mainLayout->addWidget(note);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(buttons);
}

void OptionsDialog::accept()
{
    AppSettings& settings = AppSettings::instance();
    settings.setUnit(static_cast<Unit>(m_unitCombo->currentIndex()));
    settings.setMeshDeflection(m_meshSpin->value());
    settings.setAuthorName(m_authorEdit->text().trimmed());
    QDialog::accept();
}
