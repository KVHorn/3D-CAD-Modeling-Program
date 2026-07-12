#ifndef FIRSTRUNDIALOG_H
#define FIRSTRUNDIALOG_H

#include <QDialog>

class QComboBox;
class QLineEdit;

// Shown once, on the very first launch: sets the initial application
// defaults (units, export quality, author). Everything chosen here can be
// changed later in Edit > Options.
class FirstRunDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FirstRunDialog(QWidget* parent = nullptr);

    void accept() override;

private:
    QComboBox* m_unitCombo;
    QComboBox* m_qualityCombo;
    QLineEdit* m_authorEdit;
};

#endif // FIRSTRUNDIALOG_H
