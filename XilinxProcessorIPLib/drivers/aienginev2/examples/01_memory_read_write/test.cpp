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

    mlir_aie_clear_tile_memory(_xaie, 23, 3);

    //XAie_CoreReset(&(_xaie->DevInst), XAie_TileLoc(23,3));
    //XAie_CoreDisable(&(_xaie->DevInst), XAie_TileLoc(23,3));
    mlir_aie_configure_cores(_xaie);
    mlir_aie_configure_switchboxes(_xaie);
    mlir_aie_initialize_locks(_xaie);
    mlir_aie_configure_dmas(_xaie);
    mlir_aie_start_cores(_xaie);

    int errors = 0;

    mlir_aie_check("After memory writes. Check [3]=14",
                   mlir_aie_read_buffer_a(_xaie, 3), 14, errors);
    mlir_aie_check("After memory writes. Check [5]=8",
                   mlir_aie_read_buffer_a(_xaie, 5), 8, errors);
    mlir_aie_check("After memory writes. Check [9]=14",
                   mlir_aie_read_buffer_a(_xaie, 9), 14, errors);

    int res = 0;
    if (!errors) {
      printf("PASS!\n");
      res = 0;
    } else {
      printf("Fail!\n");
      res = -1;
    }
    mlir_aie_deinit_libxaie(_xaie);

    printf("test done.\n");
    return res;
}
