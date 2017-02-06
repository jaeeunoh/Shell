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
    int child_pid = waitpid(0, &status, WNOHANG); 
    if (child_pid > 0) { 
      printf ("Child process %d exited with status %d\n", child_pid, status);  
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

    // Check to see if the input contains a semicolon
    if (strchr (line, ';')) { 
      int inital_counter = 0; 
      char** inputsWithSemicolon = (char**) malloc (strlen(line)); 
      char* tokenizeBySemicolon = strtok (line, ";\n");
      while (tokenizeBySemicolon != NULL) { 
        inputsWithSemicolon[inital_counter] = tokenizeBySemicolon; 
        inital_counter++;
        tokenizeBySemicolon = strtok (NULL, ";\n");   
      }
      for (int i = 0; i < inital_counter; i++) { 
        char** inputs = (char**) malloc (strlen(inputsWithSemicolon[i])); 
        tokenize_inputs (inputsWithSemicolon[i], inputs); 
        run_command(inputs); 
      }
    } else { 
      char** inputs = (char**) malloc (strlen(line));  
      tokenize_inputs(line, inputs); 
      run_command(inputs); 
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




   