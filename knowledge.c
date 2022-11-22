/*
 * INF1002 (C Language) Group Project.
 *
 * This file implements the chatbot's knowledge base.
 *
 * knowledge_get() retrieves the response to a question.
 * knowledge_put() inserts a new response to a question.
 * knowledge_read() reads the knowledge base from a file.
 * knowledge_reset() erases all of the knowledge.
 * knowledge_write() saves the knowledge base in a file.
 *
 * You may add helper functions as necessary.
 */

#include "chat1002.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*Additional declaration of global variables for head pointers*/
Node *who_ptr;
Node *what_ptr;
Node *where_ptr;

/*
 * Helper function to push a new node to the end of the chosen linked list
 * Would also help to check if a node with the same entity is in the current
 * linked list. If so, it would update the response.
 *
 * Input:
 *   head    - the head node of the chosen linked list
 *   new_node    - the new_node that we want to push onto our linked list
 *
 */

void push_to_list(Node *head, Node *new_node) {
  Node *curr_ptr = head;
  while (curr_ptr != NULL) {
    if (compare_token(new_node->entity, curr_ptr->entity) == 0) {
      strcpy(curr_ptr->response, new_node->response);
    }
    if (curr_ptr->next == NULL) {
      curr_ptr->next = new_node;
      break;
    }
    curr_ptr = curr_ptr->next;
  }
}

/*
 * Helper function to write the entity and response for an intent into a file
 *
 * Input:
 *   f    - file pointer
 *   entity    - entity string
 *   response  - response string
 *   buffer    - string to be written to the line in file
 *   delimiter    - the string containing the "=" delimiter
 *   end    - the string containing the end of a line "\n\0"
 *
 */

void write_section_to_file(FILE *f, char *entity, char *response, char *buffer,
                           const char *delimiter, const char *end) {
  memcpy(buffer, entity, strlen(entity));
  memcpy(buffer + strlen(entity), delimiter, strlen(delimiter));
  memcpy(buffer + strlen(entity) + strlen(delimiter), response,
         strlen(response));
  memcpy(buffer + strlen(entity) + strlen(delimiter) + strlen(response), end,
         strlen(end));
  fprintf(f, buffer);
  memset(buffer, 0, strlen(buffer));
}

/*
 * Helper function to help create a new_node
 *
 * Input:
 *   entity    - the question word
 *   response    - the entity
 *
 * Returns:
 *   NULL, if there is memory allocation error
 *   A pointer to the new node
 */

Node *create_node(const char *entity, const char *response) {

  Node *new_node = malloc(sizeof(Node));

  if (new_node == NULL) {
    return NULL;
  } else {
    snprintf(new_node->entity, MAX_ENTITY, "%s", entity);
    snprintf(new_node->response, MAX_RESPONSE, "%s", response);
    new_node->next = NULL;
    return new_node;
  }
}

// Reading of ini files

/*
 * Get the response to a question.
 *
 * Input:
 *   intent   - the question word
 *   entity   - the entity
 *   response - a buffer to receive the response
 *   n        - the maximum number of characters to write to the response buffer
 *
 * Returns:
 *   KB_OK, if a response was found for the intent and entity (the response is
 * copied to the response buffer) KB_NOTFOUND, if no response could be found
 *   KB_INVALID, if 'intent' is not a recognised question word
 */
int knowledge_get(const char *intent, const char *entity, char *response,
                  int n) {
  int res = KB_INVALID;

  Node *temp_ptr = NULL;
  if (compare_token(intent, "who") == 0) {
    temp_ptr = who_ptr;
  } else if (compare_token(intent, "what") == 0) {
    temp_ptr = what_ptr;
  } else if (compare_token(intent, "where") == 0) {
    temp_ptr = where_ptr;
  }
  while (temp_ptr != NULL) {
    if (compare_token(temp_ptr->entity, entity) == 0) {
      snprintf(response, n, "%s", temp_ptr->response);
      res = KB_OK;

      break;
    }
    if (temp_ptr->next == NULL) {
      res = KB_NOTFOUND;
    }
    temp_ptr = temp_ptr->next;
  }
  return res;
}

/*
 * Insert a new response to a question. If a response already exists for the
 * given intent and entity, it will be overwritten. Otherwise, it will be added
 * to the knowledge base.
 *
 * Input:
 *   intent    - the question word
 *   entity    - the entity
 *   response  - the response for this question and entity
 *
 * Returns:
 *   KB_FOUND, if successful
 *   KB_NOMEM, if there was a memory allocation failure
 *   KB_INVALID, if the intent is not a valid question word
 */
int knowledge_put(const char *intent, const char *entity,
                  const char *response) {
  /*This function will be called each time there is a new entity/response pair
  that is unknown, which we will then create a node containing the
  entity/response in the appropriate linked list*/
  int returns = KB_INVALID;

  // Create a new temporary Node to store the data
  Node *temp = create_node(entity, response);

  if (temp == NULL) {
    returns = KB_NOMEM;
  }

  // Checks if the intent is "who
  if (compare_token(intent, "who") == 0) {
    if (who_ptr == NULL) {
      who_ptr = temp;
    } else {
      push_to_list(who_ptr, temp);
    }
    returns = KB_OK;
  }
  // Checks if the intent is "what"
  else if (compare_token(intent, "what") == 0) {
    if (what_ptr == NULL) {
      what_ptr = temp;
    } else {
      push_to_list(what_ptr, temp);
    }
    returns = KB_OK;
  }
  // Checks if the intent is "where"
  else if (compare_token(intent, "where") == 0) {
    if (where_ptr == NULL) {
      where_ptr = temp;
    } else {
      push_to_list(where_ptr, temp);
    }
    returns = KB_OK;
  } else {
    returns = KB_INVALID;
  }
  return returns;
}

/*
 * Read a knowledge base from a file.
 *
 * Input:
 *   f - the file
 *
 * Returns: the number of entity/response pairs successful read from the file
 */
int knowledge_read(FILE *f) {

  int s_pairs = 0;
  char buffer[MAX_ENTITY + MAX_RESPONSE + 2];
  char *start=NULL, *end=NULL, *intent = NULL, *token=NULL;
  Node *temp_ptr = NULL;
  const char delimiter[2] = "=";
  int entity_count = 0;

  char *entity, *response;
  memset(buffer, 0, strlen(buffer));
  while (fgets(buffer, MAX_ENTITY + MAX_RESPONSE + 2, f) != NULL) {
    start = strstr(buffer, "[");
    if (start != NULL) {
      start += strlen("[");
      end = strstr(buffer, "]");
      if (end != NULL) {
        intent = (char *)malloc(sizeof(start));
        memcpy(intent, start, strlen(start) - strlen(end));
        intent[end - start] = '\0';
      }
    }
    else if (strstr(buffer, delimiter) && intent != NULL) {
      if (compare_token(intent, "what") == 0 ||
          compare_token(intent, "where") == 0 ||
          compare_token(intent, "who") == 0) 
      {
        //only saves
        token = strtok(buffer, delimiter);
        entity = token;
        token = strtok(NULL, delimiter);
        response = token;
        if (response) {
          for (int i = 0; i < strlen(response); i++) {
            if (response[i] == '\n') {
              response[i] = '\0';
            }
          }
        }
        int success=knowledge_put(intent, entity, response);
        if(success==-3){
          return -1;
        }
        else if(success==0){
          entity_count++;
        }
        
      }
      else 
      {
        free(intent);
        continue;
      }
    }
  }
  return entity_count;
}

/*
 * Reset the knowledge base, removing all know entitities from all intents.
 */
void knowledge_reset() {
  // Creating the temp pointer to traverse linked list
  Node *current_ptr;
  // Freeing the memory for the who linked list
  while (what_ptr != NULL) {
    current_ptr = what_ptr;
    what_ptr = what_ptr->next;
    free(current_ptr);
  }
  // freeing the memory allocated for the "where" linked list
  while (where_ptr != NULL) {
    current_ptr = where_ptr;
    where_ptr = where_ptr->next;
    free(current_ptr);
  }
  // freeing the memory allocated for the "who" linked list
  while (who_ptr != NULL) {
    current_ptr = who_ptr;
    who_ptr = who_ptr->next;
    free(current_ptr);
  }

  what_ptr = NULL;
  where_ptr = NULL;
  who_ptr = NULL;
}

/*
 * Write the knowledge base to a file.
 *
 * Input:
 *   f - the file
 */
void knowledge_write(FILE *f) { 
  char buffer[MAX_ENTITY + MAX_RESPONSE + 2];
  Node *temp_ptr = NULL;
  const char delimiter[2] = "=";
  const char end[3] = "\n\0";
  char *entity, *response;
  memset(buffer, 0, sizeof(buffer));
  if (who_ptr != NULL) {
    fprintf(f, "[who]\n");
    temp_ptr = who_ptr;
    while (temp_ptr != NULL) {
      entity = temp_ptr->entity;
      response = temp_ptr->response;
      write_section_to_file(f, entity, response, buffer, delimiter, end);
      temp_ptr = temp_ptr->next;
    }
    fprintf(f, "\n");
  }
  if (what_ptr != NULL) {
    fprintf(f, "[what]\n");
    temp_ptr = what_ptr;
    while (temp_ptr != NULL) {
      entity = temp_ptr->entity;
      response = temp_ptr->response;
      write_section_to_file(f, entity, response, buffer, delimiter, end);
      temp_ptr = temp_ptr->next;
    }
    fprintf(f, "\n");
  }
  if (where_ptr != NULL) {
    fprintf(f, "[where]\n");
    temp_ptr = where_ptr;
    while (temp_ptr != NULL) {
      entity = temp_ptr->entity;
      response = temp_ptr->response;
      write_section_to_file(f, entity, response, buffer, delimiter, end);
      temp_ptr = temp_ptr->next;
    }
    fprintf(f, "\n");
  }
}