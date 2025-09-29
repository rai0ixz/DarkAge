#include "UI/SettingsMenuWidget.h"
#include "Core/SettingsManager.h"

void USettingsMenuWidget::ApplySettings() {
    if (SettingsManager) SettingsManager->SaveSettings();
}
void USettingsMenuWidget::LoadSettings() {
    if (SettingsManager) SettingsManager->LoadSettings();
}
void USettingsMenuWidget::SaveSettings() {
    if (SettingsManager) SettingsManager->SaveSettings();
}
