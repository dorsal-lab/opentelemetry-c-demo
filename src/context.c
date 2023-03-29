/**
 * @file context.c
 * @brief This file give utility functions for combining context and message
 * Instead of sending message and context in two separate messages, use
 * add_context_to_message() function to combine both and send one message only.
 * split_message_and_context() could be use to invert the operation.
 */

#include "context.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Combine a string context to a string message
 *
 * Use the method if you need to combine message and context as one message to
 * avoid send message and context separately.
 *
 * @param message The message
 * @param context The context
 * @return char* The message with its context
 */
char *add_context_to_message(const char *message, const char *context) {
  unsigned long message_length = strlen(message);
  unsigned long context_length = strlen(context);
  char *buffer = malloc(sizeof(char) * (20 + context_length + message_length));
  sprintf(buffer, "%ld %s %s", context_length, context, message);
  return buffer;
}

/**
 * @brief Reverse add_context_to_message function operation
 *
 * @param message_and_context The message and its context
 * @param message (Out parameter) The message
 * @param context (Out parameter) The context
 */
void split_message_and_context(const char *message_and_context, char **message,
                               char **context) {
  char buffer[20];
  int index = 0;
  for (; message_and_context[index] != ' '; index++) {
    buffer[index] = message_and_context[index];
  }
  index++;

  int context_length = atoi(buffer);
  *context = malloc(sizeof(char) * (context_length + 1));
  memcpy(*context, &message_and_context[index], context_length);
  (*context)[context_length] = '\0';

  int message_length = strlen(message_and_context) - index - context_length - 1;
  *message = malloc(sizeof(char) * (message_length + 1));
  memcpy(*message, &message_and_context[index + context_length + 1],
         message_length);
  (*message)[message_length] = '\0';
}
