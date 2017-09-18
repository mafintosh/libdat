#include <stdio.h>
#include <stdlib.h>
#include <uv.h>
#include "storage.h"

static dat_storage_req_t read_req;

void on_read (dat_storage_t* self, long result, dat_storage_req_t* req) {
  if (result > 0) {
    printf("(on_read (%ld bytes))\n", result);
    req->offset += result;
    // dat_storage_read(self, req, on_read);
    return;
  }
  // if (result == 0) {
  //   printf("(on_end)\n");
  //   return;
  // }

  printf("(on_read_error (%ld))\n", result);
}

void on_open (dat_storage_t* self, long result, dat_storage_req_t* req) {
  read_req.offset = 0;
  read_req.buffer.len = self->length;
  read_req.buffer.base = malloc(read_req.buffer.len);

  if (result > -1) {
    printf("(on_open)\n");
    dat_storage_read(self, &read_req, on_read);
    return;
  }

  printf("(on_open_error (%ld))\n", result);
}

int main (int argc, char** argv) {
  dat_storage_t st_r;
  dat_storage_t st_w;

  dat_storage_req_t open_req_r;
  dat_storage_req_t open_req_w;

  dat_storage_init(&st_r, "dat.c");
  dat_storage_open(&st_r, &open_req_r, on_open);

  printf("Running loop ...\n");

  uv_run(uv_default_loop(), UV_RUN_DEFAULT);
  uv_loop_close(uv_default_loop());

  printf("Exiting...\n");
  return 0;
}
