Isbat-ul Islam 261117219

Neil Joe George 261107755

For assignment 2, we built upon out assignment 1 submission. We did not use the starter code for assignment 1.


High-Level Design Choices:

For code (script) loading, we used the shell memory and used an key-value system to find the instructions. Each generated PCB has a unique ID and is associated with one file's commands that were loaded into the shell memory. These instructions are found using the number of lines that has been stored, and the PID of the PCB. The Ready Queue is a Linked-List that had a head pointing to a PCB, and each PCB has a pointer to a "next" PCB. Thus to traverse the list, we need to go through head. Enqueue and dequeue are designed as a FIFO system.

We sort using selection sort since we can find the minimum value in each queue and can place it to the head since we know it's the smallest element. We can continue doing so till we sort the queue completely. We compare with job_length_score since at the beginning it's the same as number_of_lines and since we can reuse the same sorting algorithm everywhere we can sort with jobScore instead.