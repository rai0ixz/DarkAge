# UAICombatBehaviorComponent (AICombatBehaviorComponent.h)

## Purpose
This component encapsulates the combat logic and state management for an AI character. It provides a simple state machine to drive AI behavior and exposes functions to control targeting and combat actions. It is intended to be used in conjunction with a Behavior Tree, which can call the functions on this component to execute specific actions.

## Key Properties & Enums

### `ECombatState` (Enum)
A `UENUM` that defines the possible combat states for the AI.
- `Idle`: The AI is not engaged in combat and is not actively patrolling.
- `Patrol`: The AI is moving along a predefined path.
- `Chase`: The AI is actively pursuing a target.
- `Attack`: The AI is in range and is performing attack actions.
- `Flee`: The AI is attempting to escape from combat.

### Properties
- `CombatState (ECombatState)`: The current combat state of the AI. Can be set and read from Blueprints.
- `CurrentTarget (AActor*)`: A pointer to the AI's current target.

## Key Methods

### Target Management
- `SetTarget(AActor* NewTarget)`: Sets the `CurrentTarget` and can be used to trigger a transition into the `Chase` or `Attack` state.
- `ClearTarget()`: Clears the `CurrentTarget`, often causing a transition back to `Idle` or `Patrol`.

### State Management
- `EnterCombatState(ECombatState NewState)`: Explicitly sets the `CombatState`. This is the primary way a Behavior Tree should change the AI's state.
- `HandleCombatState(float DeltaTime)`: The main private function called by `TickComponent`. It contains a switch statement that calls the appropriate handler for the current state (e.g., `HandleAttackState`).

### Combat Actions
- `PerformAttack()`: A placeholder function intended to be implemented in Blueprints. This should contain the logic for the AI's attack (e.g., playing an animation, dealing damage).
- `FleeFromCombat()`: A placeholder function intended to be implemented in Blueprints. This should contain the logic for how the AI flees (e.g., finding a safe location).

## Example Usage

In a Behavior Tree, you would create a custom task that gets the `AICombatBehaviorComponent` from the controlled pawn and calls its functions.

**Example BT Task: "Start Attack"**
1. Get the AI Controller's Pawn.
2. Get the `AICombatBehaviorComponent` from the Pawn.
3. Get the target from the Blackboard.
4. Call `SetTarget()` on the component, passing in the target.
5. Call `EnterCombatState()` on the component with `ECombatState::Attack`.

## Integration Points
- **Behavior Trees:** This component is designed to be driven by a Behavior Tree. The BT makes the high-level decisions (e.g., "attack this target") and uses the component's functions to execute the low-level actions.
- **`ADAAIBaseCharacter`**: This component should be attached to an `ADAAIBaseCharacter` or a Blueprint subclass thereof.
- **`UStatlineComponent`**: The component will likely need to interact with the character's `UStatlineComponent` to check for things like health (to decide when to flee) or stamina.