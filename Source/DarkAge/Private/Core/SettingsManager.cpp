#include "Core/SettingsManager.h"
#include "Kismet/GameplayStatics.h"

void USettingsManager::SaveSettings()
{
    SaveConfig();
}

void USettingsManager::LoadSettings()
{
    LoadConfig();
}
