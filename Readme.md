### get\_proc\_ticks

This is just a small program, ideally statically linked (more ideally with musl libc) to get the percentage of CPU ticks used by all the processes in a linux machine during a certain amount of time. It can be used with Scaphandre (or some other way to get the CPU usage power) and can give an estimation of the power used for each process on a VM.

The output shows the VM id, the PID of the process and its command line

Options:

- `-s`: number of seconds that will be used to compute the amount of ticks (default value: 3 seconds)
- `-m`: VM ID for a virtual machine (defaults to 0)
- `-p`: total power (microWatts, uW) used by the VM

If no power is set up, a percentage of the amount of ticks will be shown instead.
An example of the output in that case would be

```
$ get_proc_ticks
0|71|Kernel proc [71]|0.61
0|98|Kernel proc [98]|0.61
0|559|/lib/systemd/systemd-oomd |1.22
0|688|/usr/sbin/thermald --systemd --dbus-enable --adaptive |1.22
0|690|/bin/warp-svc |0.61
0|800|/usr/bin/containerd |1.22
0|2660|/usr/lib/xorg/Xorg vt2 -displayfd 3 -auth /run/user/1000/gdm/Xauthority -nolisten tcp -background none -noreset -keeptty -novtswitch -verbose 3 |1.22
0|13287|/usr/bin/oned -f |0.61
0|13288|ruby /usr/lib/one/mads/one_hm.rb -p 2101 -l 2102 -b 127.0.0.1 |0.61
0|41524|tmux |0.61
0|2669819|/usr/libexec/gnome-terminal-server |1.22
........
```

If some power is set up, the command would show the following

```
$ get_proc_ticks -p 9000000 -m 66
66|96|Kernel proc [96]|50847
66|103|Kernel proc [103]|50847
66|499|Kernel proc [499]|50847
66|690|/bin/warp-svc |101694
66|782|/usr/sbin/libvirtd |152541
66|800|/usr/bin/containerd |101694
66|2660|/usr/lib/xorg/Xorg vt2 -displayfd 3 -auth /run/user/1000/gdm/Xauthority -nolisten tcp -background none -noreset -keeptty -novtswitch -verbose 3 |101694
66|2804|/usr/bin/ibus-daemon --daemonize --xim |50847
66|2854|i3status |101694
66|2905|/bin/warp-taskbar |50847
66|41524|tmux |50847
66|948376|Kernel proc [948376]|50847
66|997044|/usr/bin/vim /home/administrador/sources/one-ee/.git/COMMIT_EDITMSG |50847
66|1002587|./get_proc_ticks -p 9000000 -m 66 |50847
66|2020028|ruby /var/tmp/one/im/kvm.d/monitord-client.rb kvm 6 127.0.0.1 |50847
66|2669819|/usr/libexec/gnome-terminal-server |50847
...
```

### Dependencies

- [`stb_ds.h`](https://github.com/nothings/stb/blob/master/stb_ds.h): typesafe hash tables for C written by Sean Barrett. It is a single file downloaded from github when executing a `make deps`

### Compilation

```
make
```
