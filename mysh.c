#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_ARGS 128

void run_command(char** args);

int main(int argc, char** argv) {
  while(1) {
    char* line = NULL;    // Pointer that will hold the line we read in
    size_t line_length;   // Space for the length of the line
    int counter = 0;  
    
    // Print the shell prompt
    printf("> ");
    
    // Get a line of stdin, storing the string pointer in line and length in line_length
    if(getline(&line, &line_length, stdin) == -1) {
      if(errno == EINVAL) {
        perror("Unable to read command line");
        exit(2);
      } else {
        // Must have been end of file (ctrl+D)
        printf("\nShutting down...\n");
        exit(0);
      }
    }

    char** inputs = (char**) malloc (strlen(line)); 
    char* word = strtok (line, " ");
    while (word != NULL) {
      inputs[counter] = word;
      //printf ("word is %s \n", inputs[counter]); 
      counter++;
      word = strtok (NULL, " ");
    }
    // Append null terminator
    inputs[counter] = NULL;  

    pid_t child_id = fork();

    int status;
    int exec_status;
    if (child_id == 0) {
      exec_status = execvp (inputs[0], inputs);
      if (exec_status < 0) {
        perror("Error");
        exit(2);
      }
    } else {
      wait(&status);
      printf ("Child process %d exited with status %d\n", child_id, status); 
    } 
      
    free(line);
  }
  
  return 0;
}
