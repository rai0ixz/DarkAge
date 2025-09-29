#pragma once

#include "CoreMinimal.h"
#include "NPCPersonalityData.h"
#include "SamplePersonalities.generated.h"

// This file contains sample personality data that can be used to create Data Assets
// in the Unreal Editor. Copy these values when creating new personality assets.

UCLASS()
class DARKAGE_API USamplePersonalities : public UObject
{
    GENERATED_BODY()

public:
    // Sample data for creating personality assets in the editor

    /*
    === BRAVE WARRIOR PERSONALITY ===
    PersonalityName: "Brave Warrior"
    Description: "A courageous fighter who faces danger head-on"

    PrimaryTraits: Brave, Aggressive, Hardworking
    SecondaryTraits: Honest, Ambitious

    Modifiers:
    - HungerDecayModifier: 1.2 (eats more due to activity)
    - SafetyDecayModifier: 0.8 (less afraid)
    - RiskTolerance: 0.8
    - WorkEthic: 0.9

    GreetingLines:
    - "Ready for battle, friend!"
    - "A warrior's greeting to you!"

    AngryLines:
    - "You dare challenge me?!"
    - "This insult demands satisfaction!"

    FriendlyLines:
    - "It's an honor to fight alongside you."
    - "Your courage impresses me."

    PreferredActivities: "Combat", "Training", "Patrol"
    DislikedActivities: "Idle chatter", "Politics"

    === COWARDLY MERCHANT PERSONALITY ===
    PersonalityName: "Cowardly Merchant"
    Description: "A nervous trader who avoids conflict and values safety"

    PrimaryTraits: Cowardly, Greedy, Peaceful
    SecondaryTraits: Deceitful, Lazy

    Modifiers:
    - SafetyDecayModifier: 1.3 (very concerned about safety)
    - SocialDecayModifier: 0.9 (prefers company for protection)
    - RiskTolerance: 0.2
    - Generosity: 0.3

    GreetingLines:
    - "Oh, hello there! You startled me!"
    - "Welcome, welcome! No trouble, I hope?"

    AngryLines:
    - "This is most upsetting!"
    - "Please, let's not make a scene..."

    FriendlyLines:
    - "It's so nice to have peaceful visitors."
    - "Your presence brings me comfort."

    PreferredActivities: "Trading", "Counting money", "Hiding"
    DislikedActivities: "Fighting", "Dangerous travel"

    === HARDWORKING FARMER PERSONALITY ===
    PersonalityName: "Hardworking Farmer"
    Description: "A dedicated worker who values honest labor"

    PrimaryTraits: Hardworking, Honest, Content
    SecondaryTraits: Generous, Peaceful

    Modifiers:
    - HungerDecayModifier: 1.1 (physical work increases appetite)
    - RestDecayModifier: 1.1 (tired from work)
    - WorkEthic: 0.9
    - SocialPreference: 0.6

    GreetingLines:
    - "Good day to you! Fine weather for work."
    - "Hello there, friend. The fields are calling."

    AngryLines:
    - "That's not right at all!"
    - "I won't stand for such dishonesty!"

    FriendlyLines:
    - "It's good to see an honest face."
    - "Hard work brings its own rewards."

    PreferredActivities: "Farming", "Animal care", "Community work"
    DislikedActivities: "Laziness", "Deception"

    === SOCIAL TAVERN KEEPER PERSONALITY ===
    PersonalityName: "Social Tavern Keeper"
    Description: "A friendly host who loves company and conversation"

    PrimaryTraits: Social, Generous, Peaceful
    SecondaryTraits: Hardworking, Content

    Modifiers:
    - SocialDecayModifier: 0.7 (needs less social interaction)
    - RestDecayModifier: 1.2 (late nights)
    - SocialPreference: 0.9
    - Generosity: 0.8

    GreetingLines:
    - "Welcome, welcome! Come in and rest your bones!"
    - "Ah, a new face! The more the merrier!"

    AngryLines:
    - "Now that's no way to behave in my establishment!"
    - "I won't have fighting in here!"

    FriendlyLines:
    - "It's always good to see familiar faces."
    - "Stay awhile and share some stories!"

    PreferredActivities: "Serving drinks", "Conversation", "Music"
    DislikedActivities: "Loneliness", "Boredom"
    */

    // Use this data to create personality assets in the Unreal Editor
    // Go to Content -> Data Assets -> Create NPCPersonalityData
    // Then copy the values from the comments above
};