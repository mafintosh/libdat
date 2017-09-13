#include <stdio.h>
#include <stdlib.h>
#include <uv.h>
#include "storage.h"

#include <unistd.h>

typedef struct {
  uv_fs_t open_req;
  uv_fs_t read_req;
  uv_fs_t write_req;

  uv_file file;

} file_storage_t;

void file_storage_read (file_storage_t* self, int offset, uv_buf_t *buf);

uv_buf_t buffer;
long offset;

void on_read (uv_fs_t* req) {
  long result = req->result;

  if (result < 0) {
    uv_fs_req_cleanup(req);
    printf("read error!!!\n");
    return;
  }

  write(1, buffer.base, (int) result);

  uv_fs_req_cleanup(req);

  if (result == buffer.len) {
    offset += result;
    file_storage_read(req->data, offset, &buffer);
  }
}

void on_write (uv_fs_t* req) {
  printf("onwrite %zu\n", req->result);

  uv_fs_req_cleanup(req);
}

void file_storage_read (file_storage_t* self, int offset, uv_buf_t *buf) {
  uv_fs_t* req = &(self->read_req);
  req->data = self;
  uv_fs_read(uv_default_loop(), req, self->file, buf, 1, offset, on_read);
}

void file_storage_write (file_storage_t* self, int offset, uv_buf_t *buf) {
  uv_fs_write(uv_default_loop(), &(self->write_req), self->file, buf, 1, offset, on_write);
}

void on_file_open (file_storage_t* self) {
  file_storage_read(self, 0, &buffer);
}

// void on_open (uv_fs_t* req) {
//   if (req->result < 0) {
//     printf("ERROR!!!\n");
//     return;
//   }

//   printf("file opened\n");

//   file_storage_t *fs = req->data;
//   fs->file = req->result;

//   uv_fs_req_cleanup(req);
//   on_file_open(fs);
// }

void on_open (dat_storage_t *self, long result, void *ctx) {
  printf("opened %s (%zu)\n", self->path, result);
}

void on_timer (uv_timer_t* handle) {
  printf("on timer!\n");
}

int main (int argc, char** argv) {
  offset = 0;
  buffer.len = 3200;
  buffer.base = malloc(buffer.len);

  dat_storage_t st;

  dat_storage_init(&st, "main.c");
  dat_storage_open(&st, NULL, on_open);

  // file_storage_t fs;

  // uv_fs_t* req = &(fs.open_req);
  // req->data = &fs;

  // uv_fs_open(uv_default_loop(), req, "main.c", O_RDONLY, 0, on_open);

  // uv_timer_t handle;

  // uv_timer_init(uv_default_loop(), &handle);
  // uv_timer_start(&handle, on_timer, 1000, 1000);

  printf("Running loop ...\n");

  uv_run(uv_default_loop(), UV_RUN_DEFAULT);
  uv_loop_close(uv_default_loop());

  printf("Exiting...\n");
  return 0;
}
