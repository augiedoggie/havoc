## Media Key Helper

Media Key Helper is a small command line tool to control the system volume of the Haiku operating system.  It is a more powerful version of the `setvolume` command included with Haiku and was written so that I could set keyboard shortcuts to toggle the mute and adjust the volume incrementally.

------------------------------------------------------------

Output from running `mkh` with no arguments:
```
Volume: -0.0 (min=-60.0, max=18.0, step=0.1)
State: Not Muted
```

The list of options in the `mkh --help` output:
```
Usage: mkh [OPTION...]
  -a, --adjust=[1,-2.5,-4,9.5,...]      Increase/decrease volume by X
  -v, --volume=[-60,-20.5,0,18,...]     Set absolute volume level
  -t, --toggle                          Toggle mute on/off
  -m, --mute                            Set mute on
  -u, --unmute                          Set mute off
  -n, --notify[=0,1,1.5,3,...]          Show system notification and specify
                                        optional timeout (default: 1.5 seconds)

Help options:
  -?, --help                            Show this help message
      --usage                           Display brief usage message
```

### Command Examples

```
$ mkh -t -n
```
Toggle mute on or off and show a notification

```
$ mkh -a 1.5 -n 3
```
Increase volume by 1.5 dB and show a notification for 3 seconds

```
$ mkh -a -5 -n
```
Decrease volume by 5 dB and show a notification

```
$ mkh -n 10
```
Show a notification with the current status for 10 seconds

### Build Instructions

```
cmake .
make
```
