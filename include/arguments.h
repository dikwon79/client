#ifndef ARGUMENTS_H
#define ARGUMENTS_H
#define UNKNOWN_OPTION_MESSAGE_LEN 24

#include <netinet/in.h>

void parse_arguments(int argc, char *argv[], char **ip_address, char **port);
void handle_arguments(const char *binary_name, const char *ip_address, const char *port_str, in_port_t *port);

#endif /* ARGUMENTS_H */
