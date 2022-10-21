By default max CPU memory accesses to be traced is 1-billion, it can be changed by preprocessor: Max_Accesses


Running it with a binary will produce a main memory access trace by simulating caches using FIFO and using an approcimate timing model to find out the cycle at which it will sent out of the LLC.

The trace generated is inside output directory and node(no) directory. This traced is to be parsed using 'parse' executable to generate final trace. 

You can also create a text trace using 'parse' executable and analyse the trace, but the final useable trace is inside the above mentioned directory.

(If you want to create the trace again, please rename existing Node(no) directory)

Trace has multiple rows for processid, thread-id, CPU generated virtual address (going to miss caches and access main memory), rd/wr/fetch and the approximate clock cycle at which it will be LLC miss

It only instrument the multi-threaded part of the workload and skip initially setup part where only one core is working. (This can be changed to instrument all of the workload by commenting code at 665 line)


Tool was initially built to use with multiple workloads, where another pintool will run other workload and also send its CPU memory accessesfor cache simulation
