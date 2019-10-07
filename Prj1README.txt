
MyCompress - Gets two files from the command line to compress a file containing 1's and 0's and sends to a new file
MyDecompress - Gets two files from the command line and undos the compression from MyCompress
ForkCompress - Gets two file from the command line and uses a fork to create a child that uses execl to execute the file MyCompress on the two files
PipeCompress - create a pipe between two childs and sends a file from one to the other to compress it.
ParFork x Did not do
MinShell - a simple shell program that can only execute commands without arguments
MoreShell - A shell program that allows the user to execute commands with arguments
DupShell - A shell program that allows the user to execute commands with arguments and send them through a pipe to be used by another command
ParThread - Separates a file into 10 chunks and sends each chunk to one thread to be compressed and written to one single file, however only the last thread is being applied.

10. Since number 5 was not done, we cant check the comparison.
