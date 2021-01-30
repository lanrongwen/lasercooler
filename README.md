# lasercooler
An Arduino Controller for hacking a Counter Top Ice Maker into a Cooler for a k40 Laser.

### Initial Implementation

This project is starting simple, using a spare Arduino Mega and relay board, it will control the power to a counter top ice maker's compressor and cooling fan to control the temperature of the water used to cool a laser.  Initial features:
* A RGB LED to help indicate status at a glance.
   * White - Just powered on, not running yet.
   * Red - Stopped
   * Blue - Cooling
* LCD Display with Current temperatures and state of the cooler.
* 2 Sensors to pick up the temperatures.
* A Start/Stop button
* A pair of relays to power the compressor and compressor fan.


### Project Background
Buying the K40 was a big investment as far as hobbies for me.  I wanted a proper cooler as it gets quite hot during the summer, but all of them I found either weren't good enough or cost too much money (as much as the laser itself).

I found a table top ice maker on clearance and saw other people talk about using them.  I had a bunch of components laying around, and decided to put the two together into this.  I also added a few things just for the fun of it.

### How To Build
1. [Parts List](/physicalBuild/ComponentList.md)
1. [Build Steps](/physicalBuild/Build.md)

### Future Enhancements
* Add Heater Control for the cold winter months to bring it up to operating temperature.

### Parting Notes:
This is a hobby only, but I'm always open to improvements and making the code better.  Feel free to help contribute back in.  Happy Lasering!