/* Dcache Control Library
 * Copyright (C) 2017 dkato
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "cmsis.h"
#include "dcache-control.h"
#if defined(TARGET_RZ_A2XX)
#include "r_cache_lld_rza2m.h"
#endif

void dcache_clean(void * p_buf, uint32_t size) {
    uint32_t start_addr = (uint32_t)p_buf & 0xFFFFFFE0;
    uint32_t end_addr   = (uint32_t)p_buf + size;
    uint32_t addr;

    /* Data cache clean */
    for (addr = start_addr; addr < end_addr; addr += 0x20) {
        L1C_CleanDCacheMVA((void *)addr);
    }
#if defined(TARGET_RZ_A2XX)
    R_CACHE_L2CleanAll();
#endif
}

void dcache_invalid(void * p_buf, uint32_t size) {
    uint32_t start_addr = (uint32_t)p_buf & 0xFFFFFFE0;
    uint32_t end_addr   = (uint32_t)p_buf + size;
    uint32_t addr;

    /* Data cache invalid */
    for (addr = start_addr; addr < end_addr; addr += 0x20) {
        L1C_InvalidateDCacheMVA((void *)addr);
    }
#if defined(TARGET_RZ_A2XX)
    R_CACHE_L2InvalidAll();
#endif
}

