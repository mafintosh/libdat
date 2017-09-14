#include "storage.h"

static uv_loop_t *loop = NULL;

static void on_uv_callback (uv_fs_t *ureq) {
  dat_storage_req_t* req = (dat_storage_req_t *) ureq;
  dat_storage_t *self = ureq->data;
  dat_storage_callback_t cb = req->callback;
  long result = ureq->result;

  uv_fs_req_cleanup((uv_fs_t *) req);

  if (cb) {
    cb(self, result, req);
  }
}

static void on_uv_open (uv_fs_t* ureq) {
  dat_storage_t *self = ureq->data;
  long result = ureq->result;
  if (result > -1) self->_file = result;
  on_uv_callback(ureq);
}

void dat_storage_init (dat_storage_t* self, char *path) {
  if (!loop) loop = uv_default_loop();
  self->path = path;
}

void dat_storage_open (dat_storage_t* self, dat_storage_req_t* req, dat_storage_callback_t on_open) {
  req->req.data = self;
  req->callback = on_open;

  uv_fs_open(loop, (uv_fs_t *) req, self->path, O_RDONLY, 0, on_uv_open);
}

void dat_storage_read (dat_storage_t* self, dat_storage_req_t* req, dat_storage_callback_t on_read) {
  req->req.data = self;
  req->callback = on_read;

  uv_fs_read(loop, (uv_fs_t *) req, self->_file, &(req->buffer), 1, req->offset, on_uv_callback);
}
