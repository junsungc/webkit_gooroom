#ifndef TaskManager_h
#define TaskManager_h

#include <cstdint>
#include <string>
#include <vector>

#include <sys/types.h>

void add_web_page(uint64_t page_id, pid_t pid);
void remove_web_page(uint64_t page_id);
void update_page_url(uint64_t page_id, std::string url);

#endif
