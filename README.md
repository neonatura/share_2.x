share
====

The Share Library Project.

The share library introduces a new model of handling IPC and Internet communications. Supplies documented API with POSIX and convienence calls for networking applications.

<strong>Note: This implementation is incomplete and non-functioning. A production level library will be avaialable in the indeterminate future.</strong>

- Internet

Network communications can be performed in a traditional manner in order to replace the TCP protocol with the share library's built-in Encoded Stream Protocol.

The Encoded Stream Protocol introduces several enhancement over the original 1981 TCP model. TCP was designed for a much different virtual-verse than what is available in the 21st century.

The Encoded Stream Protocol provides extra security, built-in stream compression, backwards compatibility, packet route probing, optimized route detection, and increases the maximum limitations on the amount of data that can be sent in a single interval of time.

Large stream buffer support (over 64k per r/w) which minimizes critical program termination due to socket errors.

The Share Library also implements maximum operating system thresholds and the ability to accept more than 1024 sockets by a single process, 

- Inter-Process Communication (IPC)

The share library supports POSIX style share memory mapping, message queues, and memory mapped files. The mapped files may preside on a local file system or a shared file.

The share library provides simulated block devices in order to stream file data between multiple endpoints. Permissions allow control of whether the stream is readable, writeable, or executable. 

- Shared Filesystem

The share library introduces a flexible approach to inter-process communication by combining a shared memory map with a versioned file system.

The sharefs file system utilitizes the operating system's file cache in order to simulate a shared memory segment. 

File data is saved persistently with access rights based on the peer reading or writing the file. Several version of the same file may exist, and based on the trust level with another sharefs peer multiple copies of the same file may exist.

The structure of the file system is designed in order to allow for multiple hierarchies simutaneously. The file system contains "meta file" definitions which allow for a program dynamically interpret how to handle file data. 

- Share Daemons

The share library provides the ability to easily establish several standard daemons such as web or email server. The daemon uses a combination of specific content, meta file definitions, and shared file data.


