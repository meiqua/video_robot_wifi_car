#include "settingsdialog.h"
#include "ui_settingsdialog.h"

QT_USE_NAMESPACE

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    defaultSettings.IPaddress = ui->IPaddress->text();
    defaultSettings.controlIP = ui->controlIP->text();

    updateSettings();
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

SettingsDialog::Settings SettingsDialog::settings() const
{
    return currentSettings;
}

void SettingsDialog::on_applyButton_clicked()
{
    updateSettings();
    hide();
}

void SettingsDialog::updateSettings()
{
    currentSettings.IPaddress = ui->IPaddress->text();
    currentSettings.controlIP = ui->controlIP->text();
}


void SettingsDialog::on_defultButton_clicked()
{
    ui->IPaddress->setText(defaultSettings.IPaddress);
    ui->controlIP->setText(defaultSettings.controlIP);
}
