CSci4061 Fall 2018 Project 3
============================
Name: Jared Erlien, Chase Rogness, Brian Cooper

X500: erlie003, rogne066, coope824

Group Number: 17
___
#### Individual Contributions
- All: Worked on main function error checking and structure. Worked on overall program error checking and memory management. Worked on request handling.
- Jared: Worked on cache functions, cache memory management and cache requests and decided on FIFO policy.
- Chase: Worked on worker and dispatcher thread functions. Worked on dynamic flag and mutex lock code.
- Brian: Worked on reading from disk and obtaining file information (such as content type). Worked on stat struct.
___
#### Project Information
- To compile the program, run the `make` command from a command line interface in the directory containing the source files (the Makefile is provided). This will result in an executable called `web_server`.
To run the program, run the executable in the current directory with `.\web_server port path num_dispatchers num_workers dynamic_flag queue_length cache_size` (change the directory as needed). The final parameter is only needed if you would like to use caching (explained below). The parameters are:

  - `port` (integer): the port to run the server on (range of 1025-65535, inclusive)
  - `path` (string): path to web root location
  - `num_dispatchers` (integer): number of dispatcher threads to initialize
  - `num_workers` (integer): number of worker threads to initialize
  - `dynamic_flag` (integer): indicates static or dynamic worker thread pool size (default 0)
  - `queue_length` (integer): size of request queue
  - `cache_size` (integer): number of entries available in cache


- The program works by initializing a local web server with specified parameters (listed above). The web server is hosted from the command line, and clients can be initialized to request data from the server. A multi-threaded environment is utilized by way of worker and dispatcher threads. The dispatcher threads regularly accept a request from an incoming connection, read the request, and place the request in a queue. The worker threads regularly monitor the request queue for changes, and receive any requests contained within (and then serve them to a client). On each client, times (in milliseconds) associated with each request are printed to the console.
By using a bounded buffer to store given requests in a fifo queue. The queue has a lock it shares across the two of them to allow us the ability to make sure one does not extract from another. The buffer also has two CV's that alert one or another when they are allowed to continue putting in or take out of the buffer. This allows us to continuously get a request and take one out and service it. Whether this be through the cache or through the reading directly from the disk. Which inside of the worker it is also locked to allow us not to accidentally be overwriting other threads data.
- For efficiency, this program supports caching. When a worker thread receives a request, it first searches a cache to see if that request has occured before. If it has, "HIT" is printed in the client console. Otherwise, "MISS" is printed. We implemented a FIFO policy in order to take care of overflow.
- For worker thread pool size we agreed upon a static number for working on our project.
