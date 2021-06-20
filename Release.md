Features in this release:

MIDI:
* MIDI out of drumsounds playing in a pattern
* MIDI select for bank and pattern. Notes 61-68 select the bank, 72-87 select the pattern
* Separate channel for the external instrument, selectable in config page 2

Interface:
* Shift+step in Mute mode solos the instrument (shift+encoder mutes all) [sandormatyi]

Track Write: [Neuromancer]
* Shift+Clear in Track write mode now clears the current track. 
* In Track Write mode pressing Last Step will now place and End Of Track marker at the current track possition. Reaching this marker stops the sequencer on playback.
* It is no longer possible to move the track position beyond the track length.
* You can't move the track position beyond the End of Track marker.
* When using Enter to advance to the next track position the pattern stays the same for track positions. This should make track programming a bit faster.


At startup the display now shows the build date for this release

Bugfixes:
- Issue #12: Flam bug with external instrument and others
- Issue #13: Switching from keyboardMode to config page doesn't work
- Issie #21: Pattern from track is not updated when going to track play mode.
