# pthread-cpu-affinity-

These test programs were written to prove that CPU affinity inheritence from pthread_create 
designates which logical CPU a thread may prefer.

Each file can be compiled with:

    # gcc -o try try.c -pthread
    
And run with: 

    # taskset -a -c 0,1,2 ./try
    
where 0,1,2 can be replaced with your local CPU numbers of choice.






```
I've done some testing internally and I see in our test program that using pthread_setaffinity_np(3, ...) causes my subsequent threads to use 
CPU 3 as well, ignoring the CPU set provided via `taskset`. 

I looked at the `taskset` source in util-linux-2.23.2 and it looks like it uses sched_setaffinity() to adjust the CPU affinity of the process.
Reading [1], I believe that pthread_setaffinity_np() adjusts the CPU affinity thus causing the CPU affinity of the subsequently created threads
to be what was set by pthread_setaffinity_np() and not by taskset.   

        [1] https://man7.org/linux/man-pages/man3/pthread_setaffinity_np.3.html

As an example, I set the CPU affinity to CPU 3 with pthread_setaffinity_np() very early in the main() of `try1.c`, and the taskset CPU set is ignored
as the threads inherit the CPU affinity from `try1`.

	[root@rhel7vm affinity]# taskset -c 0,2,3 ./try1

	[root@rhel7vm affinity]# ps -aF
	UID        PID  PPID  C    SZ   RSS PSR STIME TTY          TIME CMD
	root      1356  1307  0 11873   392   3 16:21 pts/0    00:00:00 ./try1
	root      1362  1356 18 27284  1408   3 16:21 pts/0    00:00:01 dd if=/dev/urandom of=/dev/null bs=1M
	root      1363  1356 18 27284  1412   3 16:21 pts/0    00:00:01 dd if=/dev/urandom of=/dev/null bs=1M
	root      1364  1356 18 27284  1412   3 16:21 pts/0    00:00:01 dd if=/dev/urandom of=/dev/null bs=1M
	root      1365  1356 18 27284  1412   3 16:21 pts/0    00:00:01 dd if=/dev/urandom of=/dev/null bs=1M
	root      1366  1356 18 27284  1412   3 16:21 pts/0    00:00:01 dd if=/dev/urandom of=/dev/null bs=1M
	root      1367  1326  0 38863  1848   0 16:21 pts/1    00:00:00 ps -aF


In `try2.c` I first create a few threads with pthread_create(), and then later set the CPU affinity to CPU 3 with pthread_setaffinity_np().
Its visible that the threads are using the CPU set from taskset, and `try2` had it's CPU affinity changed to CPU 3 with pthread_setaffinity_np().
Since the threads were created first, they do not inherit the new CPU affinity specified by pthread_setaffinity_np().

	[root@rhel7vm affinity]# taskset -c 0,1,2 ./try2

	[root@rhel7vm affinity]# ps -aF
	UID        PID  PPID  C    SZ   RSS PSR STIME TTY          TIME CMD
	root      1377  1307  0 11873   628   3 16:25 pts/0    00:00:00 ./try2
	root      1381  1377 60 27284  1408   2 16:25 pts/0    00:07:06 dd if=/dev/urandom of=/dev/null bs=1M
	root      1382  1377 59 27284  1412   2 16:25 pts/0    00:07:02 dd if=/dev/urandom of=/dev/null bs=1M
	root      1384  1377 58 27284  1412   0 16:25 pts/0    00:06:55 dd if=/dev/urandom of=/dev/null bs=1M
	root      1386  1377 60 27284  1412   1 16:25 pts/0    00:07:08 dd if=/dev/urandom of=/dev/null bs=1M
	root      1387  1377 60 27284  1412   0 16:25 pts/0    00:07:04 dd if=/dev/urandom of=/dev/null bs=1M
	root      1398  1326  0 38863  1852   3 16:37 pts/1    00:00:00 ps -aF


Based on [1], it's my understanding that this is expected behaviour.
```
