# UDiseaseSystem (DiseaseSystem.h)

## Purpose
This subsystem manages a comprehensive disease simulation for the entire game world. It handles disease definitions, character infections, progression over time, treatment, and large-scale regional outbreaks. It is a data-driven system that relies on a `DT_DiseaseData` DataTable.

## Core Data Structures
- `FActiveDiseaseInstance`: A struct that tracks the state of a specific disease on a single character, including its current severity and duration.
- `RegionalOutbreaks`: A `TMap` that tracks the severity of various diseases within different game regions, influencing infection risk.

## Key Delegates
- `OnDiseaseInfection (AActor* Character, FName DiseaseID)`: Broadcasts when a character is newly infected.
- `OnDiseaseProgression (AActor* Character, FName DiseaseID, EDiseaseSeverity NewSeverity)`: Broadcasts when a disease on a character changes its severity stage.
- `OnDiseaseRecovery (AActor* Character, FName DiseaseID)`: Broadcasts when a character is cured of a disease.

## Key Methods

### Character-Specific Functions
- `InfectCharacter(AActor* Character, FName DiseaseID)`: Attempts to infect a character with a specific disease. Returns `true` if successful.
- `TreatDisease(AActor* Character, FName DiseaseID, FName MedicineID)`: Attempts to cure or reduce the severity of a disease using a specific medicine item.
- `IsInfectedWith(AActor* Character, FName DiseaseID) const`: Checks if a character is currently afflicted with a specific disease.
- `GetActiveDiseases(AActor* Character) const`: Returns an array of all active diseases currently affecting a character.
- `CalculateInfectionRisk(AActor* Character, FName DiseaseID) const`: Calculates the probability (0.0 to 1.0) of a character contracting a disease, based on their stats, equipment, and the current regional outbreak severity.

### World & Regional Functions
- `UpdateDiseases(float DeltaTime)`: The main tick function for the subsystem, which calls internal update functions for progression, transmission, and regional outbreaks.
- `TriggerOutbreak(FName DiseaseID, FName RegionID, float Severity)`: Starts or intensifies a disease outbreak in a specific region, increasing the ambient infection risk.
- `GetEndemicDiseases(FName RegionID) const`: Returns a list of diseases that are naturally present in a given region.
- `GetDiseaseData(FName DiseaseID) const`: Retrieves the static data for a disease from the internal disease database (loaded from a DataTable).

## Data-Driven Design
The entire system is configured through a `UDataTable` using the `FDiseaseData` struct.

### `FDiseaseData` (Struct)
- `SeverityStages (TArray<FDiseaseSeverityStage>)`: An array defining the different stages of the disease (e.g., Incubation, Mild, Severe), their duration, and the status effects they apply.
- `TransmissionMethods (TArray<EDiseaseTransmissionMethod>)`: How the disease can spread (e.g., Airborne, Waterborne, Contact).
- `BaseInfectivity (float)`: The base chance of the disease being transmitted.
- `Treatments (TMap<FName, float>)`: A map where the key is a Medicine Item ID and the value is the effectiveness of that medicine against this disease.

## Example Usage

To make a character drink from a potentially contaminated water source:
```cpp
// This logic would exist somewhere in the world interaction code.
void AWaterSource::OnDrink(AActor* Character)
{
    UDiseaseSystem* DiseaseSystem = GetGameInstance()->GetSubsystem<UDiseaseSystem>();
    if (DiseaseSystem)
    {
        // Check the risk for a common waterborne illness
        float Risk = DiseaseSystem->CalculateInfectionRisk(Character, FName("StomachFlu"));
        
        // Roll the dice
        if (FMath::FRand() < Risk)
        {
            DiseaseSystem->InfectCharacter(Character, FName("StomachFlu"));
        }
    }
}
```

## Integration Points
- **`UStatlineComponent`**: The `ApplyDiseaseEffects` function will apply status effects that modify the character's stats in their `UStatlineComponent`.
- **`AdvancedSurvivalSubsystem`**: This system can be a primary driver for calling `CalculateInfectionRisk` based on character actions.
- **World Systems**: The `WeatherSystem` or other world management systems can call `TriggerOutbreak` to simulate seasonal sickness or other large-scale events.
- **UI**: The UI can bind to the delegates (`OnDiseaseInfection`, `OnDiseaseProgression`, etc.) to provide notifications to the player about their health status.
