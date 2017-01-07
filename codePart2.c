#include <stdio.h>   /* all IO stuff is contained here */
#include <mpi.h>     /* MPI and MPI-IO are contained here */
#include <string.h>  /* string functions are contained here */
#include <stdlib.h>  /* atoi is contained here */
#include <time.h>    /* time functions are contained here */

static void handle_error(int errcode, char *str)
{
char msg[MPI_MAX_ERROR_STRING];
int resultlen;
MPI_Error_string(errcode, msg, &resultlen);
fprintf(stderr, "%s: %s\n", str, msg);
/* Aborting on error might be too aggressive.  If
 * you're sure you can
 * continue after an error, comment or remove
 * the following line */
MPI_Abort(MPI_COMM_WORLD, 1);
}

#define MPI_CHECK(fn) { int errcode; errcode = (fn); \
    if (errcode != MPI_SUCCESS) handle_error(errcode, #fn ); }

int main(int argc, char *argv[])
{

int i, rank, size, nints, master, mbRank, filenameLength, fileLength, iterations, inputError =0, correctness=1, finalCorrectness=1;
char *fileName = NULL;
double initTime, endTime, writeTime, readTime;
char buffer[1024];
MPI_File file;
MPI_Status status;
MPI_Offset offset, fileSize;

MPI_Init(&argc, &argv);
initTime = MPI_Wtime();
MPI_Comm_rank(MPI_COMM_WORLD, &rank);
MPI_Comm_size(MPI_COMM_WORLD, &size);

//linking comand params
if(argc <= 1 || argc > 4){
  inputError = 1;
}
if(argc > 2){
  mbRank = atoi(argv[2]);
}
if(argc > 3){
  fileName = argv[3];
  fileLength = strlen(fileName)+1;
}

//Sharing input error variable with all processes
MPI_Bcast(&inputError, 1, MPI_INT, 0, MPI_COMM_WORLD);

//if input no error go inside
if(inputError == 0){
  
  //Sharing number of MB per process
  MPI_Bcast(&mbRank, 1, MPI_INT, 0, MPI_COMM_WORLD);
  initTime = MPI_Wtime();
  long bytesRank = 1000000 * mbRank;
  offset = (long long) rank * (long long) (bytesRank * sizeof(char));

  //Writing file starts
  MPI_CHECK(MPI_File_open(MPI_COMM_WORLD, "temp.dat", MPI_MODE_CREATE | MPI_MODE_RDWR, MPI_INFO_NULL, &file));
  MPI_CHECK(MPI_File_seek(file, offset, MPI_SEEK_SET));

  int bufferSize = 1024;
  char randomData[bufferSize];
  srand(time(NULL));

  int i;
  for(i = 0; i < bufferSize; i++)
    buffer[i] = rand() % bufferSize;

  iterations = bytesRank / bufferSize;

  for(i = 0; i < iterations; i++)
  MPI_CHECK(MPI_File_write(file, buffer, bufferSize, MPI_CHAR, &status));

  MPI_CHECK(MPI_File_close(&file));
  endTime = MPI_Wtime();
  writeTime = endTime - initTime;
  // Reading file starts
  initTime = MPI_Wtime();
  MPI_CHECK(MPI_File_open(MPI_COMM_WORLD, "temp.dat",
                MPI_MODE_RDONLY, MPI_INFO_NULL, &file));
  MPI_CHECK(MPI_File_seek(file, offset, MPI_SEEK_SET));

  for(i = 0; i < iterations; i++)
    MPI_CHECK(MPI_File_read(file, buffer, bufferSize, MPI_CHAR, &status));

  endTime = MPI_Wtime();
  readTime = endTime - initTime;
  MPI_File_close(&file);

  // Writing results file
  if(rank == 0){
    remove("temp.dat");//delete temporal file
    FILE *output;
    output = fopen(fileName, "w");
    fprintf(output, "write time: %f seconds\n", writeTime);
    fprintf(output, "read time: %f seconds\n", readTime);
    fprintf(output, "overall time: %f seconds\n", readTime+writeTime);
    fprintf(output, "maximum bandwidth: %f MB/s\n", ((double)mbRank)*size/writeTime);
    fclose(output);
  }


  MPI_Finalize();
}else{//if input error go inside
    printf("Input error.\n");
    endTime = MPI_Wtime();
    MPI_Finalize();
    printf("Elepsed time: %f seconds\n ", readTime+writeTime);
}
return 0;

}