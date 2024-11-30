Isbat-ul Islam 261117219

Neil Joe George 261107755

For assignment 3, we built the paging infrastructure on our assignment 2 submission.

High-Level Design Choices:

Initially, our program only used one array for memory, which was split into two, one for frames and one for variables. We also opted to use the backing store which is initialized and cleaned during and after each execution. The page replacement policy of LRU was implemented using a doubly linked-list, where the least recently used node (the head) was the one to be evicted, whereas the tail has the most recently used nodes. The DLL data structure is called the DemandPagingQueue and it keeps track of DemandPagingTracker structures. The reason why a DLL was used is because it makes page handling O(1) operations, and is simple to implement despite it's slightly higher complexity, and frames can be easily found within, and replaced.  

Other data structures used are the pagingReturn structure, or the pagingFileTracker structure. These structures are used to make data handling easier by encapsulating necessary information. Since these data are passed around a lot, having structures to "cache" them, or pass around pointer make it easier. 

