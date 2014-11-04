Often times, when reverse engineering an RF protocol, you will be looking for
a particular pattern to signify the start of a packet. This can be done
manually by searching through a file with a one-off python script, but that's
tedious.

This package contains a single block, that looks for a pattern. When found, it
prints to the screen. It probably works close-to-real-time, but it works for
offline analysis too.

A bit silly to make an entire module for a single block, eh? Maybe I'll add
some extra features to it down the road.
