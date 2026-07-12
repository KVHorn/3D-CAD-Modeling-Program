#include "ui/ParameterDialog.h"
#include "model/Feature.h"

#include <QVBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QDialogButtonBox>

ParameterDialog::ParameterDialog(Feature* feature, QWidget* parent)
    : QDialog(parent)
    , m_feature(feature)
{
    setWindowTitle(feature->typeName() + QStringLiteral(" Definition"));
    setModal(true);

    auto* mainLayout = new QVBoxLayout(this);
    auto* form = new QFormLayout();
    mainLayout->addLayout(form);

    m_nameEdit = new QLineEdit(feature->name, this);
    form->addRow(QStringLiteral("Name"), m_nameEdit);

    if (feature->supportsBooleanOp())
    {
        m_booleanCombo = new QComboBox(this);
        // Order matches the BooleanOp enum values.
        m_booleanCombo->addItems({ QStringLiteral("New body"),
                                   QStringLiteral("Add (fuse)"),
                                   QStringLiteral("Subtract (cut)"),
                                   QStringLiteral("Intersect") });
        m_booleanCombo->setCurrentIndex(static_cast<int>(feature->booleanOp));
        form->addRow(QStringLiteral("Operation"), m_booleanCombo);
    }

    for (const FeatureParameter& p : feature->parameters())
    {
        auto* spin = new QDoubleSpinBox(this);
        spin->setRange(-100000.0, 100000.0);
        spin->setDecimals(3);
        spin->setValue(p.value);
        const bool isAngle = p.key.startsWith(QStringLiteral("r"))
                             && p.label.contains(QStringLiteral("deg"));
        spin->setSuffix(isAngle ? QStringLiteral(" deg") : QStringLiteral(" mm"));
        form->addRow(p.label, spin);
        m_spinBoxes.append(spin);
    }

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(buttons);
}

void ParameterDialog::accept()
{
    // Write values back only on OK - Cancel leaves the feature untouched,
    // which matters when this dialog is reused for editing in Phase 2.
    m_feature->name = m_nameEdit->text().trimmed().isEmpty()
        ? m_feature->typeName() : m_nameEdit->text().trimmed();

    if (m_booleanCombo)
        m_feature->booleanOp = static_cast<BooleanOp>(m_booleanCombo->currentIndex());

    QVector<FeatureParameter>& params = m_feature->parameters();
    for (int i = 0; i < params.size() && i < m_spinBoxes.size(); ++i)
        params[i].value = m_spinBoxes[i]->value();

    QDialog::accept();
}
