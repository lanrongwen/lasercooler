# lasercooler
An Arduino Controller for hacking a Counter Top Ice Maker into a Cooler for a k40 Laser.

### Initial Implementation

This project is starting simple, using a spare Arduino Mega and relay board, it will control the power to a counter top ice maker's compresser and cooling fan to control the temperature of the water used to cool a laser.  Initial features:
* A RGB LED to help indicate status at a glance.
   * White - Just powered on, not running yet.
   * Red - Stopped
   * Blue - Cooling
* A Start/Stop button
* A pair of relays to power the compressor and compressor fan.

### Planned Enhancements
* Temperature sensors that will allow it to automatically control the cooling cycles.
* LCD to display the temps and status updates.
* Docs and how-tos on how I build the system (may even get fancy enough to add some photos and diagrams.)
