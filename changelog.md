# r55 Tank tower updates
## Client
- Fixed a packet handling fluke

## Server
- Towers can now apply debuffs to enemies (speed debuff, dps, etc)

## Multiplayer
- Clients can now send money to each other

## Tank Tower
- Incendiary shells now work
- Fixed Stun lock shell shock and biggest one being in the wrong unit (s instead of ms)

# r54 Tank tower updates
## Tank Tower
- Stun lock now works.
- Shell shock now works.
- The biggest one now works.

# r53 QOL updates
## Server
- Improved hitboxes
- Fixed a bug where some enemy spawns would occur far before the start of the map

## Client
- Added a way to lower the volume. Thank f***

# r50 Sounds
## Client

- Added enemy kill sound effects (some enemy types only for now)
- Added tower placement sound effects
- Added error sound effect
- Localhost now works
- Fixed an issue that lead to massive desync when frame rate was too high

## General

- Fixed a crash that happened when execfolder was called on a non existing folder.
- Fixed general weirdness of upgrade menu
- Fixed memory leak in upgrade menu

# r46 Removed useless VisualStudio files

# r45 Linux updates

- Permission updates
- ./configure

# r44 Cleanup

- Cleanup

# r43 Local server WIP
## Client

- Local server & single player WIP

# r42 Slight balance updates
## Server

- Fixed a crash that only occurred on Linux hosts
- Enemy spawns are now spread out

## Balancing
### Other

- Reduced amount of enemies in rounds 38 and 39
- Reduced kill cash multiplier in rounds 35 and up
- Increased kill cash multiplier in rounds 1 to 8

# r41 Client updates
## Client

- Client now shows purchased upgrades even when tower can no longer be upgraded
- Added a way to set the targeting priority
- Fixed full screen windowed being glitchy

## Server

- Server now lets clients set targeting priority
- Fixed an issue with projectiles with penetrating power targeting an enemy's spawns

# r40 Bug Fixes
## Client

- Fixed a game crash on start of rounds

## Server

- Fixed towers placed before a player joined not being visible for that player

## Balancing
### Sentry tower

- Increased base price from 185.00 to 200.00
- Increased base max hits from 1 to 2
- Increased base cost of double barrel to 500.00
- Increased base price of hotter gunpowder to 800.00
- Increased base price of explosive ammunition to 3000.00
- Increased base price of rifled barrels to 1750.00
- Increased base price of copper jacket to 420.00
- Increased base price of larger caliber to 1800.00
- Increased base price of hardened steel tripod to 4500.00
- Increased base price of steel penetrator cores to 6000.00
- Decreased damage modifier of larger caliber to 1.75
- Decreased fire rate modifier of hardened steel tripod to 1.05
- Decreased damage modifier of hardened steel tripod to 1.75
- Decreased damage modifier of steel penetrator cores to 1.00

### Tank tower

- Decreased range modifier of larger shells to 1.02
- Decreased range modifier of heavy shells to 1.02
- Decreased range modifier of projectile enlargment pills to 1.15
- Decreased range modifier of modern armor to 1.15
- Decreased range modifier of extremely modern armor to 1.20
- Decreased range modifier of Panzerkampfläufer to 1.40

### Other

- Significantly nerfed most rounds
- Kill cash multiplier now falls to 0.8 at round 30 instead of 35

# r39 Bug fixes and autoconf/automake scripts
## Server

- Fixed desync issue when reconnecting to server
- Slightly increased projectile despawn range
- Fixed enemy spawning being out of sync when changing speed / pausing

## Client

- Client now synchronizes with the server at the end of every round

## Linux

- Replaced Makefile with proper autoconf/automake scripts (create `./configure` with `autoreconf --verbose --install --force`)

# r38 Bug fixes and balancing updates
## Server

- Fixed explosion damage not working correctly
- Fixed projectiles not despawning when going out of range
- Marginal performance improvements

## Client

- Fixed range texture not updating when purchasing a new tower range upgrade

## Balancing updates
### Tank tower

- Doubled base explosion range
- Increased additional explosion range for all explosion range upgrades
- Increased base cost of bigger shells upgrade to 350.00
- Increased base cost of heavy shells upgrade to 725.00
- Increased base cost of huge explosions upgrade to 1400.00

### Sentry tower

- Decreased base cost of faster firing upgrade to 70.00
- Decreased base cost of ported receiver upgrade to 190.00
- Decreased base cost of copper jacket upgrade to 260.00
- Increased base cost of double barrel upgrade to 420.00
- Increased base cost of the SHREDDER upgrade to 15000.00
- Increased base cost of stealth detection upgrade to 240.00
- Increased base cost of sharpened bullets upgrade to 360.00
- Increased base cost of larger caliber upgrade to 1300.00
- Increased base cost of hardened steel tripod upgrade to 3250.00

### Other

- Increased base start cash from 750.00 to 950.00
- Increased each player's start cash by 25.00 for each player above 1

## Other

- Added rounds 33...39

# r37 Tank tower (WIP)
## Towers

- Added WIP Tank tower

# r36 Permissions update

- +x mode to Linux scripts

# r35 Makefile update

# r34 Bug fixes
## Client

- Client now requests update if last frame took too long to render
- Proper fix for N_PLACETOWER's former crash, removed temporary solution
- Made stealthed enemies translucent to be able to tell them apart until a new texture is added

## Server

- Fixed towers shooting in a random position whenever no enemies are in range
- Much more accurate hitreg

## Balancing updates

- Tweaked amount of enemies and enemy frequencies in a lot of early rounds

## Linux

- Linux run scripts

# r33 Bug fixes
## Client

- Fixed issues on non-1080p resolutions
- Fixed resolution changes not being applied in full screen mode
- Fixed full screen changes not being applied
- Temporary fix for a crash that happened when a new tower is placed by an other client while in the upgrade menu

## Server

- Fixed clients having the same client number - not sure if this was actually an issue in the first place

# r32 Bug fix
## Server

- Fixed enemy "children" being spawned at the start of the path

# r31 Bug fixes
## Server

- Fixed crash on easy mode
- Fixed clients losing tower ownership when a new tower is bought

## Client

- Fixed clients losing tower ownership when a new tower is bought
- Fixed `texture_t::~texture_t()` causing soft crash on shut down

# r30 Multiplayer working
## Client

- Client now works in multiplayer

## Server

- Server now works in multiplayer

# r29 Client multiplayer prep
## Client

- Client nearly ready for first multiplayer tests

# r28 Client packet handling
## Client

- Packet handling

## Server

- Forward information on new clients to other players

# r27 Server-side tower & upgrade purchasing
## Client

- Client now stays connected
- Began packet processing

## Server

- Server-side tower & upgrade purchasing

# r26 Client multiplayer preparation
## Client

- Client can now parse server welcome info
- Fixed texture reference bug
- Client now downloads map from server upon joining

## Server

- Fixed server not actually sending welcome packet

# r25 Server ready to test
## Server

- Server can now load maps
- Server sends map to clients as they join
- Kill reward is spread amongst all players
- Server sends game and player infos when a client joins
- Server now detects when rounds are over and sends roundover packet to clients

# r24 Multiplayer WIP
## Server

- Server can now process enemies, tower and projectiles update cycles

## Gameplay

- Projectiles can now penetrate through enemies
- Projectiles can now explode
- Projectiles can now strip properties
- Enemies can now be shielded, which makes them immune to the first hit they receive regardless of the amount of damage

# r23 Multiplayer WIP
## Bug fixes
### Server

- Server now receives connection and packages correctly
- Server now processes connections and disconnections properly

## Generic

- Server can now be built on Linux (GCC 11.2.0)
- Everything can now be built on 32-bit Windows

# r22 More Multiplayer WIP
## Server

- Began porting single player game logic to server
- More robust server main loop
- Local server can now be started with a simple command
- Added a Linux Makefile

## Client

- Added command to connect to server

# r21 Multiplayer
## Server

- Packet handling
- Connection/disconnection

# r20 Bugfix
## Bug fixes
### Generic

- Fix typo in Network.hpp

# r19 Multiplayer
## Miscellaneous

- Fixed wrong compile order on VisualStudio
- Updated CXXGUI to r57
- Began working on dedicated server

# r18 Multiplayer begin
## Gameplay

- Began working towards multiplayer support

## Miscellaneous

- Began restructuring code for multiplayer support. (This is not finished!)

# r17 Language support, more rounds
## Gameplay

- Added convenient way to allow for future support of additional languages
- Added rounds 21 ... 30
- Increased spacing between spawns of enemies
- Stopped cmd window from opening on Release build

## Rounds balancing updates

- Reduced amount of enemies in rounds 1 ... 6, 8, 9, and 19

## Bug fixes
### Generic

- Fixed enemies not taking damage when tower does less damage than required to kill an enemy
- Fixed bounding boxes of upgrade menus being wrong and resulting in it being difficult to purchase upgrades when menu was on left side

### Sentry turret

- Fixed stealth detection and sharpened bullets upgrades being swapped

# r16 Balancing updates, more rounds, stealthed enemies
## Gameplay

- Stealthed enemies + targeting of stealthed enemies
- Added spacing between spawns of enemies
- Added rounds 16 ... 20

## Rounds balancing updates

- Changed amount of enemies in rounds 9 ... 12 and 15
- Changed enemy spacing in rounds 9 and 11

## Towers balancing updates
### Sentry turret

- Reduced base cost from 200.00 to 185.00
- Reduced base cost of faster firing from 80.00 to 75.00
- Reduced base cost of ported receiver from 240.00 to 200.00
- Reduced base cost of hotter gunpowder from 800.00 to 640.00
- Reduced base cost of AP rounds from 750.00 to 725.00
- Reduced base cost of longer barrel from 800.00 to 700.00
- Reduced base cost of rifled barrel from 1200.00 to 1150.00
- Reduced base cost of copper jacket from 300.00 to 275.00
- Reduced base cost of mana infusion from 12500.00 to 11500.00
- Increased base cost of explosive ammunition from 2500.00 to 2900.00
- Increased base cost of special ammunition from 12000.00 to 15000.00
- Increased base cost of hardened steel tripod from 2100.00 to 3100.00
- Increased base cost of steel penetrator cores from 3500.00 to 4500.00
- Increased fire rate buff of ported receiver from 33% to 38%
- Increased maximum amount of enemies damaged by explosive ammunition from 8 to 12
- Increased explosive ammunition's explosion range from 4.0 to 6.0
- Increased fire rate buff of special ammunition from 5% to 10%
- Increased damage buff of special ammunition from 13% to 25%
- Increased steel penetrator core penetration range from 16.0 to 128.0
