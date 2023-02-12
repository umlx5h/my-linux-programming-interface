#ifndef SYSLOG_PRINTF_H
#define SYSLOG_PRINTF_H

#define syslog(priority, ...) printf(__VA_ARGS__)

#endif
