share
====

<b>The Share Library Project.</b>

The share library introduces a new model of handling IPC and Internet communications. Supplies documented API with POSIX and convienence calls for networking applications.

<h2>Quick Instructions</h2>

<h3>Share Library Build Instructions</h3>

On linux, a build can be performed by running the following:
  git clone https://github.com/neonatura/share
  cd share
  mkdir build
  cd build
  ../configure
  make
  make install


<h3>Stratum + USDe Coin Service</h3>
A stratum server for the USDe virtual currency is provided in this library. The server is embedded into the usde coin server in the program "shcoind". The "shcoin" program is provided to perform RPC commands against the coin server.

Note: No additional programs from the share library suite is required in order to run the coin+stratum service. The C share library is staticly linked against the coin service, and a 'make install' is not required to run the program.

The stratum service utilizes new stratum methods that are not standard, and require a compatible web-based front end. See the 'sharenet-php' project for more information.


<h3>Stratum Protocol Specifications</h3>
"mining.shares"
"mining.get_transactions"
"mining.info"
"mining.authorize"
"mining.submit"
"mining.subscribe"
"mining.ping"
"account.info"
"block.info"

<h3>Build Dependencies</h3>

The c++ boost shared library is required.  To be specific, the "system", "filesystem", "program_options", and "thread" boost libraries. The "shcoind" and "shcoin" programs are the only sharelib program that link against boost libraries.
To install on linux run 'yum install libboost*' or 'apt-get install libboost*'.

The 'openssl version 1.0.1g' distribution has been included in the directory '/depend/openssl-1.0.1g'. This version will automatically be compiled and linked against the shcoind and shcoin programs.

You must install the 'gcc-java' package in order to provide the "jni.h" gcc header file for the java API to compile correctly.
To install on linux run 'yum install gcc-java' or 'apt-get install gcc-java'.

SWIG is optional. Pre-built source code has been generated as part of the distritution. 
To install on linux run 'yum install swig' or 'apt-get install swig'.


<h2>Library Specifications</h2> 

<h3>Internet </h3>

Network communications can be performed in a traditional manner in order to replace the TCP protocol with the share library's built-in Encoded Stream Protocol.

The Encoded Stream Protocol introduces several enhancement over the original 1981 TCP model. TCP was designed for a much different virtual-verse than what is available in the 21st century.

The Encoded Stream Protocol provides extra security, built-in stream compression, backwards compatibility, packet route probing, optimized route detection, and increases the maximum limitations on the amount of data that can be sent in a single interval of time.

Large stream buffer support (over 64k per r/w) which minimizes critical program termination due to socket errors.

The Share Library also implements maximum operating system thresholds and the ability to accept more than 1024 sockets by a single process, 

<h3>Inter-Process Communication (IPC)</h3>

The share library supports POSIX style share memory mapping, message queues, and memory mapped files. The mapped files may preside on a local file system or a shared file.

The share library provides simulated block devices in order to stream file data between multiple endpoints. Permissions allow control of whether the stream is readable, writeable, or executable. 

- Shared Filesystem

The share library introduces a flexible approach to inter-process communication by combining a shared memory map with a versioned file system.

The sharefs file system utilitizes the operating system's file cache in order to simulate a shared memory segment. 

File data is saved persistently with access rights based on the peer reading or writing the file. Several version of the same file may exist, and based on the trust level with another sharefs peer multiple copies of the same file may exist.

The structure of the file system is designed in order to allow for multiple hierarchies simutaneously. The file system contains "meta file" definitions which allow for a program dynamically interpret how to handle file data. 

<h3>Share Daemons</h3>

The share library provides the ability to easily establish several standard daemons such as web or email server. The daemon uses a combination of specific content, meta file definitions, and shared file data.

<h3>Virtual Currency</h3>
An implementation of the USDe virtual currency, with a built-in stratum server, is provided via the "shcoind" and "shcoin" programs.

The "shcoind" runs in a similar fashion to the existing "usded" program available at "https://github.com/usde-project/USDE". The "shcoin" program is used in order to provide direct RPC communication with the coin server. The stratum server listens on port 9448.


<h3>Portability</h3>

Support share library API calls are accessible from php, java, and android platforms. 


<h2>Revision History</h2>

<u>June 30 2014release of v2.1.4</u>
Local shfs partition support.
The usded daemon with embedded stratum capability.

<u>April 10 2013release of v2.1 (releases/libshare_2.1.tar.xz)</u>
Current functionality provides preliminary support for sharefs filesystem, socket wrappers, hashmaps, memory buffers/pools, tiny encryption, and locks. See the 'doc/html' directory for a API reference manual. Includes gnulib support for base fs/socket operations, java bindings, and self-test routines.  

<h3>Future Release Versions</h3>

<u>share library version 2.3.0</u>
Distributed file-system support.
