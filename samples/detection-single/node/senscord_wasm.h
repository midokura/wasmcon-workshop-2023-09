#ifndef SENSCORD_WASM_H
#define SENSCORD_WASM_H

#include <stdint.h>

// this is not official senscord API, temoprary workaround until wasm memory
// allocation is supported
int senscord_memcpy(uint32_t wasm_addr, uint64_t native_addr, uint32_t size);

#endif /* SENSCORD_WASM_H */
