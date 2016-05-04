# userlandexec
## userland exec for Linux x86_64

This code emulates an `exec()` system call. That is, it reads and ELF format file,
and loads it into memory at the right place, and then starts the newly-loaded
executable to running.

All this is usually done by the Linux kernel, so some bizarre things go on.
For starters, the userland exec unmaps the currently-executing ELF file,
so as to be able to put the new ELF file's contents in the right place
in memory.

### Building

Run `make` - that should compile `example` and `ulexec.so`. Once you've
done that, you can try it out: 

`./example  ./ulexec.so  /usr/bin/cat /proc/self/maps`

### Fun

`./example  ./ulexec.so  `./example  ./ulexec.so  `./example  ./ulexec.so  /usr/bin/cat /proc/self/maps`

Yes! You can have it overlay itself with another copy of itself.
