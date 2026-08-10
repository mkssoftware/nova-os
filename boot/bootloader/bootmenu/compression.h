#ifndef NOVA_BOOT_COMPRESSION_H
#define NOVA_BOOT_COMPRESSION_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {NOVA_COMPRESSION_NONE,NOVA_COMPRESSION_LZ4,NOVA_COMPRESSION_ZSTD,
    NOVA_COMPRESSION_COUNT} nova_compression_type_t;
typedef enum {NOVA_COMPRESSION_OK,NOVA_COMPRESSION_INVALID_ARGUMENT,
    NOVA_COMPRESSION_UNSUPPORTED,NOVA_COMPRESSION_CORRUPT,
    NOVA_COMPRESSION_SOURCE_TRUNCATED,NOVA_COMPRESSION_DESTINATION_TOO_SMALL,
    NOVA_COMPRESSION_SIZE_MISMATCH} nova_compression_result_t;
typedef struct {
    uint32_t initializations,calls,none_calls,lz4_calls,zstd_rejections;
    uint32_t corrupt_streams,size_errors,integrity_errors;
    uint64_t compressed_bytes,original_bytes,written_bytes,workspace_bytes;
    uint32_t last_ratio_permille;
    nova_compression_type_t last_type;
    bool initialized;
} nova_compression_diagnostics_t;

bool nova_compression_initialize(void);
bool nova_compression_supported(nova_compression_type_t type);
nova_compression_result_t nova_resource_decompress(nova_compression_type_t type,
    const void *source,uint64_t source_size,void *destination,
    uint64_t destination_size,uint64_t *written);
const nova_compression_diagnostics_t *nova_compression_diagnostics(void);

#endif
