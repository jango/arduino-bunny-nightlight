# arduino-bunny-nightlight
An Arduino nightlight that looks like a bunny.

# Final Product
Not finished yet, but will be soon.

# Inspiration
I was inspired by [this blog post] on Sparkfun. However, I wanted to do something more complex in terms of lightning and did not have access to acrylic. So instead, the bunny shape was laser cut from birch and a simple board was assembled to host two RGB LEDs, a button, a power regulator circuit, and a few extra components for a standalone ATmega168.

There are three modes for this nighlight:

1. solid color (five colors pre-loaded; those can be rotated through by a short button press)
The single button press controls the light mode:
2. pulsating color (same five colors; those can also be rotated through by a short button press)
3. crossfading light (uses the same five colors).


The long button press activates one of the sleep mode durations (turning the current light off after 15 / 30 / 60 minutes). A short button press after the sleep mode was activated will cancel it.

# What's Included
1. Eagle schematics for the board
2. Arduino sketch
 
# What's Not Included
1. The bunny shape vector was not included; you can purchase one from a variery of sites, and it does not even have to be a bunny!

License
----

MIT

[this blog post]:https://learn.sparkfun.com/tutorials/elasto-nightlight
