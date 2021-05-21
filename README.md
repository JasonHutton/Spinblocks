# Spinblocks
## Jason Hutton

## Compiling
Windows is Spinblocks' build target.
Only x64 builds will work.
Please compile using Microsoft Visual Studio 2019.
Windows 10 SDK v10.0.18362.0 is required. (Though this can probably be trivially changed.)
No special setup should be required beyond this.

## Git Repository
Github repository is publicly accessible, here: https://github.com/JasonHutton/Spinblocks.git
The main (AKA master) branch should be used to build from.

## Controls
Controls are as follows:
- Move Left: Left Arrow
- Move Right: Right Arrow
- Soft Drop: Down Arrow
- Hard Drop: Up Arrow
- Rotate Counter-Clockwise: Z
- Rotate Clockwise: X
- Pause: Pause
- Menu: Escape

## Gameplay


## Known Issues
### Major
* Blocks in non-adjecant rows sometimes do not collapse properly during a board rotation.
* Tetrominos can be rotated through obstructions in some circumstances.
* Tetromino rotation can be obstructed when it should not be.

### Minor
* Tetrominos spawn into the play area at orientations that are not intended. (Should always be North-facing.)
* Window resolution not designed to be changed, so may behave undesirably.
* Controls are handled improperly when simultaneously pressed. 
* Timers are not halted when the game is paused, though they do not elapse multiple times.
* Falling timer may be out of sync with initial block spawn, so initial movement may be of shorter duration than it should be.
* Tetrominos can lock down in midair after sliding across existing blocks.
* It's possible to interact with background menu windows in unintended ways.
* The Play Area Matrix takes longer than intended to rotate.

### Not Implemented in Order to Prioritize Other Tasks
* Menu settings are not saved/loaded. (By extension, controls may not be changed.)
* High scores are not saved/loaded.
* Directional Censor blocks are not setup for the Play Area Matrix, leaving buffer areas un-covered.