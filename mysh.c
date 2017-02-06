#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_ARGS 128

void run_command(char** args);
void run_in_background (char** args);
int tokenize_inputs (char* line, char** inputs); 


int main(int argc, char** argv) {
  while(1) {
    char* line = NULL;    // Pointer that will hold the line we read in
    size_t line_length;   // Space for the length of the line


    // Print the exit child processes 
    int status; 
    pid_t child_id = waitpid(-1, &status, WNOHANG);
    if (child_id > 0) {
        printf ("Child process %d exited with status %d\n", child_id, WEXITSTATUS(status)); 
    }

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

    // If the user inputs exit as a command, returns; 
    if (strcmp (line, "exit\n") == 0) {
      exit(0); 
    } 

    
    // Citation: Below code is taken and modified from parser.c file provided by Professor Curtsinger. 
    char* pos = line;
    while(1) {
      char* end_pos = strpbrk(pos, "&;\n");
      
      if(end_pos == NULL || *end_pos == '\n') {
        char** inputs = (char**) malloc (strlen(pos));  
        tokenize_inputs(pos, inputs); 
        run_command(inputs); 
        // When end_pos is null or user enters '\n', end of the line is reached. 
        break;
      } else if(*end_pos == ';') {
        // Found a semicolon
        *end_pos = '\0';
        char** inputs = (char**) malloc (strlen(pos));  
        tokenize_inputs(pos, inputs); 
        run_command(inputs); 
      } else if(*end_pos == '&') {
        // Found an ampersand
        *end_pos = '\0';
        char** inputs = (char**) malloc (strlen(pos));  
        tokenize_inputs(pos, inputs); 
        run_in_background(inputs); 
      } else {
        // This should never happen, but just being thorough
        printf("Something strange happened!\n");
        break; 
      }

      // Move the position pointer to the beginning of the next split
      pos = end_pos +1;
    }
    
    free(line); 
  }
  return 0; 
} 

  void run_command(char** args) { 
    int status; 
    pid_t child_id = fork();
    int exec_status;
    // If the child creation process fails
    if (child_id < 0) {
      perror("Error");
    }
    // If the child creation is successful
    if (child_id == 0) {
      exec_status = execvp (args[0], args);
      // If the command fails to execute
      if (exec_status < 0) {
        perror("Error");
        exit(2);
      }
    } else {
      wait(&status);
      printf ("Child process %d exited with status %d\n", child_id, status); 
    } 
  }

  void run_in_background (char** args) { 
    int status; 
    pid_t child_id = fork();
    int exec_status;
    // If the child creation process fails
    if (child_id < 0) {
      perror("Error");
    }
    // If the child creation is successful
    if (child_id == 0) {
      exec_status = execvp (args[0], args);
      // If the command fails to execute
      if (exec_status < 0) {
        perror("Error");
        exit(2);
      }
    } else { 
      printf ("Child process %d exited with status %d\n", child_id, status); 
    }
  }


  int tokenize_inputs (char* line, char** inputs) { 
    int counter = 0; 
    char* word = strtok (line, " \n"); 
    if (strcmp (word, "cd") == 0) {
      word = "chdir";
      int error_msg = chdir(strtok(NULL, " \n"));
      if (error_msg < 0) {
        perror("Error");
      }
    } else { 
      while (word != NULL) {    
        inputs[counter] = word;
        counter++;
        word = strtok (NULL, " \n");
      }
      //Append null terminator
      inputs[counter] = NULL;
    }
    return counter;
  }   




   