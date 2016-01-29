#include "TaskManager.h"

#include <cstdio>
#include <string>
#include <vector>

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include <sqlite3.h>

#define DB_FILE ("/tmp/task-manager.db")
#define TMP_FILE ("/tmp/task-manager.db.tmp")
#define LOCK_FILE ("/tmp/task-manager.lock")

#define BUSY_WAIT_MS 10000

int busy_callback(void*, int) {
    return 0;
}

void ensure_task_manager() {
    if (access(DB_FILE, F_OK) == 0) {
        return;
    }

    int fd = open(LOCK_FILE, O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
    lockf(fd, F_LOCK, 0);
    if (access(DB_FILE, F_OK) == 0) {
        lockf(fd, F_ULOCK, 0);
        return;
    }

    sqlite3* db;
    sqlite3_open_v2(TMP_FILE, &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
    sqlite3_busy_handler(db, busy_callback, NULL);
    sqlite3_busy_timeout(db, BUSY_WAIT_MS);
    sqlite3_exec(db, "CREATE TABLE process(pid INTEGER, uid INTEGER);", NULL, NULL, NULL);
    sqlite3_exec(db, "CREATE TABLE permission(uid INTEGER PRIMARY KEY, perm_printer INTEGER, perm_network INTEGER, perm_usb INTEGER, perm_capture INTEGER, perm_harddisk INTEGER);", NULL, NULL, NULL);
    sqlite3_exec(db, "CREATE TABLE page_pid(page_id INTEGER PRIMARY KEY, pid INTEGER);", NULL, NULL, NULL);
    sqlite3_exec(db, "CREATE TABLE page_url(page_id INTEGER PRIMARY KEY, url TEXT);", NULL, NULL, NULL);
    sqlite3_close(db);
    chmod(TMP_FILE, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
    rename(TMP_FILE, DB_FILE);

    lockf(fd, F_ULOCK, 0);
    close(fd);
}

void add_web_page(uint64_t page_id, pid_t pid) {
    char query_buffer[256];

    ensure_task_manager();
    sqlite3* db;
    sqlite3_open_v2(DB_FILE, &db, SQLITE_OPEN_READWRITE, NULL);
    sqlite3_busy_handler(db, busy_callback, NULL);
    sqlite3_busy_timeout(db, BUSY_WAIT_MS);
    snprintf(query_buffer, sizeof(query_buffer), "INSERT OR REPLACE INTO page_pid(page_id, pid) VALUES (%lu, %d);", page_id, pid);
    sqlite3_exec(db, query_buffer, NULL, NULL, NULL);
    sqlite3_close(db);
}

void remove_web_page(uint64_t page_id) {
    char query_buffer[256];

    ensure_task_manager();
    sqlite3* db;
    sqlite3_open_v2(DB_FILE, &db, SQLITE_OPEN_READWRITE, NULL);
    sqlite3_busy_handler(db, busy_callback, NULL);
    sqlite3_busy_timeout(db, BUSY_WAIT_MS);
    snprintf(query_buffer, sizeof(query_buffer), "DELETE FROM page_pid WHERE page_id = %lu;", page_id);
    sqlite3_exec(db, query_buffer, NULL, NULL, NULL);
    snprintf(query_buffer, sizeof(query_buffer), "DELETE FROM page_url WHERE page_id = %lu;", page_id);
    sqlite3_exec(db, query_buffer, NULL, NULL, NULL);
    sqlite3_close(db);
}

void update_page_url(uint64_t page_id, std::string url) {
    char query_buffer[256];

    ensure_task_manager();
    sqlite3* db;
    sqlite3_open_v2(DB_FILE, &db, SQLITE_OPEN_READWRITE, NULL);
    sqlite3_busy_handler(db, busy_callback, NULL);
    sqlite3_busy_timeout(db, BUSY_WAIT_MS);
    snprintf(query_buffer, sizeof(query_buffer), "INSERT OR REPLACE INTO page_url(page_id, url) VALUES (%lu, '%.100s');", page_id, url.c_str());
    sqlite3_exec(db, query_buffer, NULL, NULL, NULL);
    sqlite3_close(db);
}
