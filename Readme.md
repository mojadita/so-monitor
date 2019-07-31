# Mon.

A program to monitor several tty lines for data and output
timestamped and formatted traces on how and where the data comes
from.

`Makefile` is generated for berkeley make and it's not fully
compatible with GNU make.  I'll make a portable `Makefile` in the
near future, just when I have some time to do it.

The program uses `select(2)` to decide which line has data
available and puts each device in raw mode to read one character
at a time, so the individual characters are available for program
processing as soon as possible.

Using select makes it a best approach to read ASAP, while not
consuming innecessary CPU time.

To end, nothing but saying that the program answers the question
made in [StackOverflow](https://stackoverflow.com/questions/57234078/how-to-read-multiple-serial-ports-in-realtime-in-c-or-python/57251384#57251384)
on how to read simultaneously several serial ports with maximum
time granularity, as to be able to see the temporal sequence of
data flow.
