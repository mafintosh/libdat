#include "storage.h"

static uv_loop_t *loop = NULL;

static void on_uv_open (uv_fs_t* req) {
  _dat_storage_req* open_req = (_dat_storage_req *) req;
  dat_storage_t *self = req->data;

  void *ctx = open_req->ctx;
  void (*on_open)(struct dat_storage_t* self, long result, void* ctx) = open_req->callback;
  long result = req->result;

  uv_fs_req_cleanup(req);

  if (on_open) {
    on_open(self, result, ctx);
  }
}

static void on_read (uv_fs_t *req) {

}

void dat_storage_init (dat_storage_t* self, char *path) {
  if (!loop) loop = uv_default_loop();

  self->on_open = NULL;
  self->on_read = NULL;
  self->on_write = NULL;
  self->on_close = NULL;
  self->path = path;
}

void dat_storage_open (dat_storage_t* self, void *ctx, void (*on_open)(struct dat_storage_t* self, long result, void* ctx)) {
  _dat_storage_req* open_req = &(self->_open_req);
  uv_fs_t* req = (uv_fs_t *) open_req;

  req->data = self;
  open_req->ctx = ctx;
  open_req->callback = on_open;

  uv_fs_open(loop, req, self->path, O_RDONLY, 0, on_uv_open);
}

void dat_storage_read (dat_storage_t* self, void *ctx, long offset, uv_buf_t *buf) {

}
