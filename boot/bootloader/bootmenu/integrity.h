#ifndef NOVA_BOOT_INTEGRITY_H
#define NOVA_BOOT_INTEGRITY_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {NOVA_INTEGRITY_PERMISSIVE,NOVA_INTEGRITY_STANDARD,
    NOVA_INTEGRITY_STRICT} nova_integrity_policy_t;
typedef enum {NOVA_RESOURCE_TRUST_UNKNOWN,NOVA_RESOURCE_TRUST_VALID,
    NOVA_RESOURCE_TRUST_SIGNED,NOVA_RESOURCE_TRUST_INVALID,
    NOVA_RESOURCE_TRUST_CORRUPTED} nova_resource_trust_t;
typedef enum {NOVA_SIGNATURE_NONE,NOVA_SIGNATURE_VERIFIED,
    NOVA_SIGNATURE_INVALID,NOVA_SIGNATURE_UNSUPPORTED} nova_signature_status_t;
typedef enum {NOVA_INTEGRITY_OK,NOVA_INTEGRITY_INVALID_ARGUMENT,
    NOVA_INTEGRITY_CHECKSUM_MISMATCH,NOVA_INTEGRITY_SIGNATURE_REQUIRED,
    NOVA_INTEGRITY_SIGNATURE_INVALID,NOVA_INTEGRITY_UNSUPPORTED}
nova_integrity_result_t;

typedef struct {
    uint64_t resource_id;
    uint32_t type,version;
    const void *data;
    uint64_t size;
    uint32_t expected_crc32;
    const uint8_t *expected_sha256;
    nova_signature_status_t signature;
    bool trusted_origin;
} nova_integrity_descriptor_t;

typedef struct {
    nova_integrity_result_t result;
    nova_resource_trust_t trust;
    uint32_t crc32;
    uint8_t sha256[32];
    bool crc_checked,sha256_checked,signature_valid;
} nova_integrity_report_t;

typedef struct {
    uint32_t initializations,verifications,valid,corrupted,invalid_signatures;
    uint32_t unsigned_rejections,crc32_checks,sha256_checks,cache_skips;
    uint64_t bytes_hashed,workspace_bytes;
    nova_integrity_policy_t policy;
    nova_integrity_result_t last_error;
    uint64_t last_resource_id;
    bool initialized;
} nova_integrity_diagnostics_t;

bool nova_integrity_initialize(nova_integrity_policy_t policy);
bool nova_integrity_set_policy(nova_integrity_policy_t policy);
nova_integrity_policy_t nova_integrity_policy(void);
uint32_t nova_integrity_crc32(const void *data,uint64_t size);
bool nova_integrity_sha256(const void *data,uint64_t size,uint8_t digest[32]);
nova_integrity_result_t nova_resource_verify(const nova_integrity_descriptor_t *descriptor,
                                             nova_integrity_report_t *report);
void nova_integrity_record_cache_skip(void);
const nova_integrity_diagnostics_t *nova_integrity_diagnostics(void);

#endif
