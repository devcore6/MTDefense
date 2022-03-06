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
