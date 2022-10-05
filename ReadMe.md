## Haiku Advanced Volume Control

`havoc` is a small command line tool to control the system volume of the Haiku operating system.  It is intended to be used with the standard `Shortcuts` application so that keyboard shortcuts can be set to toggle the mute and adjust the volume incrementally.

------------------------------------------------------------

Output from running `havoc` with no arguments:
```
Volume: 0 (min=-60, max=18, step=0.1)
State: Not Muted
```

The list of options in the `havoc --help` output:
```
Usage: havoc [OPTION...]
  -a, --adjust=[1,-2.5,-4,9.5,...]      Increase/decrease volume by X dB
  -v, --volume=[-60,-20.5,0,18,...]     Set absolute volume dB level
  -t, --toggle                          Toggle mute on/off
  -m, --mute                            Set mute on
  -u, --unmute                          Set mute off
  -n, --notify[=1,1.5,3,...]            Show system notification and specify
                                        optional timeout (default: 1.5 seconds)

Help options:
  -?, --help                            Show this help message
      --usage                           Display brief usage message
```

### Command Examples

```
$ havoc -t -n
```
Toggle mute on or off and show a notification

```
$ havoc -a 1.5 -n 3
```
Increase volume by 1.5 dB and show a notification for 3 seconds

```
$ havoc -a -5 -n
```
Decrease volume by 5 dB and show a notification

```
$ havoc -n 10
```
Show a notification with the current status for 10 seconds

### Build Instructions

```
$ cd havoc
$ cmake .
$ make
```
