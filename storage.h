#ifndef DAT_STORAGE_H
#define DAT_STORAGE_H

#include <uv.h>

typedef struct {
  uv_fs_t req;
  void* ctx;
  void* callback;
} _dat_storage_req;

typedef struct dat_storage_t {
  // public

  void (*on_open)(struct dat_storage_t* self, long result, void* ctx);
  void (*on_close)(struct dat_storage_t* self, long result, void* ctx);
  void (*on_read)(struct dat_storage_t* self, long result, void* ctx);
  void (*on_write)(struct dat_storage_t* self, long result, void* ctx);

  char *path;

  // private

  _dat_storage_req _open_req;

} dat_storage_t;

void dat_storage_init (dat_storage_t *self, char *path);
void dat_storage_open (dat_storage_t *self, void *ctx, void (*on_open)(struct dat_storage_t* self, long result, void* ctx));
void dat_storage_read (dat_storage_t *self, void *ctx, long offset, uv_buf_t *buf);
void dat_storage_write (dat_storage_t *self, void *ctx, long offset, uv_buf_t *buf);
void dat_storage_close (dat_storage_t *self, void *ctx);

#endif
