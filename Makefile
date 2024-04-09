all:
	gcc -Os -static -o get_proc_ticks get_proc_ticks.c

clean:
	rm get_proc_ticks
