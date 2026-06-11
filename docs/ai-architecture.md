# AI Architecture — Command & Conquer Generals

This document describes the design of the opponent AI system in Generals. The system is organized into three tiers — **strategic** (player-level), **tactical** (group-level), and **execution** (unit-level) — plus cross-cutting subsystems for pathfinding, turrets, and data-driven configuration.

---

## Source layout

```
Generals/Code/GameEngine/
├── Include/GameLogic/
│   ├── AI.h                  — TheAI singleton, AIGroup, AICommandInterface, AttitudeType
│   ├── AIPlayer.h            — Computer opponent player (strategic tier)
│   ├── AISkirmishPlayer.h    — Skirmish-mode extension of AIPlayer
│   ├── AIStateMachine.h      — All AIStateType enum values + state machine base
│   ├── AIGuard.h             — Guard state machine
│   ├── AIPathfind.h          — Pathfinder interface
│   ├── TurretAI.h            — Turret tracking/firing
│   └── Module/AIUpdate.h     — Per-unit AI update module, AutoAcquireStates, LocomotorSetType
└── Source/GameLogic/AI/
    ├── AI.cpp                — TheAI singleton implementation
    ├── AIPlayer.cpp          — Strategic AI (~118 KB)
    ├── AISkirmishPlayer.cpp  — Skirmish strategy (~38 KB)
    ├── AIStates.cpp          — All state machine behaviors (~235 KB)
    ├── AIGroup.cpp           — Group pathfinding and coordination (~90 KB)
    ├── AIPathfind.cpp        — Pathfinding algorithms (~305 KB)
    ├── AIGuard.cpp           — Guard behavior states (~30 KB)
    ├── TurretAI.cpp          — Turret AI (~48 KB)
    ├── AIDock.cpp            — Docking behavior (~26 KB)
    └── AITNGuard.cpp         — Tunnel-network guard behavior (~31 KB)
```

INI configuration is parsed in `Source/Common/INI/INIAiData.cpp`.

---

## System overview

```
┌─────────────────────────────────────────────────────────┐
│                     TheAI singleton                      │
│  • owns Pathfinder                                       │
│  • owns list of AIGroups                                 │
│  • owns TAiData (global numeric parameters)              │
│  • findClosestEnemy / findClosestAlly / findClosestRepulsor│
└────────────────┬────────────────────────────────────────┘
                 │
    ┌────────────┴─────────────┐
    │                          │
┌───▼────────┐        ┌────────▼──────────────────────────┐
│  AIPlayer  │        │  AIGroup                          │
│ (per CPU   │        │  • collection of Object*           │
│  player)   │        │  • group movement & formation      │
│            │        │  • delegates commands to members   │
│ AISkirmish-│        └────────────┬──────────────────────┘
│ Player     │                     │ (each member)
└────────────┘           ┌─────────▼─────────────────────┐
                         │  AIUpdate (per-unit module)    │
                         │  • drives AIStateMachine       │
                         │  • auto-acquire logic          │
                         │  • locomotor selection         │
                         │  • optional TurretAI           │
                         └────────────────────────────────┘
```

---

## Tier 1 — Strategic AI (`AIPlayer` / `AISkirmishPlayer`)

**Files:** `AIPlayer.h/.cpp`, `AISkirmishPlayer.h/.cpp`

`AIPlayer` represents a single computer-controlled player. Its main loop is called every frame and drives three recurring decisions:

| Decision | Key method | Gating condition |
|---|---|---|
| Build a structure | `doBaseBuilding()` | Timer fires every `m_structureSeconds` |
| Train a team | `doTeamBuilding()` | Timer fires every `m_teamSeconds` |
| Research an upgrade | `doUpgradesAndSkills()` | Resources available |

Both timers are scaled by wealth modifiers (`m_structuresWealthyMod`, `m_teamWealthyMod`, `m_structuresPoorMod`, `m_teamPoorMod`) so a rich AI builds faster and a broke AI slows down automatically.

### Team building pipeline

1. `selectTeamToBuild()` — picks the next team prototype from the faction's build list.
2. `isAGoodIdeaToBuildTeam()` — sanity-checks resources and factory availability before committing.
3. A `TeamInQueue` record is created, with one `WorkOrder` per unit in the team pairing each unit type with a factory.
4. `queueUnits()` — assigns pending `WorkOrder`s to idle production buildings.
5. When a unit finishes, `WorkOrder::m_numCompleted` is incremented; when the whole team is ready, `TeamInQueue` moves from the build queue to the ready queue and gets sent to a start location.

`AISkirmishPlayer` extends this with auto-enemy detection: at startup it scans for the nearest enemy player and stores it, replacing the need for a hand-authored enemy assignment in a skirmish map.

### Superweapon targeting

`computeSuperweaponTarget()` scores enemy clusters and returns the position that maximises expected damage before the AI fires a superweapon (nuclear missile, particle cannon, etc.).

### Resource model

```
m_resourcesWealthy  — above this supply count → wealthy (build faster)
m_resourcesPoor     — below this supply count → poor (build slower)
```

Supply gatherer counts per supply center are faction-specific and difficulty-dependent (`AISideInfo.m_easy`, `.m_normal`, `.m_hard`).

---

## Tier 2 — Tactical AI (`AIGroup`)

**Files:** `AI.h`, `AIGroup.cpp`

An `AIGroup` is a reference-counted set of `Object*` pointers managed by `TheAI`. Groups are created with `TheAI->createGroup()` and destroyed with `TheAI->destroyGroup()`.

Groups exist to enable **coordinated movement**:

- `getSpeed()` returns the speed of the slowest member; the whole group moves at that pace so nobody outruns their formation.
- `getCenter()` and `getMinMaxAndCenter()` compute the group's centroid for destination calculations.
- `computeIndividualDestination()` maps a single group-level move order to a per-unit offset position, maintaining loose formation.
- `friend_computeGroundPath()` computes a single shared ground path for the group and distributes it.

Every high-level group command (e.g. `groupMoveToPosition`, `groupAttackObject`, `groupHunt`) iterates `m_memberList` and calls the equivalent `AICommandInterface` method on each member. The group itself does not execute behavior — it fans orders out to unit-level AI.

The `m_dirty` flag triggers a recompute of speed and leader on the next frame after membership changes.

---

## Tier 3 — Unit AI (`AIUpdate` / `AIStateMachine`)

**Files:** `AIUpdate.h`, `AIStateMachine.h`, `AIStates.cpp`

Every game object that can act has an `AIUpdate` module attached. `AIUpdate` is an `UpdateModule` that:

1. Owns an `AIStateMachine` instance.
2. Processes incoming `AICommandParms` and translates them into state transitions.
3. Runs the auto-acquire scan on idle units.
4. Selects the appropriate `LocomotorSet` for the current behavior.
5. Optionally owns a `TurretAI` for objects with independently-rotating weapons.

### State machine

`AIStateMachine` is a hierarchical finite state machine. Each `AIStateType` enum value (defined in `AIStateMachine.h`) corresponds to a concrete state class implemented in `AIStates.cpp`. Full list of major states:

| Category | States |
|---|---|
| Idle / waiting | `AI_IDLE`, `AI_WAIT`, `AI_BUSY`, `AI_DEAD` |
| Movement | `AI_MOVE_TO`, `AI_FOLLOW_WAYPOINT_PATH_AS_TEAM`, `AI_FOLLOW_WAYPOINT_PATH_AS_INDIVIDUALS`, `AI_FOLLOW_PATH`, `AI_MOVE_AND_TIGHTEN`, `AI_MOVE_AND_EVACUATE`, `AI_MOVE_AWAY_FROM_REPULSORS`, `AI_MOVE_OUT_OF_THE_WAY` |
| Combat | `AI_ATTACK_OBJECT`, `AI_FORCE_ATTACK_OBJECT`, `AI_ATTACK_AND_FOLLOW_OBJECT`, `AI_ATTACK_POSITION`, `AI_ATTACK_MOVE_TO`, `AI_ATTACK_AREA`, `AI_HUNT`, `AI_ATTACK_SQUAD` |
| Guarding | `AI_GUARD`, `AI_GUARD_TUNNEL_NETWORK` |
| Special actions | `AI_DOCK`, `AI_ENTER`, `AI_EXIT`, `AI_RAPPEL_INTO`, `AI_COMBATDROP`, `AI_HACK_INTERNET` |
| Reactionary | `AI_PANIC`, `AI_WANDER`, `AI_GO_PRONE`, `AI_PICK_UP_CRATE` |

State transitions are driven by:
- Explicit commands arriving via `AICommandInterface::aiDoCommand(AICommandParms*)`.
- Internal events (target lost, path finished, damage received).
- Auto-acquire scans (idle units looking for enemies).

### Attitude / mood system

Each unit has an `AttitudeType` that widens or narrows its effective vision and reaction range:

| Attitude | Vision multiplier | Behaviour |
|---|---|---|
| `ATTITUDE_SLEEP` (-2) | 0 | Never responds autonomously |
| `ATTITUDE_PASSIVE` (-1) | reduced | Minimal self-defence |
| `ATTITUDE_NORMAL` (0) | ×1.0 | Standard scan/attack |
| `ATTITUDE_ALERT` (+1) | ×`m_alertRangeModifier` | Extended scan range |
| `ATTITUDE_AGGRESSIVE` (+2) | ×`m_aggressiveRangeModifier` | Extended attack range |

`AI::getAdjustedVisionRangeForObject()` applies the appropriate multiplier based on the combination of attitude, owner type (AI vs. human), and whether guard inner/outer radius rules apply.

### Auto-acquire

When a unit is idle for at least `m_forceIdleFramesCount` frames, it scans for enemies using `TheAI->findClosestEnemy()`. The set of valid targets is controlled by `AutoAcquireStates` flags:

| Flag | Meaning |
|---|---|
| `AAS_Idle` | Attack any idle enemy in range |
| `AAS_Idle_Stealthed` | Also attack stealthed enemies |
| `AAS_Idle_No` | Never auto-acquire |
| `AAS_Idle_Not_While_Attacking` | Only auto-acquire when truly idle |
| `AAS_Idle_Attack_Buildings` | Include structures as valid targets |

### Locomotor sets

`AIUpdate` switches between named `LocomotorSet`s to express different movement modes without changing the unit's stats:

| Set | Used for |
|---|---|
| `LOCOMOTORSET_NORMAL` | Everyday movement |
| `LOCOMOTORSET_FREEFALL` | Airborne / parachuting |
| `LOCOMOTORSET_WANDER` | Civilian wander |
| `LOCOMOTORSET_PANIC` | Panicking civilians |
| `LOCOMOTORSET_TAXIING` | Aircraft on the ground |
| `LOCOMOTORSET_SUPERSONIC` | High-speed attack runs |
| `LOCOMOTORSET_SLUGGISH` | Damaged / slow movement |

---

## Target selection (`TheAI->findClosestEnemy`)

**File:** `AI.h`, `AI.cpp`

```cpp
Object *findClosestEnemy(
    const Object *me,
    Real range,
    UnsignedInt qualifiers,
    const AttackPriorityInfo *info = nullptr,
    PartitionFilter *optionalFilter = nullptr
);
```

The search is spatially bounded (world partition) and then filtered by `qualifiers`:

| Qualifier | Effect |
|---|---|
| `CAN_SEE` | Target must be visible |
| `CAN_ATTACK` | Caller must have a weapon that reaches the target |
| `IGNORE_INSIGNIFICANT_BUILDINGS` | Skip buildings flagged as unimportant |
| `ATTACK_BUILDINGS` | Include structures in the result set |
| `WITHIN_ATTACK_RANGE` | Target must be inside weapon range right now |
| `UNFOGGED` | Target must not be inside fog of war |

When `AttackPriorityInfo` is supplied, each candidate is scored:

```
effective_priority = base_priority − (distance / m_attackPriorityDistanceModifier)
```

Higher priority wins. The distance penalty means a high-value far target can lose to a lower-value close target, producing naturally aggressive behaviour rather than tunnel-vision on a distant headquarters.

There are two companion methods:

- `findClosestAlly()` — same interface, used for healing/repairing decisions.
- `findClosestRepulsor()` — finds the nearest object that is "repulsing" nearby units (corpses, explosions) so civilians can flee away from them.

---

## Guard behavior (`AIGuard` / `AITNGuard`)

**Files:** `AIGuard.h/.cpp`, `AITNGuard.h/.cpp`

Guard behavior is implemented as a nested state machine (`AIGuardMachine`) that activates when a unit receives `AICMD_GUARD_POSITION`, `AICMD_GUARD_OBJECT`, or `AICMD_GUARD_AREA`.

The guard machine maintains two concentric radii derived from the unit's vision range:

| Radius | Multiplier parameter | Purpose |
|---|---|---|
| Inner | `m_guardInnerModifierAI` / `m_guardInnerModifierHuman` | Unit pursues an enemy that enters this circle |
| Outer | `m_guardOuterModifierAI` / `m_guardOuterModifierHuman` | Unit gives up the chase and returns when enemy exits this circle |

Enemy scanning while guarding runs at `m_guardEnemyScanRate` (normal guard) or `m_guardEnemyReturnScanRate` (returning to post). Chase duration is capped at `m_guardChaseUnitFrames`.

`AITNGuardMachine` is a variant for units inside tunnel networks that must guard without being directly visible on the map.

---

## Turret AI (`TurretAI`)

**Files:** `TurretAI.h/.cpp`

Objects with independently-rotating weapons (tanks, bunkers, stationary guns) have a `TurretAI` instance embedded inside their `AIUpdate`. The turret runs its own `TurretStateMachine` in parallel with the main unit state machine.

### Turret states

| State | Description |
|---|---|
| `TURRETAI_IDLE` | No target; turret is stationary |
| `TURRETAI_IDLESCAN` | No target; sweeping to scan |
| `TURRETAI_AIM` | Rotating toward a target |
| `TURRETAI_FIRE` | On target; weapon firing |
| `TURRETAI_RECENTER` | Target lost; returning to natural angle |
| `TURRETAI_HOLD` | Brief hold before recentering |

### Turret configuration (per `TurretAIData`)

- Turn rate and pitch rate (degrees per frame)
- Natural/default facing angle
- Min and max idle-scan angles (defines the sweep arc)
- Fire angle sweep — allows the turret to keep shooting while traversing slightly
- Recenter delay (frames to wait before snapping back after losing a target)

---

## Docking behavior (`AIDock`)

**File:** `AIDock.cpp`

Docking handles units that must physically align with a structure before transferring resources or passengers (supply trucks pulling into a supply center, helicopters landing on a pad). The dock state machine negotiates queue position with the building's `DockUpdate` module and guides the unit along a precise approach path.

---

## Pathfinding (`AIPathfind`)

**Files:** `AIPathfind.h`, `AIPathfind.cpp` (~305 KB)

`Pathfinder` (owned by `TheAI`) provides grid-based pathfinding for ground and air units. Key configuration from `TAiData`:

| Parameter | Meaning |
|---|---|
| `m_infantryPathfindDiameter` | Cell-width of paths for infantry |
| `m_vehiclePathfindDiameter` | Cell-width of paths for vehicles |
| `m_minInfantryForGroup` | Minimum unit count before group pathfinding activates |
| `m_minVehiclesForGroup` | Same threshold for vehicles |
| `m_minDistanceForGroup` | Short moves skip group pathfinding |
| `m_distanceRequiresGroup` | Long moves always force group pathfinding |
| `m_minClumpDensity` | Density threshold that constitutes a "clump" |

Group pathfinding uses `AIGroup::friend_computeGroundPath()` to compute a single shared path, then `computeIndividualDestination()` offsets each member to a slot in formation.

---

## Data-driven configuration (`TAiData` / INI)

**Files:** `AI.h` (`TAiData`), `INIAiData.cpp`

Almost all numeric AI parameters are loaded from INI rather than hardcoded. The `TAiData` struct is the in-memory representation; `AI::parseAiDataDefinition()` populates it.

### Global parameters (excerpt)

| Field | Description |
|---|---|
| `m_structureSeconds` | Base interval between base-building decisions |
| `m_teamSeconds` | Base interval between team-building decisions |
| `m_resourcesWealthy` / `m_resourcesPoor` | Supply thresholds that scale build timers |
| `m_teamResourcesToBuild` | Fraction of team cost that must be in bank before queuing |
| `m_attackPriorityDistanceModifier` | Distance (feet) per 1 point of priority reduction |
| `m_repulsedDistance` | How far civilians flee past their vision range |
| `m_aiCrushesInfantry` | Whether AI vehicles attempt to run over infantry |
| `m_rebuildDelaySeconds` | Cooldown after a building is destroyed before AI rebuilds it |
| `m_supplyCenterSafeRadius` | Radius scanned to decide if a supply center is safe |

### Per-faction configuration (`AISideInfo`)

Each playable faction (side) has an `AISideInfo` block:

| Field | Description |
|---|---|
| `m_easy/m_normal/m_hard` | Gatherer count per supply center at each difficulty |
| `m_skillSet1…5` | Ordered lists of science upgrades for each difficulty tier |
| `m_baseDefenseStructure1` | Template name of the base-defense building to construct |

### Per-faction build lists (`AISideBuildList` / `BuildListInfo`)

Each faction has a prioritized build list of structures and unit teams. `doBaseBuilding()` and `selectTeamToBuild()` walk these lists when making production decisions.

---

## Difficulty system

**File:** `Common/GameCommon.h`

```cpp
enum GameDifficulty {
    DIFFICULTY_EASY,
    DIFFICULTY_NORMAL,
    DIFFICULTY_HARD,
    DIFFICULTY_COUNT
};
```

Difficulty affects:

1. **Economy** — `AISideInfo.m_easy/m_normal/m_hard` controls how many supply gatherers the AI deploys. More gatherers → faster income.
2. **Weapon bonuses** — `WEAPONBONUSCONDITION_SOLO_AI_EASY/NORMAL/HARD` applies damage/rate-of-fire modifiers on top of normal weapon stats.
3. **Skill acquisition** — Each difficulty tier selects a different `TSkillSet` from `AISideInfo`, unlocking different upgrades.

Difficulty does **not** change the AI's decision-making logic; it adjusts the resources and bonuses available to the AI player.

---

## Command flow summary

The following shows how a high-level player-AI decision reaches a unit executing an attack:

```
AIPlayer::doTeamBuilding()
  → builds team, sends to attack waypoint
      → AIGroup::groupAttackMoveToPosition()
          → per unit: aiDoCommand(AICMD_ATTACKMOVE_TO_POSITION)
              → AIStateMachine transitions to AI_ATTACK_MOVE_TO
                  → on contact: transitions to AI_ATTACK_OBJECT
                      → TheAI->findClosestEnemy() (with AttackPriorityInfo)
                          → TurretAI aims and fires
```

Scripts or the player can inject commands at any level — directly to an `AIGroup` or to an individual unit's `AICommandInterface` — which is how triggered scripts override autonomous AI behavior during scripted missions.
