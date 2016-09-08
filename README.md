# arduino-bunny-nightlight
An Arduino nightlight that looks like a bunny.

# Final Product
[<img src="./readme_files/yt_thumbnail.png">](https://youtu.be/e8dI3R4e3lQ)

# Inspiration
I was inspired by [this blog post] on Sparkfun. However, I wanted to do something more complex in terms of lightning and did not have access to acrylic. So instead, the bunny shape was laser cut from birch and a simple board was assembled to host two RGB LEDs, a button, a power regulator circuit, and a few extra components for a standalone ATmega168.

There are three modes for this nighlight:

1. solid color (five colors pre-loaded; those can be rotated through by a short button press)
The single button press controls the light mode:
2. pulsating color (same five colors; those can also be rotated through by a short button press)
3. crossfading light (uses the same five colors).


The long button press activates one of the sleep mode durations (turning the current light off after 15 / 30 / 60 minutes). A short button press after the sleep mode was activated will cancel it.

# What's Included
1. Eagle schematics for the board<sup name="a1">[1](#f1)</sup>
2. Arduino sketch
 
# What's Not Included
1. The bunny shape vector was not included; you can purchase one from a variery of sites, and it does not even have to be a bunny!

License
----

MIT

Footnotes
----
<b name="f1">1</b> Funny story. I can only make a few itirations of the board and it was my first time manufacturing of PCB. Being a lengthy and expensive process, I never waited for the last iteration of the board to arrive and instead made the LED subcircuit using a piece of prototyping board. Long story short - the circuit included is about right, being very simple, but I am not sure if the PCB actually works since it was untested. [↩](#a1)
[this blog post]:https://learn.sparkfun.com/tutorials/elasto-nightlight
