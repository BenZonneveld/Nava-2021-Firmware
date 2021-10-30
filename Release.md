Features in this release:

October 2021:
This release focuses on pattern groups.

* Pattern groups can now be used in edit mode without losing edits when going to the next pattern in a group.

* When saving edits in a pattern group only edited patterns are save to reduce wear and tear on the eeprom.

* Pattern groups are shown in Pattern Play while not running.

Other changes:
* Also only instrument hits within the pattern length are shown.

* When there are muted instruments and the Nava is not in MUTE mode the MUTE button will be lit dimly.

* MIDI pattern change now only works in pattern play mode.

Older releases:
The Nava Tool Installer for windows is included in this release.


** Nava Tool use **

* Setup your midi ports using the MIDI menu's Midi Port Settings.

* Colored buttons have a Right Click Context menu with the options to load. save. request, dump and edit if available.

* The track editor expects pattern names as displayed on the Nava.  
  Separate patterns with whitespace.
  Use EOT to set the End Of Track marker when you want the Nava to stop playing at the end of the track.



Configuration:

* New option on Config page 2 to select the boot mode for the Nava. Feature #16



MIDI:

* MIDI out of drumsounds playing in a pattern, it is now possible to turn the drumnotes of by selecting "OFF" as the mtx channel on Config page 1.

* MIDI select for bank and pattern. Notes 61-68 select the bank, 72-87 select the pattern.

* Separate channel for the external instrument, selectable in config page 2

* EXT now has 2 levels + total accent implemented. Closes #15

* System Exclusive dumps.
  Sysex only works in the Sysex Page which can be found on Config Page 3.



Muting:

* Shift+step in Mute mode solos the instrument (shift+encoder mutes all) [sandormatyi]

* When using mute in Track play mode the track keeps running and advancing.



Track Write: [Neuromancer]

* Shift+Clear in Track write mode now clears the current track. 

* In Track Write mode pressing Last Step will now place and End Of Track marker at the current track possition. Reaching this marker stops the sequencer on playback.

* It is no longer possible to move the track position beyond the track length.

* You can't move the track position beyond the End of Track marker.

* When using Enter to advance to the next track position the pattern stays the same for track positions. This should make track programming a bit faster.


At startup the display now shows the build date for this release

Bugfixes:
- Issue #35: Expander Mode does not work outside settings.
- Issue #36: Missed MIDI notes (MIDI out).
- Issue #37: Config changes can't be saved.

