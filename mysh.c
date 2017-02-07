#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#define MAX_ARGS 128

void run_command(char** args);
void run_in_background (char** args);
int tokenize_inputs (char* line, char** inputs);
void tokenize_by_special_chars (char* pos);


int main(int argc, char** argv) {
  while(1) {
    int status;
    char* line = NULL;    // Pointer that will hold the line we read in
    size_t line_length;   // Space for the length of the line

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

    line = strtok (line, "\n");


    // Exit if the user inputs exit as a command
    if (strcmp (line, "exit") == 0) {
      exit(0); 
    } 

    // Tokenize line by ampersand and semicolon
    char* pos = line; 
    tokenize_by_special_chars(pos);

    int child_pid;
    while ((child_pid = waitpid(-1, &status, WNOHANG)) > 0) { 
      printf("Child %d exited with status %d\n", child_pid, status);
    }

    free(line); 
  }

  return 0; 
} 

// Citation: Below code is taken and modified from parser.c file provided by Professor Curtsinger. 
void tokenize_by_special_chars (char* pos) {
  while(1) {
    // Check to see if line ends with ampersand or semicolon. 
    bool check = false; 
    if (pos[strlen(pos)-1] == '&' || pos[strlen(pos)-1] == ';') { 
      check = true;
    }
    char* end_pos = strpbrk(pos, "&;");

    if(end_pos == NULL) {
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
      if (check) { 
        break;
      }
    } else if(*end_pos == '&') {
      // Found an ampersand
      *end_pos = '\0';
      char** inputs = (char**) malloc (strlen(pos));  
      tokenize_inputs(pos, inputs); 
      run_in_background(inputs); 
      if (check) { 
        break;
      }
    } else {
      // This should never happen, but just being thorough
      printf("Something strange happened!\n");
      break;
    }

    // Move the position pointer to the beginning of the next split
    pos = end_pos +1;
  }
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
    int child_status;
    if(wait(&child_status) == -1) {
      perror("wait failed");
    } 
    printf("Child %d exited with status %d\n", getpid(), WEXITSTATUS(child_status));
  }
}

// 
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




   