//===- test.cpp -------------------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2020 Xilinx Inc.
//
//===----------------------------------------------------------------------===//

#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <thread>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <xaiengine.h>
#include "test_library.h"

#define LOCK_TIMEOUT 1000
#define HIGH_ADDR(addr)	((addr & 0xffffffff00000000) >> 32)
#define LOW_ADDR(addr)	(addr & 0x00000000ffffffff)
#define MLIR_STACK_OFFSET 4096

#define BAR_PF0_DEV_FILE_AIE    "/sys/bus/pci/devices/0000:21:00.0/resource2" 

#include "aie_inc.cpp"

int
main(int argc, char *argv[])
{
    printf("test start.\n");

    char bar_dev_file_aie[100];
    strcpy(bar_dev_file_aie, BAR_PF0_DEV_FILE_AIE);
    printf("Opening %s to access MMIO AIE\n", bar_dev_file_aie);

    // Opening BAR2
    int fda;
    if((fda = open(bar_dev_file_aie, O_RDWR | O_SYNC)) == -1) {
        printf("[ERROR] Failed to open device file\n");
        return 1;
    }
    printf("device file opened\n");

    // Map the memory region into userspace
    volatile void *map_aie_base = mmap(NULL,    // virtual address
                        0x20000000,             // length
                        PROT_READ | PROT_WRITE, // prot
                        MAP_SHARED,             // flags
                        fda,                    // device fd
                        0);                     // offset
    if (!map_aie_base) return 1;
    printf("AIE registers mapped into userspace at %p\n",map_aie_base);

    aie_libxaie_ctx_t *_xaie = mlir_aie_init_libxaie(map_aie_base);
    mlir_aie_init_device(_xaie);

    mlir_aie_configure_cores(_xaie);
    mlir_aie_configure_switchboxes(_xaie);
    mlir_aie_configure_dmas(_xaie);
    mlir_aie_initialize_locks(_xaie);

    int errors = 0;

    printf("Acquire input buffer lock first.\n");
    mlir_aie_acquire_lock(_xaie, 31, 3, 3, 0, LOCK_TIMEOUT); // Should this part of setup???

    mlir_aie_clear_tile_memory(_xaie, 41, 3);
    mlir_aie_clear_tile_memory(_xaie, 43, 2);
    mlir_aie_clear_tile_memory(_xaie, 43, 3);
    mlir_aie_clear_tile_memory(_xaie, 43, 4);
    mlir_aie_write_buffer_a13(_xaie, 3, 7);

    mlir_aie_check("Before start cores:", mlir_aie_read_buffer_a13(_xaie, 3), 7,
                   errors);
    mlir_aie_check("Before start cores:", mlir_aie_read_buffer_b13(_xaie, 5), 0,
                   errors);
    mlir_aie_check("Before start cores:", mlir_aie_read_buffer_a32(_xaie, 5), 0,
                   errors);
    mlir_aie_check("Before start cores:", mlir_aie_read_buffer_b32(_xaie, 5), 0,
                   errors);
    mlir_aie_check("Before start cores:", mlir_aie_read_buffer_a33(_xaie, 5), 0,
                   errors);
    mlir_aie_check("Before start cores:", mlir_aie_read_buffer_b33(_xaie, 5), 0,
                   errors);
    mlir_aie_check("Before start cores:", mlir_aie_read_buffer_a34(_xaie, 5), 0,
                   errors);
    mlir_aie_check("Before start cores:", mlir_aie_read_buffer_b34(_xaie, 5), 0,
                   errors);

    printf("Start cores\n");
    mlir_aie_start_cores(_xaie);

    mlir_aie_check("After start cores:", mlir_aie_read_buffer_b13(_xaie, 5), 0,
                   errors);
    mlir_aie_check("After start cores:", mlir_aie_read_buffer_a32(_xaie, 5), 0,
                   errors);
    mlir_aie_check("After start cores:", mlir_aie_read_buffer_b32(_xaie, 5), 0,
                   errors);
    mlir_aie_check("After start cores:", mlir_aie_read_buffer_a33(_xaie, 5), 0,
                   errors);
    mlir_aie_check("After start cores:", mlir_aie_read_buffer_b33(_xaie, 5), 0,
                   errors);
    mlir_aie_check("After start cores:", mlir_aie_read_buffer_a34(_xaie, 5), 0,
                   errors);
    mlir_aie_check("After start cores:", mlir_aie_read_buffer_b34(_xaie, 5), 0,
                   errors);

    printf("Release input buffer lock.\n");
    mlir_aie_release_lock(_xaie, 41, 3, 3, 1, LOCK_TIMEOUT);

    printf("Waiting to acquire output lock for read tile[3][2]...\n");
    if (!mlir_aie_acquire_lock(_xaie, 43, 2, 7, 1, LOCK_TIMEOUT)) {
      printf("ERROR: timeout hit!\n");
    }

    mlir_aie_print_dma_status(_xaie, 43, 2);

    mlir_aie_check("After acquire lock:", mlir_aie_read_buffer_b13(_xaie, 5),
                   35, errors);
    mlir_aie_check("After acquire lock:", mlir_aie_read_buffer_a32(_xaie, 5),
                   35, errors);
    mlir_aie_check("After acquire lock:", mlir_aie_read_buffer_b32(_xaie, 5),
                   105, errors);

    printf("Waiting to acquire output lock for read tile[3][3]...\n");
    if (!mlir_aie_acquire_lock(_xaie, 43, 3, 7, 1, LOCK_TIMEOUT)) {
      printf("ERROR: timeout hit!\n");
    }
    mlir_aie_check("After acquire lock:", mlir_aie_read_buffer_a33(_xaie, 5),
                   35, errors);
    mlir_aie_check("After acquire lock:", mlir_aie_read_buffer_b33(_xaie, 5),
                   140, errors);

    printf("Waiting to acquire output lock for read tile[3][4]...\n");
    if (!mlir_aie_acquire_lock(_xaie, 43, 4, 7, 1, LOCK_TIMEOUT)) {
      printf("ERROR: timeout hit!\n");
    }
    mlir_aie_check("After acquire lock:", mlir_aie_read_buffer_a34(_xaie, 5),
                   35, errors);
    mlir_aie_check("After acquire lock:", mlir_aie_read_buffer_b34(_xaie, 5),
                   175, errors);

    int res = 0;
    if (!errors) {
      printf("PASS!\n");
      res = 0;
    } else {
      printf("Fail!\n");
      res = -1;
    }
    mlir_aie_deinit_libxaie(_xaie);

    munmap((void *)map_aie_base,0x20000000);
    close(fda);

    printf("test done.\n");
    return res;
}
