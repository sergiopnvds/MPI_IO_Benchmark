=== CS546 PARALLEL AND DISTRIBUTED PORCESSING HW6  ===
Author: Sergio Penavades Suarez
CWID: A20387929
Date: 11/09/2016
Release: 1.0.1


	-codePart1: includes the code of the part1 

	-codePart2: includes the code of the part2, the benchmark 

		
= INSTRUCTIONS FOR PART 1 =

1 - Download and uncompress "CS546_01_PenavadesSuarez_Sergio_HW6.zip".

2 - Open a terminal window and go to the uncompressed folder described in the step before.

3 - Compile the file using: mpicc codePart1.c -o part1 

4 - Execute the code using: mpiexec -np <num_threads> ./part1 <fileName>
	
	- example: mpiexec -np 4 ./part1 output1.dat

5 - You can view the results using: od -td -v <fileName>

	- example: od -td -v output1.dat


= INSTRUCTIONS FOR PART 2 =

1 - Download and uncompress "CS546_01_PenavadesSuarez_Sergio_HW6.zip".

2 - Open a terminal window and go to the uncompressed folder described in the step before.

3 - Compile the file using: mpicc codePart2.c -o part2 

4 - Execute the code using: mpiexec -np <num_threads> ./part2 <temporal_fileName> <num_MB> <fileName>
	
	- example: mpiexec -np 4 ./part2 file.dat 16 output2.dat

5 - You can view the results using: od -td -v <fileName>

	- example: od -td -v output2.dat