COPAL - Community Physics Additions for Lumberyard

Version: 0.4.1

-------------------------------------------------------------------------------------------------

Installation notes:

1) Extract this zip into your Amazon\Lumberyard\1.9.0.0\dev\Gems folder.
2) In the project configurator add this Gem to your project
3) Build the project.

-------------------------------------------------------------------------------------------------

Changelog:
0.4.1 - Global areas out, Ignore params in.

- Removed the global areas for gravity and buoyancy and replaced them with flags on aggregators.

-------------------------------------------------------------------------------------------------
0.4.0 - Force struct and Force tag system

- Moved all forces and torques to be represented as a "Force" struct
- Fixed gravity not working properly at times
- Fixed forces not respecting "Enable" tags
- Added tag system to filter forces by tags, aggregators only respond to forces with their tags

-------------------------------------------------------------------------------------------------
0.3.0 - Expanded Examples

- Added an Electromagnetic Force Aggregator as an example of new force "types"
- Added a fixed strengh local Electromagnetic field (force component)
- Added a local gravitational acceleration field (force component)
- added a simple spring force with a variable k value and rest length (force component)

-------------------------------------------------------------------------------------------------

0.2.0 - First release as Gem

- Moved into a Gem
- Fixed Simple Oscillator exploding in distance (actions apply immediately)

-------------------------------------------------------------------------------------------------

0.1.0 - Initial release

-------------------------------------------------------------------------------------------------

Autors: 
EladDv

-------------------------------------------------------------------------------------------------
This work is licensed under the GPL-3.0+ license