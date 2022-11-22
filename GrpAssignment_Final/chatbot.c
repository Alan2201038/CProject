/*
 * INF1002 (C Language) Group Project.
 *
 * This file implements the behaviour of the chatbot. The main entry point to
 * this module is the chatbot_main() function, which identifies the intent
 * using the chatbot_is_*() functions then invokes the matching chatbot_do_*()
 * function to carry out the intent.
 *
 * chatbot_main() and chatbot_do_*() have the same method signature, which
 * works as described here.
 *
 * Input parameters:
 *   inc      - the number of words in the question
 *   inv      - an array of pointers to each word in the question
 *   response - a buffer to receive the response
 *   n        - the size of the response buffer
 *
 * The first word indicates the intent. If the intent is not recognised, the
 * chatbot should respond with "I do not understand [intent]." or similar, and
 * ignore the rest of the input.
 *
 * If the second word may be a part of speech that makes sense for the intent.
 *    - for WHAT, WHERE and WHO, it may be "is" or "are".
 *    - for SAVE, it may be "as" or "to".
 *    - for LOAD, it may be "from".
 * The word is otherwise ignored and may be omitted.
 *
 * The remainder of the input (including the second word, if it is not one of
 * the above) is the entity.
 *
 * The chatbot's answer should be stored in the output buffer, and be no longer
 * than n characters long (you can use snprintf() to do this). The contents of
 * this buffer will be printed by the main loop.
 *
 * The behaviour of the other functions is described individually in a comment
 * immediately before the function declaration.
 *
 * You can rename the chatbot and the user by changing chatbot_botname() and
 * chatbot_username(), respectively. The main loop will print the strings
 * returned by these functions at the start of each line.
 */

#include "chat1002.h"
#include <stdio.h>
#include <string.h>

/*
 * Get the name of the chatbot.
 *
 * Returns: the name of the chatbot as a null-terminated string
 */
const char *chatbot_botname() { return "Zeus"; }

/*
 * Get the name of the user.
 *
 * Returns: the name of the user as a null-terminated string
 */
const char *chatbot_username() { return "Prometheus"; }

/*
 * Get a response to user input.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0, if the chatbot should continue chatting
 *   1, if the chatbot should stop (i.e. it detected the EXIT intent)
 */
int chatbot_main(int inc, char *inv[], char *response, int n) {

  /* check for empty input */
  if (inc < 1) {
    snprintf(response, n, "");
    return 0;
  }

  /*if (inc > 256) {
    snprintf(response, n, "Too many characters, Please try again.");
    return 0;
  }*/

  /* look for an intent and invoke the corresponding do_* function */
  if (chatbot_is_exit(inv[0]))
    return chatbot_do_exit(inc, inv, response, n);
  else if (chatbot_is_load(inv[0]))
    return chatbot_do_load(inc, inv, response, n);
  else if (chatbot_is_question(inv[0]))
    return chatbot_do_question(inc, inv, response, n);
  else if (chatbot_is_reset(inv[0]))
    return chatbot_do_reset(inc, inv, response, n);
  else if (chatbot_is_save(inv[0]))
    return chatbot_do_save(inc, inv, response, n);
  else {
    snprintf(response, n, "I don't understand \"%s\".", inv[0]);
    return 0;
  }
}

/*
 * Determine whether an intent is EXIT.
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  1, if the intent is "exit" or "quit"
 *  0, otherwise
 */
int chatbot_is_exit(const char *intent) {

  return compare_token(intent, "exit") == 0 ||
         compare_token(intent, "quit") == 0;
}

/*
 * Perform the EXIT intent.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0 (the chatbot always continues chatting after a question)
 */
int chatbot_do_exit(int inc, char *inv[], char *response, int n) {

  snprintf(response, n, "Goodbye!");

  return 1;
}

/*
 * Determine whether an intent is LOAD.
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  1, if the intent is "load"
 *  0, otherwise
 */
int chatbot_is_load(const char *intent) {

  /* to be implemented */
  return compare_token(intent, "load") == 0;
}

/*
 * Load a chatbot's knowledge base from a file.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0 (the chatbot always continues chatting after loading knowledge)
 */
int chatbot_do_load(int inc, char *inv[], char *response, int n) {
  /* to be implemented */
  int entity_count;
  if (inc > 1) {
    int filePosition = 1;
    if (compare_token(inv[1], "from") == 0 ||
        compare_token(inv[1], "as") == 0) {
      filePosition = 2;
    }
    char fileStr[MAX_INPUT];
    for (int i = 0; i < MAX_INPUT; i++) {
      fileStr[i] = '\0';
    }
    for (int i = filePosition; i < inc; i++) {
      if (i > filePosition) {
        strcat(fileStr, " ");
      }
      strcat(fileStr, inv[i]);
    }
    // printf("%s\n", fileStr);
    FILE *f;
    f = fopen(fileStr, "r");
    if (f == NULL) {
      snprintf(response, n, "Can't open file. Please enter a correct file.");
      return 0;
    }

    entity_count = knowledge_read(f);
    snprintf(response, n,
             "I have read %d entities. I have %s %s into my system.",
             entity_count, inv[0], fileStr);
    fclose(f);
    // snprintf(response, n, "I have %s %s into my system.", inv[0], fileStr);
    return 0;
  } else {
    snprintf(response, n, "Please enter a file name after the load command!");
    return 0;
  }
}

/*
 * Determine whether an intent is a question.
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  1, if the intent is "what", "where", or "who"
 *  0, otherwise
 */
int chatbot_is_question(const char *intent) {

  /* to be implemented */

  return compare_token(intent, "what") == 0 ||
         compare_token(intent, "where") == 0 ||
         compare_token(intent, "who") == 0;
}

/*
 * Answer a question.
 *
 * inv[0] contains the the question word.
 * inv[1] may contain "is" or "are"; if so, it is skipped.
 * The remainder of the words form the entity.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0 (the chatbot always continues chatting after a question)
 */
int chatbot_do_question(int inc, char *inv[], char *response, int n) {

  int entityPosition = 1;
  if (inc <= 1) {
    snprintf(response, n, "Please enter an entity.");
    return 0;
  }
  if (inv[1]){
    if (compare_token(inv[1], "is") == 0 || compare_token(inv[1], "are") == 0) {
      entityPosition = 2;
    }
  }

  if (inv[entityPosition + 1]&&inv[entityPosition]) {
    if (compare_token(inv[entityPosition], "the") == 0) {
      entityPosition++;
    }
  }

  if (inc <= entityPosition) {
    snprintf(response, n, "Please enter an entity.");
    return 0;
  }

  char entityStr[MAX_INPUT];
  for (int i = 0; i < MAX_INPUT; i++) {
    entityStr[i] = '\0';
  }
  for (int i = entityPosition; i < inc; i++) {
    if (i > entityPosition) {
      strcat(entityStr, " ");
    }
    strcat(entityStr, inv[i]);
  }

  if (knowledge_get(inv[0], entityStr, response, n) == 0) {
    knowledge_get(inv[0], entityStr, response, n);
  } else {
    char answer[MAX_INPUT];
    prompt_user(answer, n, "I don't know. %s is %s?", inv[0], entityStr);
    if (answer[0] == NULL) {
      do {
        prompt_user(answer, n, "Please enter something.");
      } while (answer[0] == NULL);
    }
    knowledge_put(inv[0], entityStr, answer);
    snprintf(response, n, "Thank you.");
  }
  return 0;

  // /* to be implemented */
  //	knowledge_get(inv[0],inv[2],response,n);
  //	return 0;
}

/*
 * Determine whether an intent is RESET.
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  1, if the intent is "reset"
 *  0, otherwise
 */
int chatbot_is_reset(const char *intent) {

  return compare_token(intent, "reset") == 0;
}

/*
 * Reset the chatbot.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0 (the chatbot always continues chatting after beign reset)
 */
int chatbot_do_reset(int inc, char *inv[], char *response, int n) {
  knowledge_reset();
  snprintf(response, n, "Chatbot Reset.");
  return 0;
}

/*
 * Determine whether an intent is SAVE.
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  1, if the intent is "what", "where", or "who"
 *  0, otherwise
 */
int chatbot_is_save(const char *intent) {

  /* to be implemented */

  return compare_token(intent, "save") == 0;
}

/*
 * Save the chatbot's knowledge to a file.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0 (the chatbot always continues chatting after saving knowledge)
 */
int chatbot_do_save(int inc, char *inv[], char *response, int n) {
  if (inc > 1) {
    int filePosition = 1;
    if (compare_token(inv[1], "to") == 0 || compare_token(inv[1], "as") == 0) {
      filePosition = 2;
    }
    char fileStr[MAX_INPUT];
    for (int i = 0; i < MAX_INPUT; i++) {
      fileStr[i] = '\0';
    }

    for (int i = filePosition; i < inc; i++) {
      if (i > filePosition) {
        strcat(fileStr, " ");
      }
      strcat(fileStr, inv[i]);
    }

    FILE *f;
    f = fopen(fileStr, "w");
    if (f == NULL) {
      snprintf(response, n, "I can't write to that file.");
      return 0;
    }
    knowledge_write(f);
    fclose(f);
    snprintf(response, n, "My knowledge has been saved to %s.", fileStr);
    return 0;
  } else {
    snprintf(response, n, "Please enter a file name after the save command!");
    return 0;
  }
}