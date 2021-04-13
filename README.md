## Media Key Helper

Media Key Helper is a small command line tool to control the system volume on Haiku OS.  It is a more powerful version of the `setvolume` command included with Haiku and was written so that I could set keyboard shortcuts to toggle the mute and adjust the volume incrementally.

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

Help options:
  -?, --help                            Show this help message
      --usage                           Display brief usage message
```

### Build Instructions

```
cmake .
make
```
