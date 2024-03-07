#ifndef UTILS_H
#define UTILS_H

#define BASE_TEN 10

#include <netinet/in.h>

in_port_t      parse_in_port_t(const char *binary_name, const char *str);
void           convert_address(const char *address, struct sockaddr_storage *addr);
_Noreturn void usage(const char *program_name, int exit_code, const char *message);

#endif /* UTILS_H */
