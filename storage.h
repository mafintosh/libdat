#ifndef DAT_STORAGE_H
#define DAT_STORAGE_H

#include <uv.h>

typedef struct {
  // public

  char *path;
  uint64_t length;

  // private

  uv_file file;

} dat_storage_t;

typedef struct {
  uv_fs_t req;
  uv_buf_t buffer;
  uv_buf_t buffer_mut;
  uint64_t offset;
  void* ctx;
  void* callback;
} dat_storage_req_t;

typedef void (*dat_storage_callback_t)(dat_storage_t* self, long result, dat_storage_req_t* req);

void dat_storage_init (dat_storage_t *self, char *path);
void dat_storage_open (dat_storage_t *self, dat_storage_req_t* req, dat_storage_callback_t on_open);
void dat_storage_read (dat_storage_t *self, dat_storage_req_t* req, dat_storage_callback_t on_read);
void dat_storage_write (dat_storage_t *self, dat_storage_req_t* req, dat_storage_callback_t on_write);
void dat_storage_close (dat_storage_t *self, dat_storage_req_t* req, dat_storage_callback_t on_close);

#endif
