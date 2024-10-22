all: deps get_proc_tics

deps: 
	curl -OL https://raw.githubusercontent.com/nothings/stb/refs/heads/master/stb_ds.h

get_proc_tics: deps
	gcc -Os -static -o get_proc_ticks get_proc_ticks.c

clean:
	rm get_proc_ticks
