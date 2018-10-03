/* CSci4061 F2018 Assignment 1
* login: cselabs_login_name                           (update)
* date : mm/dd/yy                                     (update)
* name: Brian Cooper, Chase Rogness, Jared Erlien
* id:     coope824,     rogne066,      erlie003   */

// This is the main file for the code
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "util.h"

/*-------------------------------------------------------HELPER FUNCTIONS PROTOTYPES---------------------------------*/
void show_error_message(char * ExecName);
//Write your functions prototypes here
void show_targets(target_t targets[], int nTargetCount);
void build(char TargetName[], target_t targets[], int nTargetCount);
/*-------------------------------------------------------END OF HELPER FUNCTIONS PROTOTYPES--------------------------*/


/*-------------------------------------------------------HELPER FUNCTIONS--------------------------------------------*/

//This is the function for writing an error to the stream
//It prints the same message in all the cases
void show_error_message(char * ExecName)
{
	fprintf(stderr, "Usage: %s [options] [target] : only single target is allowed.\n", ExecName);
	fprintf(stderr, "-f FILE\t\tRead FILE as a makefile.\n");
	fprintf(stderr, "-h\t\tPrint this message and exit.\n");
	exit(0);
}

//Write your functions here

//Phase1: Warmup phase for parsing the structure here. Do it as per the PDF (Writeup)
void show_targets(target_t targets[], int nTargetCount)
{
	//Write your warmup code here
	for(int i = 0; i < nTargetCount; i++) {
		int dependencyCount = targets[i].DependencyCount;
		printf("Target Name: %s\n", targets[i].TargetName);
		printf("DependencyCount: %d\n", dependencyCount);
		if(dependencyCount > 0) {
			printf("DependencyNames: ");
			for(int j = 0; j < dependencyCount; j++) {
				printf("%s ", targets[i].DependencyNames[j]);
			}
			printf("\n");
		}
		printf("Command: %s\n", targets[i].Command);
		printf("\n");
	}
}

// Takes in the targets array with a target_index to get us the target we will be searching for.
// Status for a target:
// 0 - Not Ready, hasn't been checked. (This is the default.)
// 1 - Ready to be executed. (Change to this once all checker has been completed.)
// 2 - Finished. (Change to this once fork/exec is finished.)
// 3 - Does not need to be built at all, no fork/exec needed.
void build(char TargetName[], target_t targets[], int nTargetCount) {
	// Initial build variable declarations.
	int target_index = find_target(TargetName, targets, nTargetCount);
	if(target_index == -1) {
		// This does not have a target in the makefile, thus just return.
		return;
	}
	target_t target = targets[target_index];
	char *tokens[128];
	int nTokens = parse_into_tokens(target.Command, tokens, " ");
	int dependencyCount = target.DependencyCount;

	// Target has not been checked off
	while(target.Status == 0) {
		// File does not exist yet.
		if(does_file_exist(target.TargetName) == -1) {
			// Since file does not exist no more error checking.
			// Exit while loop.
			target.Status = 1;
		}
		// File does exist and must be checked for modification time.
		else {
			printf("File does exist and is needed to be checked.\n");
			// Do modification time checking later.
			target.Status = 1;
		}
	}
	// Target has been checked and needs to be built.
	while(target.Status == 1) {
		// Target must not be checked for dependencies.
		printf("File has been checked and needs to be built.\n");
		// Calls build on the dependencies.
		for(int i = 0; i < dependencyCount; i++) {
			// Will wait for the dependencies to return before continuing the for loop.
			build(target.DependencyNames[i], targets, nTargetCount);
			printf("Dependency Name: %s \n", target.DependencyNames[i]);
		}
		// For loop has finished and has traversed completely down.
		target.Status = 2;
	}
	// Target has been built.
	printf("Target has been built.\n");
}

/*-------------------------------------------------------END OF HELPER FUNCTIONS-------------------------------------*/


/*-------------------------------------------------------MAIN PROGRAM------------------------------------------------*/
//Main commencement
int main(int argc, char *argv[])
{
  target_t targets[MAX_NODES];
  int nTargetCount = 0;

  /* Variables you'll want to use */
  char Makefile[64] = "Makefile";
  char TargetName[64];

  /* Declarations for getopt. For better understanding of the function use the man command i.e. "man getopt" */
  extern int optind;   		// It is the index of the next element of the argv[] that is going to be processed
  extern char * optarg;		// It points to the option argument
  int ch;
  char *format = "f:h";
  char *temp;

  //Getopt function is used to access the command line arguments. However there can be arguments which may or may not need the parameters after the command
  //Example -f <filename> needs a finename, and therefore we need to give a colon after that sort of argument
  //Ex. f: for h there won't be any argument hence we are not going to do the same for h, hence "f:h"
  while((ch = getopt(argc, argv, format)) != -1)
  {
	  switch(ch)
	  {
	  	  case 'f':
	  		  temp = strdup(optarg);
	  		  strcpy(Makefile, temp);  // here the strdup returns a string and that is later copied using the strcpy
	  		  free(temp);	//need to manually free the pointer
	  		  break;

	  	  case 'h':
	  	  default:
	  		  show_error_message(argv[0]);
	  		  exit(1);
	  }

  }

  argc -= optind;
  if(argc > 1)   //Means that we are giving more than 1 target which is not accepted
  {
	  show_error_message(argv[0]);
	  return -1;   //This line is not needed
  }

  /* Init Targets */
  memset(targets, 0, sizeof(targets));   //initialize all the nodes first, just to avoid the valgrind checks

  /* Parse graph file or die, This is the main function to perform the toplogical sort and hence populate the structure */
  if((nTargetCount = parse(Makefile, targets)) == -1)  //here the parser returns the starting address of the array of the structure. Here we gave the makefile and then it just does the parsing of the makefile and then it has created array of the nodes
	  return -1;


  //Phase1: Warmup-----------------------------------------------------------------------------------------------------
  //Parse the structure elements and print them as mentioned in the Project Writeup
  /* Comment out the following line before Phase2 */
  // show_targets(targets, nTargetCount);
  //End of Warmup------------------------------------------------------------------------------------------------------

  /*
   * Set Targetname
   * If target is not set, set it to default (first target from makefile)
   */
  if(argc == 1)
		strcpy(TargetName, argv[optind]);    // here we have the given target, this acts as a method to begin the building
  else
  	strcpy(TargetName, targets[0].TargetName);  // default part is the first target

  /*
   * Now, the file has been parsed and the targets have been named.
   * You'll now want to check all dependencies (whether they are
   * available targets or files) and then execute the target that
   * was specified on the command line, along with their dependencies,
   * etc. Else if no target is mentioned then build the first target
   * found in Makefile.
   */

  //Phase2: Begins ----------------------------------------------------------------------------------------------------
  /*Your code begins here*/
	int target_index = find_target(TargetName, targets, nTargetCount);
	if(target_index == 0) {
		build(TargetName, targets, nTargetCount);
	} else {
		printf("Invalid make target, %s\n", TargetName);
	}
  /*End of your code*/
  //End of Phase2------------------------------------------------------------------------------------------------------

  return 0;
}
/*-------------------------------------------------------END OF MAIN PROGRAM------------------------------------------*/
