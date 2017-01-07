#include <stdio.h>   /* all IO stuff is contained here */
#include <mpi.h>     /* MPI and MPI-IO are contained here */

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

int i, rank, size, nints, inputError, master, correctness=1, finalCorrectness=1, iterations=10;
char *fileName = NULL;
double initTime, endTime;
MPI_File file;
MPI_Status status;
MPI_Offset offset, fileSize;


MPI_Init(&argc, &argv);
initTime = MPI_Wtime();
MPI_Comm_rank(MPI_COMM_WORLD, &rank);
MPI_Comm_size(MPI_COMM_WORLD, &size);

if(argc <= 1){
  inputError = 1;
  }else{
    fileName = argv[1];
    inputError = 0;
  }

MPI_Bcast(&inputError, 1, MPI_INT, 0, MPI_COMM_WORLD);

if(inputError == 0){
  //Write starts
  offset = rank*iterations*sizeof(int);
  MPI_CHECK(MPI_File_open(MPI_COMM_WORLD, fileName, MPI_MODE_CREATE|MPI_MODE_WRONLY, MPI_INFO_NULL, &file));
  MPI_CHECK(MPI_File_seek(file, offset, MPI_SEEK_SET));
  for(i=0; i<iterations; i++)
	MPI_CHECK(MPI_File_write(file, &rank, 1, MPI_INT, &status));
  MPI_File_close(&file);

  //Check the correctness starts
  MPI_CHECK(MPI_File_open(MPI_COMM_WORLD,fileName,MPI_MODE_RDONLY,MPI_INFO_NULL,&file));
  MPI_CHECK(MPI_File_get_size(file, &fileSize));
  int bufSize = fileSize / size;
  nints = bufSize/sizeof(int);
  int buf [nints];

  MPI_CHECK(MPI_File_seek(file, rank * bufSize, MPI_SEEK_SET));
  MPI_CHECK(MPI_File_read(file, buf, nints, MPI_INT, &status));
  for(i=0; i<nints; i++){
	  if (buf[i]!=rank){
      correctness = 0;
      break;
	  }
  }  
  MPI_Allreduce(&correctness, &finalCorrectness, 1, MPI_DOUBLE, MPI_MAX,
              MPI_COMM_WORLD);  
  MPI_File_close(&file);
  endTime = MPI_Wtime();
  MPI_Finalize();
  
  if (rank == 0){
    if (finalCorrectness == 1){
	    printf("Output file correctnes verified\n");
	    printf("Elepsed time: %f seconds\n", endTime-initTime);
    }else {
  	  printf ("Output file does not fulfill the correctnes\n");
      printf("Elepsed time: %f seconds\n ", endTime-initTime);
    }
  }
}else{
  if (rank == 0){
    printf("Input error.\n");
    endTime = MPI_Wtime();
    MPI_Finalize();
    printf("Elepsed time: %f seconds\n ", endTime-initTime);
  }  
}


return 0;

}