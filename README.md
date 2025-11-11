# Into The Frontrooms � Game Design Document (GDD)

Fear what is not Human

Version 1.0 � November 03, 2025
Author: Joshua Desroches (IdolMinds)

---

## Table of Contents
- Overview
- Design Philosophy
- Feature Summary
 - General Features
 - Multiplayer Features
 - Editor
 - Gameplay
- The Game World
- Rendering & Engine
- Camera & Controls
- Game Characters
 - Player
 - Enemies
- User Interface
- Modes & Gameplay Flow
 - Single-player
 - Multiplayer
 - Survival / Highscore
- Implementation Notes & Defaults
- Editor / Tools
- QA & Testing Checklist
- Appendix: Assets & Packaging Notes

---

## Overview

`Into The Frontrooms` is a multiplayer/co-op horror exploration game where players navigate a large atmospheric world, collect narrative notes, avoid hostile entities, and survive as long as possible.

Tagline: "Fear what is not Human."

---

## Design Philosophy

- Minimal UI, maximal immersion.
- Cooperative, emergent multiplayer scares and tactics.
- Fast iteration for designers through blueprint-exposed properties and editor tools.

---

## Feature Summary

### General Features
- Huge world with modular zones and level streaming.
- Server-authoritative AI using Unreal NavMesh .
- Roaming/chase AI in RoamingAIController.
- Player movement based on `ACharacter` with configurable speeds and inputs.
- Pickups: health, and notes.
- VFX via Niagara and Cascade legacy.
- Audio feedback to improve interaction clarity.

### Multiplayer Features
- Up to 4 players, server-authoritative.
- AI targets closest player.
- Simple, friendly UI and shared scare events.

### Gameplay
- Explore a large valley, collect notes to uncover story, and avoid/escape enemies.
- Environment interaction with pickups and basic physics props.
---

## The Game World

### Overview
Large, nonlinear environments with hidden paths, verticality, and environmental storytelling. Levels are designed to encourage exploration while offering safe/unsafe zones.

### Key Locations
- Note Sites: where narrative notes spawn.
- High-Risk Areas: choke-points where AI encounters are likely.

### Travel & Scale
- Player traverses by foot; vehicle traversal is out of scope.

### Objects
- Pickup actors: `NotePickup`, `HealthPickup`.

---

## Rendering & Engine

- UE5 engine: Niagara for particles, Lumen (or baked lighting) depending on target performance.
- Materials tuned for horror tone: desaturated palettes, high contrast, subtle post-process.

---

## Camera & Controls

- First-person perspective with 90 degree FOV.
- Controls: WASD movement, Space to jump, escape or p key(s) to open pause menu (does not pause game due to multiplayer functionality).

---

## Game Characters

### Player
- Controlled via `IntoTheFrontroomsCharacter` class.
- Collects notes, pickups, and can be damaged by AI.

### Enemies
- `ARoamingAICharacter` driven by `RoamingAIController`.
- Behaviors: roam, chase, attack, respawn with VFX/SFX.
- Configurable fields exposed to designers (sight range, speeds, attack damage/cooldown).

---

## User Interface

- Minimal HUD: health, notes collected, players alive, optional timer/score.
- Notification system on note collection via `OnNoteCollected` blueprint event.

---

## Modes & Gameplay Flow

### Single-player
- Explore, collect notes, avoid AI. Story-driven survival exploration.

### Multiplayer
-2�4 players. Server authoritative. Players cooperate to collect notes and survive AI encounters.

### Survival
- Survive for as long as possible against the enemies and the clock.

---

## Implementation Notes & Defaults

- Defaults (tuneable in `RoamingAICharacter`):
 - `SightRange =1500.0f`
 - `RoamingSpeed =200.0f`, `ChaseSpeed =400.0f`
 - `MaxRoamDistance =1000.0f`, `AcceptanceRadius =100.0f`
 - `AttackRange =150.0f`, `AttackDamage =35.0f`, `AttackCooldown =3.0f`
 - `DespawnSmokeScale =2.0f`, `DespawnSmokeLifetime =2.0f`

- Ensure `Niagara` is added to `IntoTheFrontrooms.Build.cs` if using Niagara in C++.
- AI logic runs server-side; clients receive replicated movement/state.
- Use `ProjectPointToNavigation` when choosing respawn locations.

---

## Editor / Tools

- Content Browser helpers: placement brush, scatter tool for pickups/notes.

---

## QA & Testing Checklist

- Single-player: AI roam/chase/attack, pickups apply, notes collect, respawn with VFX.
- Multiplayer: AI roams & targets nearest player, replication of deaths and pickups, no server crashes.
- Packaging: validate blueprints, fix redirectors, add Niagara to `Build.cs` when used.

---

## Appendix: Assets & Packaging Notes

- Validate `BP_RoamingAI_Reptile` and other blueprints before packaging; corrupted blueprint data can crash packaged builds (bad export index).
- If packaging crashes on startup: remove problematic blueprint instances, recreate corrupted blueprint, ensure all referenced assets exist and are non-editor-only.

---
