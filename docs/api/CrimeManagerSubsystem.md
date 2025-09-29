# UCrimeManagerSubsystem (CrimeManagerSubsystem.h)

## Purpose
This subsystem is the central authority for tracking all criminal activity in the world. It maintains a persistent record of crimes, associated evidence, witnesses, and bounties. It provides a comprehensive API for other systems to report crimes and query the crime database.

## Core Data Structures

### `FCrimeData` (Struct)
The main struct that represents a single criminal act.
- `CrimeId (FGuid)`: A unique identifier for the crime.
- `Perpetrator (TWeakObjectPtr<AActor>)`: The actor who committed the crime.
- `Witnesses (TArray<TWeakObjectPtr<AActor>>)`: A list of actors who witnessed the crime.
- `CrimeType (ECrimeType)`: The type of crime committed (e.g., Theft, Assault).
- `CrimeLocation (FVector)`: The world location where the crime occurred.
- `Timestamp (FDateTime)`: When the crime was committed.
- `Status (ECrimeStatus)`: The current status of the investigation (e.g., Reported, Witnessed, Resolved).
- `Evidence (TMap<FGuid, FEvidence>)`: A map of all evidence associated with this crime.
- `Bounty (FBounty)`: The bounty placed on the perpetrator for this crime.

### `FEvidence` (Struct)
- `EvidenceId (FGuid)`: A unique ID for the piece of evidence.
- `Description (FString)`: A description of the evidence.
- `AssociatedActor (TWeakObjectPtr<AActor>)`: An actor linked to the evidence (e.g., a stolen item).

### `FBounty` (Struct)
- `Amount (int32)`: The gold value of the bounty.
- `BountyPlacer (TWeakObjectPtr<AActor>)`: The actor or faction that placed the bounty.
- `IsActive (bool)`: Whether the bounty is currently active.

## Key Methods

### Reporting and Modifying Crimes
- `ReportCrime(...)`: The primary function to report a new crime. It takes the perpetrator, a witness, the crime type, a notoriety value, location, and region as arguments and creates a new `FCrimeData` entry.
- `AddWitness(const FGuid& CrimeId, AActor* Witness)`: Adds a new witness to an existing crime.
- `ReportEvidence(...)`: Adds a new piece of evidence to an existing crime.
- `PlaceBounty(...)`: Places a bounty on the perpetrator of a specific crime.
- `UpdateCrimeStatus(const FGuid& CrimeId, ECrimeStatus NewStatus)`: Changes the status of an ongoing investigation.

### Querying the Crime Database
- `GetCrimeData(const FGuid& CrimeId, FCrimeData& OutCrimeData) const`: Retrieves the full data for a single crime.
- `GetCrimesByPerpetrator(AActor* Perpetrator) const`: Returns all crimes committed by a specific actor.
- `GetCrimesByStatus(ECrimeStatus Status) const`: Returns all crimes that currently have a specific status.
- `GetCrimesByRegion(const FString& RegionID) const`: Returns all crimes committed within a specific region.
- `GetBounties() const`: Returns a list of all active bounties.
- `GetWitnesses(const FGuid& CrimeId) const`: Returns the list of witnesses for a specific crime.
- `GetEvidence(const FGuid& CrimeId) const`: Returns the list of evidence for a specific crime.

## Persistence
- `SaveCrimes()` / `LoadCrimes()`: Private methods called by the subsystem to handle the serialization of the `ReportedCrimes` map, ensuring that criminal records are persistent across game sessions.

## Integration Points
- **`UNotorietyComponent`**: When a crime is reported via `ReportCrime`, this subsystem should be responsible for notifying the `UNotorietyComponent` on the perpetrator to increase their notoriety.
- **AI Systems**: The AI can query this subsystem to determine if a character is a known criminal. Guards might become hostile if `GetCrimesByPerpetrator` returns active warrants.
- **Dialogue & Quest Systems**: Quests can be generated based on data from this subsystem (e.g., a quest to find evidence for an `UnderInvestigation` crime). Dialogue with NPCs might change based on the player's criminal record.
- **UI**: A "Bounty Board" UI would query `GetBounties()` to display active bounties to the player.
