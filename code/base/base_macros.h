#ifndef BASE_MACROS_H
#define BASE_MACROS_H

#define internal static
#define global static

#define __PACKED_STRUCT struct __attribute__((packed, aligned(1)))

__PACKED_STRUCT U32_WRITE { uint32_t v; };
#define UNALIGNED_U32_WRITE(addr, val) (void)((((struct U32_WRITE *)(void *)(addr))->v) = (val))

__PACKED_STRUCT U32_READ { uint32_t v; };
#define UNALIGNED_U32_READ(addr) (((const struct U32_READ *)(const void *)(addr))->v)

#endif // BASE_MACROS_H
