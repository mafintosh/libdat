#include "storage.h"

#define DAT_STORAGE_PROCESS_IO(ureq, fn, cb) \
  dat_storage_req_t* req = (dat_storage_req_t *) ureq; \
  dat_storage_t *self = ureq->data; \
  long result = ureq->result; \
  \
  if (result > 0) { \
    req->buffer_mut.len -= result; \
    if (req->buffer_mut.len > 0) { \
      req->buffer_mut.base += result; \
      size_t delta = req->buffer.len - req->buffer_mut.len; \
      uv_fs_req_cleanup(ureq); \
      fn(loop, (uv_fs_t *) req, self->file, &(req->buffer_mut), 1, req->offset + delta, cb); \
      return; \
    } \
  } else if (result == 0) { \
    if (req->buffer_mut.len > 0) { \
      ureq->result = -1; \
    } \
  } \
  \
  on_uv_callback(ureq);

static uv_loop_t *loop = NULL;

static void on_uv_callback (uv_fs_t *ureq) {
  dat_storage_req_t* req = (dat_storage_req_t *) ureq;
  dat_storage_t *self = ureq->data;
  dat_storage_callback_t cb = req->callback;
  long result = ureq->result;
  uv_fs_req_cleanup(ureq);

  if (cb) {
    cb(self, result, req);
  }
}

static void on_uv_stat (uv_fs_t* ureq) {
  dat_storage_t *self = ureq->data;
  long result = ureq->result;
  if (result > -1) {
    self->length = ureq->statbuf.st_size;
  }
  on_uv_callback(ureq);
}

static void on_uv_read (uv_fs_t* ureq) {
  DAT_STORAGE_PROCESS_IO(ureq, uv_fs_read, on_uv_read)
}

static void on_uv_write (uv_fs_t* ureq) {
  DAT_STORAGE_PROCESS_IO(ureq, uv_fs_write, on_uv_write)
}

static void on_uv_open (uv_fs_t* ureq) {
  dat_storage_t *self = ureq->data;
  long result = ureq->result;
  if (result > -1) {
    self->file = result;
    uv_fs_req_cleanup(ureq);
    uv_fs_fstat(loop, ureq, self->file, on_uv_stat);
  } else {
    on_uv_callback(ureq);
  }
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
  req->buffer_mut = req->buffer;
  req->callback = on_read;

  uv_fs_read(loop, (uv_fs_t *) req, self->file, &(req->buffer_mut), 1, req->offset, on_uv_read);
}

void dat_storage_write (dat_storage_t* self, dat_storage_req_t* req, dat_storage_callback_t on_write) {
  req->req.data = self;
  req->buffer_mut = req->buffer;
  req->callback = on_write;

  uv_fs_write(loop, (uv_fs_t *) req, self->file, &(req->buffer_mut), 1, req->offset, on_uv_write);
}
