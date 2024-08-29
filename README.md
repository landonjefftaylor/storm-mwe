# storm-mwe
Minimal working example to reproduce the "munmap_chunk(): invalid pointer" error in Storm

## Commands to Reproduce Error from this Directory:

1. `mkdir build; cd build`
2. `cmake ..`
3. `make`
4. `cd ..`
5. `./build/errormwe models/ModifiedYeastPolarization.sm models/ModifiedYeastPolarization.csl`

## The Errors:

On our machines, this produces the following error:

`munmap_chunk(): invalid pointer`

It appears that this error occurs during the deconstruction of the BitVectorHashMap, since this error only occurs when we explore a significant number of states (over about 1000), and the gdb logs produce the following:

GDB command: `gdb --args ./build/errormwe models/ModifiedYeastPolarization.sm models/ModifiedYeastPolarization.csl`

```
(gdb) r
Starting program: <files>/errormwe models/ModifiedYeastPolarization.sm models/ModifiedYeastPolarization.csl
[Thread debugging using libthread_db enabled]
Using host libthread_db library "/lib/x86_64-linux-gnu/libthread_db.so.1".
 WARN (Program.cpp:238): The input model is a CTMC, but uses probabilistic commands like they are used in PRISM. Consider rewriting the commands to use Markovian commands instead.
 WARN (Program.cpp:1586): The model uses synchronizing Markovian commands. This may lead to unexpected verification results, because of unclear semantics.
 WARN (Program.cpp:1586): The model uses synchronizing Markovian commands. This may lead to unexpected verification results, because of unclear semantics.
 WARN (Program.cpp:1586): The model uses synchronizing Markovian commands. This may lead to unexpected verification results, because of unclear semantics.
munmap_chunk(): invalid pointer

Program received signal SIGABRT, Aborted.
0x00007ffff4aa9e2c in ?? () from /lib/x86_64-linux-gnu/libc.so.6

(gdb) backtrace
#0  0x00007ffff4aa9e2c in ?? () from /lib/x86_64-linux-gnu/libc.so.6
#1  0x00007ffff4a5afb2 in raise () from /lib/x86_64-linux-gnu/libc.so.6
#2  0x00007ffff4a45472 in abort () from /lib/x86_64-linux-gnu/libc.so.6
#3  0x00007ffff4a9e430 in ?? () from /lib/x86_64-linux-gnu/libc.so.6
#4  0x00007ffff4ab37aa in ?? () from /lib/x86_64-linux-gnu/libc.so.6
#5  0x00007ffff4ab396c in ?? () from /lib/x86_64-linux-gnu/libc.so.6
#6  0x00007ffff4ab7ed8 in free () from /lib/x86_64-linux-gnu/libc.so.6
#7  0x0000555555580897 in std::__new_allocator<unsigned int>::deallocate (this=0x7fffffffce88, __p=0x555555792b50, __n=2048) at /usr/include/c++/12/bits/new_allocator.h:158
#8  0x000055555557f1cd in std::allocator_traits<std::allocator<unsigned int> >::deallocate (__a=..., __p=0x555555792b50, __n=2048) at /usr/include/c++/12/bits/alloc_traits.h:496
#9  0x000055555557d7b0 in std::_Vector_base<unsigned int, std::allocator<unsigned int> >::_M_deallocate (this=0x7fffffffce88, __p=0x555555792b50, __n=2048) at /usr/include/c++/12/bits/stl_vector.h:387
#10 0x000055555557b51e in std::_Vector_base<unsigned int, std::allocator<unsigned int> >::~_Vector_base (this=0x7fffffffce88, __in_chrg=<optimized out>) at /usr/include/c++/12/bits/stl_vector.h:366
#11 0x000055555557b56f in std::vector<unsigned int, std::allocator<unsigned int> >::~vector (this=0x7fffffffce88, __in_chrg=<optimized out>) at /usr/include/c++/12/bits/stl_vector.h:733
#12 0x000055555557ac68 in storm::storage::BitVectorHashMap<unsigned int, storm::storage::Murmur3BitVectorHash<unsigned int> >::~BitVectorHashMap (this=0x7fffffffce50, __in_chrg=<optimized out>)
    at <files>/storm/build/include/storm/storage/BitVectorHashMap.h:20
#13 0x0000555555572c77 in main (argc=3, argv=0x7fffffffdb68) at <files>/main.cpp:98
```

