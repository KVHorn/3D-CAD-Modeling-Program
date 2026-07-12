#include "ui/FirstRunDialog.h"
#include "app/AppSettings.h"

#include <QVBoxLayout>
#include <QFormLayout>
#include <QComboBox>
#include <QLineEdit>
#include <QLabel>
#include <QDialogButtonBox>

FirstRunDialog::FirstRunDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral("Welcome to Infinity Creator"));
    setModal(true);

    auto* mainLayout = new QVBoxLayout(this);

    auto* welcome = new QLabel(QStringLiteral(
        "<b>Welcome!</b><br>A few quick questions to set your defaults. "
        "You can change any of these later in <i>Edit &gt; Options</i>."), this);
    welcome->setWordWrap(true);
    mainLayout->addWidget(welcome);

    auto* form = new QFormLayout();
    mainLayout->addLayout(form);

    m_unitCombo = new QComboBox(this);
    m_unitCombo->addItems({ QStringLiteral("Millimeters (metric)"),
                            QStringLiteral("Inches (imperial)") });
    form->addRow(QStringLiteral("Preferred units"), m_unitCombo);

    m_qualityCombo = new QComboBox(this);
    m_qualityCombo->addItems({ QStringLiteral("Fine (0.05 mm) - smoothest, biggest files"),
                               QStringLiteral("Standard (0.1 mm) - recommended"),
                               QStringLiteral("Coarse (0.5 mm) - fast drafts") });
    m_qualityCombo->setCurrentIndex(1);
    form->addRow(QStringLiteral("Export mesh quality"), m_qualityCombo);

    m_authorEdit = new QLineEdit(this);
    m_authorEdit->setPlaceholderText(QStringLiteral("optional"));
    form->addRow(QStringLiteral("Your name"), m_authorEdit);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    mainLayout->addWidget(buttons);
}

void FirstRunDialog::accept()
{
    AppSettings& settings = AppSettings::instance();
    settings.setUnit(m_unitCombo->currentIndex() == 1 ? Unit::Inches : Unit::Millimeters);

    const double deflections[] = { 0.05, 0.1, 0.5 };
    settings.setMeshDeflection(deflections[m_qualityCombo->currentIndex()]);

    settings.setAuthorName(m_authorEdit->text().trimmed());
    settings.setFirstRunDone(true);
    QDialog::accept();
}
