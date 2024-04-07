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
proc_consumption_percent{vm_id="0",proc_id="1566",cmd="sway "} 20
proc_consumption_percent{vm_id="0",proc_id="1595",cmd="swaybar -b bar-0 "} 20
proc_consumption_percent{vm_id="0",proc_id="3596",cmd="/app/brave/brave "} 40
proc_consumption_percent{vm_id="0",proc_id="8892",cmd="./get_proc_tics "} 20
```

If some power is set up, the command would show the following

```
$ get_proc_ticks -p 9000000 -m 66
proc_power{vm_id="66",proc_id="18",cmd=""} 900000
proc_power{vm_id="66",proc_id="855",cmd=""} 900000
proc_power{vm_id="66",proc_id="1105",cmd="tuigreet --cmd /bin/sh"} 1800000
proc_power{vm_id="66",proc_id="3490",cmd="/app/brave/brave"} 900000
proc_power{vm_id="66",proc_id="3596",cmd="/app/brave/brave"} 1800000
proc_power{vm_id="66",proc_id="4112",cmd="/app/brave/brave"} 900000
proc_power{vm_id="66",proc_id="8893",cmd=""} 900000
proc_power{vm_id="66",proc_id="10964",cmd="./get_proc_tics -p 9000000 -m 66 "} 900000
```

### Compilation

Dependencies: 
- uthash ( https://troydhanson.github.io/uthash/ ) to create a hash of the running processes. There must be a development package for each major distribution, please check your packages.

After that, it can be compiled with a simple `make` command
