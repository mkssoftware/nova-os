#include "compression.h"

static nova_compression_diagnostics_t diagnostics;

bool nova_compression_initialize(void)
{
    diagnostics=(nova_compression_diagnostics_t){0};diagnostics.initialized=true;
    diagnostics.initializations=1;return true;
}
bool nova_compression_supported(nova_compression_type_t type)
{return type==NOVA_COMPRESSION_NONE||type==NOVA_COMPRESSION_LZ4;}

static bool extended_length(const uint8_t **source,const uint8_t *end,
                            uint64_t *length)
{
    uint8_t value=255;
    while(value==255){if(*source>=end)return false;value=*(*source)++;
        if(UINT64_MAX-*length<value)return false;
        *length+=value;}
    return true;
}

nova_compression_result_t nova_resource_decompress(nova_compression_type_t type,
    const void *source,uint64_t source_size,void *destination,
    uint64_t destination_size,uint64_t *written)
{
    if(written)*written=0;
    if(!diagnostics.initialized||!source||!destination||!source_size||
       !destination_size||type>=NOVA_COMPRESSION_COUNT){
        ++diagnostics.size_errors;return NOVA_COMPRESSION_INVALID_ARGUMENT;}
    if(source_size>UINTPTR_MAX-(uintptr_t)source||
       destination_size>UINTPTR_MAX-(uintptr_t)destination){
        ++diagnostics.size_errors;return NOVA_COMPRESSION_INVALID_ARGUMENT;}
    ++diagnostics.calls;diagnostics.last_type=type;
    diagnostics.compressed_bytes+=source_size;diagnostics.original_bytes+=destination_size;
    diagnostics.last_ratio_permille=(uint32_t)(source_size<=UINT64_MAX/1000u?
        source_size*1000u/destination_size:UINT32_MAX);
    if(type==NOVA_COMPRESSION_ZSTD){++diagnostics.zstd_rejections;
        return NOVA_COMPRESSION_UNSUPPORTED;}
    const uint8_t *input=source;uint8_t *output=destination;
    if(type==NOVA_COMPRESSION_NONE){++diagnostics.none_calls;
        if(source_size!=destination_size){++diagnostics.size_errors;
            return NOVA_COMPRESSION_SIZE_MISMATCH;}
        if(input!=output)for(uint64_t i=0;i<source_size;++i)output[i]=input[i];
        if(written)*written=source_size;
        diagnostics.written_bytes+=source_size;
        return NOVA_COMPRESSION_OK;}
    ++diagnostics.lz4_calls;const uint8_t *source_end=input+source_size;
    uint8_t *destination_end=output+destination_size,*start=output;
    while(input<source_end){uint8_t token=*input++;
        uint64_t literal_length=token>>4;
        if(literal_length==15&&!extended_length(&input,source_end,&literal_length)){
            ++diagnostics.corrupt_streams;return NOVA_COMPRESSION_SOURCE_TRUNCATED;}
        if(literal_length>(uint64_t)(source_end-input)){
            ++diagnostics.corrupt_streams;return NOVA_COMPRESSION_SOURCE_TRUNCATED;}
        if(literal_length>(uint64_t)(destination_end-output)){
            ++diagnostics.size_errors;return NOVA_COMPRESSION_DESTINATION_TOO_SMALL;}
        while(literal_length--)*output++=*input++;
        if(input==source_end)break;
        if(source_end-input<2){++diagnostics.corrupt_streams;
            return NOVA_COMPRESSION_SOURCE_TRUNCATED;}
        uint16_t offset=(uint16_t)(input[0]|(uint16_t)input[1]<<8);input+=2;
        if(!offset||offset>(uint64_t)(output-start)){++diagnostics.corrupt_streams;
            return NOVA_COMPRESSION_CORRUPT;}
        uint64_t match_length=(token&15u)+4u;
        if((token&15u)==15&&!extended_length(&input,source_end,&match_length)){
            ++diagnostics.corrupt_streams;return NOVA_COMPRESSION_SOURCE_TRUNCATED;}
        if(match_length>(uint64_t)(destination_end-output)){
            ++diagnostics.size_errors;return NOVA_COMPRESSION_DESTINATION_TOO_SMALL;}
        while(match_length--){*output=output[-(int32_t)offset];++output;}
    }
    uint64_t produced=(uint64_t)(output-start);
    if(produced!=destination_size){++diagnostics.size_errors;
        return NOVA_COMPRESSION_SIZE_MISMATCH;}
    if(written)*written=produced;
    diagnostics.written_bytes+=produced;
    return NOVA_COMPRESSION_OK;
}

const nova_compression_diagnostics_t *nova_compression_diagnostics(void)
{return &diagnostics;}
