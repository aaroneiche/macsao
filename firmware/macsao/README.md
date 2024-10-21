# Mac SAO (Simple Add-on)


## Background
The firmware for this project was built on top of E. Brombaugh's CH32V003Fun OLED Demo.
The interface for the display has moved over to ADBeta's software i2c library.


# Making/Building
Fundamentally, it's the same as other ch32v003 fun repos:
```
$: make
```
should build and upload the result for you. As of this writing, there are a non-trivial number of warnings. This is all related to what I can only assume is my poor grasp of the C langauge. So far they haven't caused me much grief. 

To my knowledge, you need a WCH LinkE to upload to the CH32V003. I haven't quite yet found a way around it. 


# Random notes
The original OLED demo didn't have built-in support for 64x48, so its been hacked in. I don't know how everything will translate if you wanted to move to a different display. 
The software i2c library uses an 8-bit address with the read bit `and`'d in. As a result, the common address supplied `0x3c` doesn't work. You need to use `0x78`.

