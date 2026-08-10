#include "integrity.h"

typedef struct {uint32_t h[8];uint64_t length;uint8_t block[64];uint8_t used;}
sha256_context_t;
static nova_integrity_diagnostics_t diagnostics;

static uint32_t rotate_right(uint32_t value,uint8_t bits)
{return (value>>bits)|(value<<(32u-bits));}

static void sha256_transform(sha256_context_t *context,const uint8_t block[64])
{
    static const uint32_t constants[64]={
        0x428a2f98u,0x71374491u,0xb5c0fbcfu,0xe9b5dba5u,0x3956c25bu,0x59f111f1u,
        0x923f82a4u,0xab1c5ed5u,0xd807aa98u,0x12835b01u,0x243185beu,0x550c7dc3u,
        0x72be5d74u,0x80deb1feu,0x9bdc06a7u,0xc19bf174u,0xe49b69c1u,0xefbe4786u,
        0x0fc19dc6u,0x240ca1ccu,0x2de92c6fu,0x4a7484aau,0x5cb0a9dcu,0x76f988dau,
        0x983e5152u,0xa831c66du,0xb00327c8u,0xbf597fc7u,0xc6e00bf3u,0xd5a79147u,
        0x06ca6351u,0x14292967u,0x27b70a85u,0x2e1b2138u,0x4d2c6dfcu,0x53380d13u,
        0x650a7354u,0x766a0abbu,0x81c2c92eu,0x92722c85u,0xa2bfe8a1u,0xa81a664bu,
        0xc24b8b70u,0xc76c51a3u,0xd192e819u,0xd6990624u,0xf40e3585u,0x106aa070u,
        0x19a4c116u,0x1e376c08u,0x2748774cu,0x34b0bcb5u,0x391c0cb3u,0x4ed8aa4au,
        0x5b9cca4fu,0x682e6ff3u,0x748f82eeu,0x78a5636fu,0x84c87814u,0x8cc70208u,
        0x90befffau,0xa4506cebu,0xbef9a3f7u,0xc67178f2u};
    uint32_t words[64];
    for(uint8_t i=0;i<16;++i)words[i]=((uint32_t)block[i*4]<<24)|
        ((uint32_t)block[i*4+1]<<16)|((uint32_t)block[i*4+2]<<8)|block[i*4+3];
    for(uint8_t i=16;i<64;++i){uint32_t a=words[i-15],b=words[i-2];
        uint32_t s0=rotate_right(a,7)^rotate_right(a,18)^(a>>3);
        uint32_t s1=rotate_right(b,17)^rotate_right(b,19)^(b>>10);
        words[i]=words[i-16]+s0+words[i-7]+s1;}
    uint32_t a=context->h[0],b=context->h[1],c=context->h[2],d=context->h[3];
    uint32_t e=context->h[4],f=context->h[5],g=context->h[6],h=context->h[7];
    for(uint8_t i=0;i<64;++i){uint32_t s1=rotate_right(e,6)^rotate_right(e,11)^
        rotate_right(e,25);uint32_t choice=(e&f)^((~e)&g);
        uint32_t t1=h+s1+choice+constants[i]+words[i];
        uint32_t s0=rotate_right(a,2)^rotate_right(a,13)^rotate_right(a,22);
        uint32_t majority=(a&b)^(a&c)^(b&c),t2=s0+majority;
        h=g;g=f;f=e;e=d+t1;d=c;c=b;b=a;a=t1+t2;}
    context->h[0]+=a;context->h[1]+=b;context->h[2]+=c;context->h[3]+=d;
    context->h[4]+=e;context->h[5]+=f;context->h[6]+=g;context->h[7]+=h;
}

static void sha256_update(sha256_context_t *context,const uint8_t *data,uint64_t size)
{
    context->length+=size;
    while(size){uint8_t available=(uint8_t)(64-context->used);
        uint8_t take=size<available?(uint8_t)size:available;
        for(uint8_t i=0;i<take;++i)context->block[context->used+i]=data[i];
        context->used=(uint8_t)(context->used+take);data+=take;size-=take;
        if(context->used==64){sha256_transform(context,context->block);context->used=0;}}
}

bool nova_integrity_sha256(const void *data,uint64_t size,uint8_t digest[32])
{
    if(!data||!size||!digest||size>UINT64_MAX/8u)return false;
    sha256_context_t context={{0x6a09e667u,0xbb67ae85u,0x3c6ef372u,0xa54ff53au,
        0x510e527fu,0x9b05688cu,0x1f83d9abu,0x5be0cd19u},0,{0},0};
    sha256_update(&context,data,size);uint64_t bits=context.length*8u;
    uint8_t padding[72]={0x80};uint8_t padding_size=context.used<56?
        (uint8_t)(56-context.used):(uint8_t)(120-context.used);
    sha256_update(&context,padding,padding_size);
    uint8_t encoded[8];for(uint8_t i=0;i<8;++i)encoded[7-i]=(uint8_t)(bits>>(i*8));
    sha256_update(&context,encoded,8);
    for(uint8_t i=0;i<8;++i)for(uint8_t j=0;j<4;++j)
        digest[i*4+j]=(uint8_t)(context.h[i]>>(24-j*8));
    return true;
}

uint32_t nova_integrity_crc32(const void *data,uint64_t size)
{
    if(!data||!size)return 0;
    const uint8_t *bytes=data;uint32_t crc=0xffffffffu;
    for(uint64_t i=0;i<size;++i){crc^=bytes[i];for(uint8_t bit=0;bit<8;++bit)
        crc=(crc>>1)^(0xedb88320u&(0u-(crc&1u)));}return ~crc;
}

bool nova_integrity_initialize(nova_integrity_policy_t policy)
{
    if(policy>NOVA_INTEGRITY_STRICT)return false;
    diagnostics=(nova_integrity_diagnostics_t){0};diagnostics.initialized=true;
    diagnostics.initializations=1;diagnostics.policy=policy;
    diagnostics.workspace_bytes=sizeof(sha256_context_t);return true;
}
bool nova_integrity_set_policy(nova_integrity_policy_t policy)
{if(!diagnostics.initialized||policy>NOVA_INTEGRITY_STRICT)return false;
 diagnostics.policy=policy;return true;}
nova_integrity_policy_t nova_integrity_policy(void){return diagnostics.policy;}

static bool equal_digest(const uint8_t *a,const uint8_t *b)
{uint8_t difference=0;for(uint8_t i=0;i<32;++i)difference|=(uint8_t)(a[i]^b[i]);
 return difference==0;}

nova_integrity_result_t nova_resource_verify(const nova_integrity_descriptor_t *descriptor,
                                             nova_integrity_report_t *report)
{
    nova_integrity_report_t local={0};if(!report)report=&local;*report=(nova_integrity_report_t){0};
    if(!diagnostics.initialized||!descriptor||!descriptor->resource_id||
       !descriptor->version||!descriptor->data||!descriptor->size||
       descriptor->size>UINTPTR_MAX-(uintptr_t)descriptor->data||
       descriptor->signature>NOVA_SIGNATURE_UNSUPPORTED){
        report->result=NOVA_INTEGRITY_INVALID_ARGUMENT;report->trust=NOVA_RESOURCE_TRUST_INVALID;
        diagnostics.last_error=report->result;return report->result;}
    ++diagnostics.verifications;diagnostics.last_resource_id=descriptor->resource_id;
    if(descriptor->signature==NOVA_SIGNATURE_INVALID){++diagnostics.invalid_signatures;
        report->result=NOVA_INTEGRITY_SIGNATURE_INVALID;report->trust=NOVA_RESOURCE_TRUST_INVALID;
        diagnostics.last_error=report->result;return report->result;}
    if(diagnostics.policy==NOVA_INTEGRITY_STRICT&&
       descriptor->signature!=NOVA_SIGNATURE_VERIFIED){++diagnostics.unsigned_rejections;
        report->result=NOVA_INTEGRITY_SIGNATURE_REQUIRED;report->trust=NOVA_RESOURCE_TRUST_INVALID;
        diagnostics.last_error=report->result;return report->result;}
    if(descriptor->signature==NOVA_SIGNATURE_UNSUPPORTED&&
       diagnostics.policy!=NOVA_INTEGRITY_PERMISSIVE){
        report->result=NOVA_INTEGRITY_UNSUPPORTED;report->trust=NOVA_RESOURCE_TRUST_INVALID;
        diagnostics.last_error=report->result;return report->result;}
    if(descriptor->expected_crc32){report->crc_checked=true;++diagnostics.crc32_checks;
        report->crc32=nova_integrity_crc32(descriptor->data,descriptor->size);
        diagnostics.bytes_hashed+=descriptor->size;
        if(report->crc32!=descriptor->expected_crc32){++diagnostics.corrupted;
            report->result=NOVA_INTEGRITY_CHECKSUM_MISMATCH;
            report->trust=NOVA_RESOURCE_TRUST_CORRUPTED;
            diagnostics.last_error=report->result;return report->result;}}
    if(descriptor->expected_sha256){report->sha256_checked=true;++diagnostics.sha256_checks;
        if(!nova_integrity_sha256(descriptor->data,descriptor->size,report->sha256)){
            report->result=NOVA_INTEGRITY_INVALID_ARGUMENT;report->trust=NOVA_RESOURCE_TRUST_INVALID;
            diagnostics.last_error=report->result;return report->result;}
        diagnostics.bytes_hashed+=descriptor->size;
        if(!equal_digest(report->sha256,descriptor->expected_sha256)){++diagnostics.corrupted;
            report->result=NOVA_INTEGRITY_CHECKSUM_MISMATCH;
            report->trust=NOVA_RESOURCE_TRUST_CORRUPTED;
            diagnostics.last_error=report->result;return report->result;}}
    if(diagnostics.policy==NOVA_INTEGRITY_STANDARD&&!descriptor->trusted_origin&&
       !report->crc_checked&&!report->sha256_checked){++diagnostics.unsigned_rejections;
        report->result=NOVA_INTEGRITY_CHECKSUM_MISMATCH;report->trust=NOVA_RESOURCE_TRUST_UNKNOWN;
        diagnostics.last_error=report->result;return report->result;}
    report->signature_valid=descriptor->signature==NOVA_SIGNATURE_VERIFIED;
    report->trust=report->signature_valid?NOVA_RESOURCE_TRUST_SIGNED:NOVA_RESOURCE_TRUST_VALID;
    report->result=NOVA_INTEGRITY_OK;++diagnostics.valid;diagnostics.last_error=report->result;
    return report->result;
}

void nova_integrity_record_cache_skip(void){if(diagnostics.initialized)++diagnostics.cache_skips;}
const nova_integrity_diagnostics_t *nova_integrity_diagnostics(void){return &diagnostics;}
