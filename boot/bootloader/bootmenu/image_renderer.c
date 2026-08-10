#include "image_renderer.h"
#include "clip_mask.h"
#include "memory.h"
#include "resources.h"

static nova_image_t images[NOVA_IMAGE_CAPACITY];
static uint8_t *pixel_pool;
static uint8_t *decode_buffer;
static uint32_t pixel_offset,next_generation;
static nova_image_diagnostics_t diagnostics;

static uint16_t le16(const uint8_t *p){return (uint16_t)(p[0]|(uint16_t)p[1]<<8);}
static uint32_t le32(const uint8_t *p)
{return p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
static bool dimensions_valid(uint32_t width,uint32_t height,uint64_t *bytes)
{
    if(!width||!height||width>4096||height>4096)return false;
    uint64_t result=(uint64_t)width*height*4u;
    if(result>NOVA_IMAGE_PIXEL_BUDGET)return false;
    if(bytes)*bytes=result;
    return true;
}
bool nova_image_initialize(void)
{
    for(uint8_t i=0;i<NOVA_IMAGE_CAPACITY;++i)images[i]=(nova_image_t){0};
    pixel_pool=nova_memory_allocate(NOVA_MEMORY_CACHE,NOVA_IMAGE_PIXEL_BUDGET,
                                    0x494d4147u,64);
    decode_buffer=nova_memory_allocate(NOVA_MEMORY_RENDER,240u*1024u,
                                       0x504e4744u,64);
    diagnostics=(nova_image_diagnostics_t){0};pixel_offset=0;next_generation=1;
    if(!pixel_pool||!decode_buffer)return false;
    diagnostics.initialized=true;diagnostics.initializations=1;
    return true;
}
bool nova_png_initialize(void){return diagnostics.initialized&&decode_buffer!=0;}
static nova_image_t *find(uint64_t id)
{for(uint8_t i=0;i<NOVA_IMAGE_CAPACITY;++i)if(images[i].used&&images[i].resource_id==id)return &images[i];return 0;}
static nova_image_t *allocate_image(uint64_t id,uint32_t width,uint32_t height)
{
    uint64_t bytes;if(!dimensions_valid(width,height,&bytes)||
       pixel_offset+bytes>NOVA_IMAGE_PIXEL_BUDGET)return 0;
    for(uint8_t i=0;i<NOVA_IMAGE_CAPACITY;++i)if(!images[i].used){
        images[i]=(nova_image_t){width,height,width,NOVA_IMAGE_RGBA8888,
            (uint32_t *)(pixel_pool+pixel_offset),id,1,next_generation++,
            (uint32_t)bytes,true,true};pixel_offset+=(uint32_t)bytes;
        diagnostics.memory_used+=bytes;if(bytes>diagnostics.largest_image)diagnostics.largest_image=bytes;
        return &images[i];}
    return 0;
}
static uint32_t premultiply(uint8_t r,uint8_t g,uint8_t b,uint8_t a)
{return (uint32_t)a<<24|(uint32_t)((r*a+127)/255)<<16|
    (uint32_t)((g*a+127)/255)<<8|(uint32_t)((b*a+127)/255);}
nova_image_result_t nova_image_create_raw(uint64_t id,uint32_t width,uint32_t height,
    uint32_t stride,nova_image_pixel_format_t format,const void *data,uint64_t size,
    nova_image_t **result)
{
    if(!diagnostics.initialized||!id||!data||!result||format>NOVA_IMAGE_RGB565){
        ++diagnostics.errors;return NOVA_IMAGE_INVALID;}
    nova_image_t *cached=find(id);if(cached){++cached->references;++diagnostics.cache_hits;
        *result=cached;return NOVA_IMAGE_OK;}
    uint32_t channels=format<=NOVA_IMAGE_BGRA8888?4:format==NOVA_IMAGE_RGB888?3:2;
    if(stride<(uint64_t)width*channels||(uint64_t)stride*height>size){
        ++diagnostics.errors;return NOVA_IMAGE_CORRUPT;}
    nova_image_t *image=allocate_image(id,width,height);if(!image){++diagnostics.errors;return NOVA_IMAGE_NO_MEMORY;}
    const uint8_t *source=data;
    for(uint32_t y=0;y<height;++y)for(uint32_t x=0;x<width;++x){
        const uint8_t *p=source+(uint64_t)y*stride+(uint64_t)x*channels;
        uint8_t r,g,b,a=255;
        if(format==NOVA_IMAGE_RGBA8888){r=p[0];g=p[1];b=p[2];a=p[3];}
        else if(format==NOVA_IMAGE_BGRA8888){b=p[0];g=p[1];r=p[2];a=p[3];}
        else if(format==NOVA_IMAGE_RGB888){r=p[0];g=p[1];b=p[2];}
        else {uint16_t value=le16(p);r=(uint8_t)(((value>>11)&31)*255/31);
            g=(uint8_t)(((value>>5)&63)*255/63);b=(uint8_t)((value&31)*255/31);}
        image->pixels[(uint64_t)y*width+x]=premultiply(r,g,b,a);
    }
    image->source_format=format;*result=image;++diagnostics.loaded;++diagnostics.raw_images;
    ++diagnostics.cache_misses;return NOVA_IMAGE_OK;
}
nova_image_result_t nova_image_create_2bit_alpha(uint64_t id,uint32_t width,uint32_t height,
    const uint8_t *data,uint64_t size,nova_image_t **result)
{
    uint64_t pixels=(uint64_t)width*height;if(!data||size<(pixels+3)/4||!result)return NOVA_IMAGE_INVALID;
    nova_image_t *cached=find(id);if(cached){++cached->references;++diagnostics.cache_hits;
        *result=cached;return NOVA_IMAGE_OK;}
    nova_image_t *image=allocate_image(id,width,height);if(!image)return NOVA_IMAGE_NO_MEMORY;
    for(uint64_t i=0;i<pixels;++i){uint8_t coverage=(data[i/4]>>((i&3)*2))&3;
        uint8_t alpha=(uint8_t)(coverage*85);image->pixels[i]=premultiply(255,255,255,alpha);}
    *result=image;++diagnostics.loaded;++diagnostics.raw_images;++diagnostics.cache_misses;
    return NOVA_IMAGE_OK;
}
static nova_image_result_t decode_bmp(uint64_t id,const uint8_t *data,uint64_t size,nova_image_t **result)
{
    if(size<54||data[0]!='B'||data[1]!='M')return NOVA_IMAGE_UNSUPPORTED;
    uint32_t offset=le32(data+10),dib=le32(data+14),width=le32(data+18);
    int32_t signed_height=(int32_t)le32(data+22);uint16_t planes=le16(data+26),bpp=le16(data+28);
    uint32_t compression=le32(data+30);if(dib<40||!width||!signed_height||planes!=1||
       (bpp!=24&&bpp!=32)||compression||signed_height==INT32_MIN)return NOVA_IMAGE_CORRUPT;
    uint32_t height=(uint32_t)(signed_height<0?-signed_height:signed_height);
    uint64_t row=((uint64_t)width*(bpp/8)+3)&~3ull;
    if(offset>size||row*height>size-offset)return NOVA_IMAGE_CORRUPT;
    nova_image_t *image=allocate_image(id,width,height);if(!image)return NOVA_IMAGE_NO_MEMORY;
    for(uint32_t y=0;y<height;++y){uint32_t sy=signed_height>0?height-1-y:y;
        for(uint32_t x=0;x<width;++x){const uint8_t *p=data+offset+(uint64_t)sy*row+x*(bpp/8);
            image->pixels[(uint64_t)y*width+x]=premultiply(p[2],p[1],p[0],bpp==32?p[3]:255);}}
    *result=image;++diagnostics.loaded;++diagnostics.bmp_decodes;++diagnostics.cache_misses;
    return NOVA_IMAGE_OK;
}

#define PNG_CHUNK_CAPACITY 16u
typedef struct {const uint8_t *data;uint32_t length;} png_chunk_t;
typedef struct {png_chunk_t chunks[PNG_CHUNK_CAPACITY];uint8_t count,index;
    uint32_t offset,remaining,bit_buffer;uint8_t bit_count;} deflate_reader_t;
typedef struct {uint16_t count[16],symbol[320];} huffman_t;

static uint32_t be32(const uint8_t *p)
{return (uint32_t)p[0]<<24|(uint32_t)p[1]<<16|(uint32_t)p[2]<<8|p[3];}
static uint32_t crc_update(uint32_t crc,const uint8_t *data,uint32_t size)
{
    for(uint32_t i=0;i<size;++i){crc^=data[i];for(uint8_t bit=0;bit<8;++bit)
        crc=(crc>>1)^(0xedb88320u&(0u-(crc&1u)));}return crc;
}
static bool reader_byte(deflate_reader_t *reader,uint8_t *value)
{
    if(!reader||!value||!reader->remaining)return false;
    while(reader->index<reader->count&&reader->offset==reader->chunks[reader->index].length){
        ++reader->index;reader->offset=0;}
    if(reader->index>=reader->count)return false;
    *value=reader->chunks[reader->index].data[reader->offset++];--reader->remaining;return true;
}
static bool chunk_byte_at(const png_chunk_t *chunks,uint8_t count,uint32_t position,
                          uint8_t *value)
{
    if(!chunks||!value)return false;
    for(uint8_t i=0;i<count;++i){
        if(position<chunks[i].length){*value=chunks[i].data[position];return true;}
        position-=chunks[i].length;
    }
    return false;
}
static bool reader_bits(deflate_reader_t *reader,uint8_t count,uint32_t *value)
{
    if(count>16||!value)return false;
    while(reader->bit_count<count){uint8_t byte;if(!reader_byte(reader,&byte))return false;
        reader->bit_buffer|=(uint32_t)byte<<reader->bit_count;reader->bit_count+=8;}
    *value=reader->bit_buffer&((1u<<count)-1u);reader->bit_buffer>>=count;
    reader->bit_count-=count;return true;
}
static void reader_align(deflate_reader_t *reader)
{reader->bit_buffer=0;reader->bit_count=0;}
static bool huffman_build(huffman_t *tree,const uint8_t *lengths,uint16_t symbols)
{
    uint16_t offsets[16]={0};*tree=(huffman_t){0};
    for(uint16_t i=0;i<symbols;++i){if(lengths[i]>15)return false;++tree->count[lengths[i]];}
    if(tree->count[0]==symbols)return false;
    int32_t left=1;for(uint8_t len=1;len<=15;++len){left=(left<<1)-tree->count[len];if(left<0)return false;}
    offsets[1]=0;for(uint8_t len=1;len<15;++len)offsets[len+1]=offsets[len]+tree->count[len];
    for(uint16_t symbol=0;symbol<symbols;++symbol)if(lengths[symbol])
        tree->symbol[offsets[lengths[symbol]]++]=symbol;
    return true;
}
static bool huffman_decode(deflate_reader_t *reader,const huffman_t *tree,uint16_t *symbol)
{
    uint32_t code=0,first=0,index=0;
    for(uint8_t length=1;length<=15;++length){uint32_t bit;
        if(!reader_bits(reader,1,&bit))return false;
        code|=bit;
        uint32_t count=tree->count[length];
        if(code-first<count){*symbol=tree->symbol[index+code-first];return true;}
        index+=count;first=(first+count)<<1;code<<=1;}
    return false;
}
static bool fixed_trees(huffman_t *literal,huffman_t *distance)
{
    uint8_t lengths[288],distances[32];
    for(uint16_t i=0;i<288;++i)lengths[i]=(uint8_t)(i<=143?8:i<=255?9:i<=279?7:8);
    for(uint8_t i=0;i<32;++i)distances[i]=5;
    return huffman_build(literal,lengths,288)&&huffman_build(distance,distances,32);
}
static __attribute__((noinline)) bool dynamic_trees(deflate_reader_t *reader,
                                                    huffman_t *literal,
                                                    huffman_t *distance)
{
    static const uint8_t order[19]={16,17,18,0,8,7,9,6,10,5,11,4,12,3,13,2,14,1,15};
    uint32_t value;if(!reader_bits(reader,5,&value))return false;uint16_t hlit=(uint16_t)value+257;
    if(!reader_bits(reader,5,&value))return false;
    uint16_t hdist=(uint16_t)value+1;
    if(!reader_bits(reader,4,&value))return false;
    uint16_t hclen=(uint16_t)value+4;
    if(hlit>286||hdist>32)return false;
    uint8_t code_lengths[19]={0},all[320]={0};
    for(uint16_t i=0;i<hclen;++i){if(!reader_bits(reader,3,&value))return false;
        code_lengths[order[i]]=(uint8_t)value;}
    huffman_t code_tree;if(!huffman_build(&code_tree,code_lengths,19))return false;
    uint16_t total=hlit+hdist,index=0;
    while(index<total){uint16_t symbol;if(!huffman_decode(reader,&code_tree,&symbol))return false;
        if(symbol<=15)all[index++]=(uint8_t)symbol;
        else {uint16_t repeat;uint8_t bits;uint8_t previous=0;
            if(symbol==16){if(!index)return false;bits=2;repeat=3;previous=all[index-1];}
            else if(symbol==17){bits=3;repeat=3;}else if(symbol==18){bits=7;repeat=11;}
            else return false;
            if(!reader_bits(reader,bits,&value))return false;
            repeat+=(uint16_t)value;
            if(index+repeat>total)return false;
            while(repeat--)all[index++]=previous;}}
    return all[256]&&huffman_build(literal,all,hlit)&&huffman_build(distance,all+hlit,hdist);
}
static bool inflate_codes(deflate_reader_t *reader,const huffman_t *literal,
                          const huffman_t *distance,uint8_t *output,uint32_t capacity,
                          uint32_t *written)
{
    static const uint16_t length_base[29]={3,4,5,6,7,8,9,10,11,13,15,17,19,23,27,
        31,35,43,51,59,67,83,99,115,131,163,195,227,258};
    static const uint8_t length_bits[29]={0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,3,3,
        3,3,4,4,4,4,5,5,5,5,0};
    static const uint16_t distance_base[30]={1,2,3,4,5,7,9,13,17,25,33,49,65,97,
        129,193,257,385,513,769,1025,1537,2049,3073,4097,6145,8193,12289,16385,24577};
    static const uint8_t distance_bits[30]={0,0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,
        8,8,9,9,10,10,11,11,12,12,13,13};
    for(;;){uint16_t symbol;if(!huffman_decode(reader,literal,&symbol))return false;
        if(symbol<256){if(*written>=capacity)return false;output[(*written)++]=(uint8_t)symbol;continue;}
        if(symbol==256)return true;
        if(symbol<257||symbol>285)return false;
        uint16_t length_index=(uint16_t)(symbol-257);uint32_t extra=0;
        if(length_bits[length_index]&&!reader_bits(reader,length_bits[length_index],&extra))return false;
        uint32_t length=length_base[length_index]+extra;uint16_t dist_symbol;
        if(!huffman_decode(reader,distance,&dist_symbol)||dist_symbol>=30)return false;
        extra=0;if(distance_bits[dist_symbol]&&!reader_bits(reader,distance_bits[dist_symbol],&extra))return false;
        uint32_t back=distance_base[dist_symbol]+extra;
        if(!back||back>*written||length>capacity-*written)return false;
        while(length--){output[*written]=output[*written-back];++*written;}}
}
static __attribute__((noinline)) bool inflate_zlib(png_chunk_t *chunks,uint8_t count,
                                                   uint32_t total,uint8_t *output,
                                                   uint32_t capacity,uint32_t *written)
{
    if(total<6)return false;
    deflate_reader_t reader={.count=count,.remaining=total};
    for(uint8_t i=0;i<count;++i)reader.chunks[i]=chunks[i];
    uint8_t adler[4];for(uint8_t i=0;i<4;++i)
        if(!chunk_byte_at(chunks,count,total-4u+i,&adler[i]))return false;
    uint8_t cmf,flg;if(!reader_byte(&reader,&cmf)||!reader_byte(&reader,&flg)||
       (cmf&15)!=8||((uint16_t)cmf*256+flg)%31||(flg&32))return false;
    reader.remaining=total-6u;*written=0;bool final=false;
    while(!final){uint32_t value,type;if(!reader_bits(&reader,1,&value))return false;final=value!=0;
        if(!reader_bits(&reader,2,&type)||type==3)return false;
        if(type==0){reader_align(&reader);uint8_t bytes[4];
            for(uint8_t i=0;i<4;++i)if(!reader_byte(&reader,&bytes[i]))return false;
            uint16_t length=le16(bytes),inverse=le16(bytes+2);
            if((uint16_t)~length!=inverse||length>capacity-*written)return false;
            while(length--)if(!reader_byte(&reader,&output[(*written)++]))return false;
        }else{huffman_t literal,distance;
            if(!(type==1?fixed_trees(&literal,&distance):dynamic_trees(&reader,&literal,&distance))||
               !inflate_codes(&reader,&literal,&distance,output,capacity,written))return false;}}
    uint32_t s1=1,s2=0;for(uint32_t i=0;i<*written;++i){s1=(s1+output[i])%65521u;s2=(s2+s1)%65521u;}
    reader_align(&reader);
    return ((s2<<16)|s1)==be32(adler)&&reader.remaining==0;
}
static uint8_t paeth(uint8_t a,uint8_t b,uint8_t c)
{int32_t p=(int32_t)a+b-c,pa=p-a,pb=p-b,pc=p-c;if(pa<0)pa=-pa;if(pb<0)pb=-pb;if(pc<0)pc=-pc;
 return pa<=pb&&pa<=pc?a:pb<=pc?b:c;}
static uint8_t png_sample(const uint8_t *row,uint32_t index,uint8_t depth)
{
    if(depth==8)return row[index];
    uint32_t bit=index*depth;uint8_t shift=(uint8_t)(8-depth-(bit&7u));
    return (uint8_t)((row[bit>>3]>>shift)&((1u<<depth)-1u));
}

static nova_image_result_t decode_png(uint64_t id,const uint8_t *data,uint64_t size,
                                      nova_image_t **result,bool validate_only)
{
    static const uint8_t signature[8]={0x89,'P','N','G',13,10,26,10};
    if(!diagnostics.initialized||!data||size<45||size>UINTPTR_MAX-(uintptr_t)data||
       (!validate_only&&(!id||!result)))return NOVA_IMAGE_INVALID;
    for(uint8_t i=0;i<8;++i)if(data[i]!=signature[i])return NOVA_IMAGE_UNSUPPORTED;
    uint32_t width=0,height=0,total_idat=0;uint8_t color_type=0,bit_depth=0;
    uint8_t palette[256][4];uint16_t palette_entries=0;for(uint16_t i=0;i<256;++i)
        palette[i][0]=palette[i][1]=palette[i][2]=0,palette[i][3]=255;
    uint16_t transparent_gray=0xffffu,transparent_red=0xffffu;
    uint16_t transparent_green=0xffffu,transparent_blue=0xffffu;
    png_chunk_t idat[PNG_CHUNK_CAPACITY];uint8_t idat_count=0;
    bool ihdr=false,iend=false,plte=false,trns=false,idat_ended=false;
    uint32_t chunk_count=0;
    uint64_t offset=8;while(offset+12<=size){uint32_t length=be32(data+offset);
        if(length>size-offset-12)return NOVA_IMAGE_CORRUPT;
        const uint8_t *type=data+offset+4,*chunk=data+offset+8;
        for(uint8_t i=0;i<4;++i)if(!((type[i]>='A'&&type[i]<='Z')||
           (type[i]>='a'&&type[i]<='z'))){++diagnostics.png_format_errors;
            return NOVA_IMAGE_CORRUPT;}
        if(type[2]>='a'&&type[2]<='z'){++diagnostics.png_format_errors;
            return NOVA_IMAGE_CORRUPT;}
        uint32_t expected=be32(chunk+length),crc=crc_update(0xffffffffu,type,4);
        crc=~crc_update(crc,chunk,length);if(crc!=expected){++diagnostics.png_crc_errors;
            return NOVA_IMAGE_CORRUPT;}
        ++chunk_count;++diagnostics.png_chunks;uint32_t name=be32(type);
        if(name!=0x49444154u&&idat_count)idat_ended=true;
        if(name==0x49484452u){if(ihdr||chunk_count!=1||length!=13){
                ++diagnostics.png_format_errors;return NOVA_IMAGE_CORRUPT;}ihdr=true;
            width=be32(chunk);height=be32(chunk+4);bit_depth=chunk[8];color_type=chunk[9];
            bool valid_depth=(color_type==0&&(bit_depth==1||bit_depth==2||bit_depth==4||bit_depth==8))||
                (color_type==2&&bit_depth==8)||(color_type==3&&(bit_depth==1||bit_depth==2||
                 bit_depth==4||bit_depth==8))||(color_type==4&&bit_depth==8)||
                (color_type==6&&bit_depth==8);
            if(!valid_depth||chunk[10]||chunk[11]||chunk[12])return NOVA_IMAGE_UNSUPPORTED;
        }else if(name==0x504c5445u){if(!ihdr||plte||idat_count||!length||length%3||
                length>768||color_type==0||color_type==4){++diagnostics.png_format_errors;
                return NOVA_IMAGE_CORRUPT;}plte=true;
            palette_entries=(uint16_t)(length/3);for(uint16_t i=0;i<palette_entries;++i){
                palette[i][0]=chunk[i*3];palette[i][1]=chunk[i*3+1];palette[i][2]=chunk[i*3+2];}
            if(color_type==3&&palette_entries>(1u<<bit_depth))return NOVA_IMAGE_CORRUPT;
        }else if(name==0x74524e53u){if(!ihdr||trns||idat_count||color_type==4||color_type==6){
                ++diagnostics.png_format_errors;return NOVA_IMAGE_CORRUPT;}trns=true;
            if(color_type==3){if(!plte||length>palette_entries)return NOVA_IMAGE_CORRUPT;
                for(uint16_t i=0;i<length;++i)palette[i][3]=chunk[i];}
            else if(color_type==0){if(length!=2)return NOVA_IMAGE_CORRUPT;
                transparent_gray=(uint16_t)((uint16_t)chunk[0]<<8|chunk[1]);}
            else if(color_type==2){if(length!=6)return NOVA_IMAGE_CORRUPT;
                transparent_red=(uint16_t)((uint16_t)chunk[0]<<8|chunk[1]);
                transparent_green=(uint16_t)((uint16_t)chunk[2]<<8|chunk[3]);
                transparent_blue=(uint16_t)((uint16_t)chunk[4]<<8|chunk[5]);}
            else return NOVA_IMAGE_CORRUPT;
        }else if(name==0x49444154u){if(!ihdr||idat_count==PNG_CHUNK_CAPACITY||
            idat_ended||UINT32_MAX-total_idat<length)return NOVA_IMAGE_CORRUPT;
            idat[idat_count++]=(png_chunk_t){chunk,length};total_idat+=length;
            ++diagnostics.png_idat_chunks;
        }else if(name==0x49454e44u){if(length||!idat_count||offset+12u+length!=size)
                return NOVA_IMAGE_CORRUPT;
            iend=true;offset+=12u+length;break;}
        else if(name==0x6163544cu)return NOVA_IMAGE_UNSUPPORTED;
        else if((type[0]&0x20u)==0)return NOVA_IMAGE_UNSUPPORTED;
        offset+=12u+length;}
    if(!ihdr||!iend||offset!=size||!idat_count||(color_type==3&&!palette_entries))
        return NOVA_IMAGE_CORRUPT;
    uint8_t channels=color_type==0||color_type==3?1:color_type==4?2:color_type==2?3:4;
    uint8_t bits_per_pixel=(uint8_t)(channels*bit_depth);
    uint8_t filter_bytes=(uint8_t)((bits_per_pixel+7u)/8u);
    uint64_t row=((uint64_t)width*bits_per_pixel+7u)/8u;
    uint64_t raw=(row+1)*height;if(raw>240u*1024u||!dimensions_valid(width,height,0))return NOVA_IMAGE_NO_MEMORY;
    uint32_t written;if(!inflate_zlib(idat,idat_count,total_idat,decode_buffer,(uint32_t)raw,&written)||written!=raw)
        return NOVA_IMAGE_CORRUPT;
    for(uint32_t y=0;y<height;++y){uint8_t *scan=decode_buffer+(uint64_t)y*(row+1);
        uint8_t filter=scan[0];if(filter>4)return NOVA_IMAGE_CORRUPT;
        ++diagnostics.png_filter_rows;
        for(uint32_t x=0;x<row;++x){uint8_t left=x>=filter_bytes?scan[1+x-filter_bytes]:0;
            uint8_t above=y?decode_buffer[(uint64_t)(y-1)*(row+1)+1+x]:0;
            uint8_t upper_left=y&&x>=filter_bytes?
                decode_buffer[(uint64_t)(y-1)*(row+1)+1+x-filter_bytes]:0;
            uint8_t value=scan[1+x];if(filter==1)value+=left;else if(filter==2)value+=above;
            else if(filter==3)value+=(uint8_t)(((uint16_t)left+above)/2);else if(filter==4)value+=paeth(left,above,upper_left);
            scan[1+x]=value;}}
    if(color_type==3)for(uint32_t y=0;y<height;++y){const uint8_t *scan=
            decode_buffer+(uint64_t)y*(row+1)+1;
        for(uint32_t x=0;x<width;++x)
            if(png_sample(scan,x,bit_depth)>=palette_entries)return NOVA_IMAGE_CORRUPT;}
    nova_image_t *image=validate_only?0:allocate_image(id,width,height);
    if(!validate_only&&!image)return NOVA_IMAGE_NO_MEMORY;
    for(uint32_t y=0;y<height;++y){uint8_t *scan=decode_buffer+(uint64_t)y*(row+1)+1;
        for(uint32_t x=0;x<width;++x){uint8_t r,g,b,a=255;
            if(color_type==3){uint8_t index=png_sample(scan,x,bit_depth);
                r=palette[index][0];g=palette[index][1];b=palette[index][2];a=palette[index][3];}
            else if(color_type==0){uint8_t sample=png_sample(scan,x,bit_depth);
                uint8_t maximum=(uint8_t)((1u<<bit_depth)-1u);
                r=g=b=(uint8_t)((uint16_t)sample*255u/maximum);
                if(trns&&sample==transparent_gray)a=0;}
            else if(color_type==4){r=g=b=scan[x*2];a=scan[x*2+1];}
            else {r=scan[x*channels];g=scan[x*channels+1];b=scan[x*channels+2];
                if(color_type==6)a=scan[x*4+3];
                else if(trns&&r==transparent_red&&g==transparent_green&&b==transparent_blue)a=0;}
            if(image)image->pixels[(uint64_t)y*width+x]=premultiply(r,g,b,a);}}
    diagnostics.last_png_width=width;diagnostics.last_png_height=height;
    diagnostics.last_png_color_type=color_type;diagnostics.last_png_bit_depth=bit_depth;
    if(validate_only){++diagnostics.png_validations;return NOVA_IMAGE_OK;}
    *result=image;++diagnostics.loaded;++diagnostics.png_decodes;++diagnostics.cache_misses;
    return NOVA_IMAGE_OK;
}

nova_image_result_t nova_png_decode(uint64_t id,const void *source,uint64_t source_size,
                                    nova_image_t **result)
{return decode_png(id,source,source_size,result,false);}
bool nova_png_validate(const void *source,uint64_t source_size)
{return decode_png(1,source,source_size,0,true)==NOVA_IMAGE_OK;}
nova_image_result_t nova_image_load(const char *uri,nova_image_t **result)
{
    if(!uri||!result)return NOVA_IMAGE_INVALID;
    uint64_t id=nova_resource_id(uri);
    nova_image_t *cached=find(id);if(cached){++cached->references;++diagnostics.cache_hits;
        *result=cached;return NOVA_IMAGE_OK;}
    const nova_resource_t *resource=nova_resource_load(id);
    if(!resource||resource->type!=NOVA_RESOURCE_IMAGE){++diagnostics.errors;return NOVA_IMAGE_NOT_FOUND;}
    const uint8_t *data=resource->data;
    if(resource->size>=8&&data[0]==0x89&&data[1]=='P'&&data[2]=='N'&&data[3]=='G'){
        nova_image_result_t status=decode_png(id,data,resource->size,result,false);
        if(status!=NOVA_IMAGE_OK)++diagnostics.errors;
        return status;
    }
    nova_image_result_t status=decode_bmp(id,data,resource->size,result);
    if(status!=NOVA_IMAGE_OK)++diagnostics.errors;
    return status;
}
bool nova_image_destroy(nova_image_t *image)
{
    if(!image||!image->used||!image->references)return false;
    --image->references;++diagnostics.destroys;return true;
}
bool nova_image_get_size(const nova_image_t *image,uint32_t *width,uint32_t *height)
{if(!image||!image->used||!width||!height)return false;*width=image->width;*height=image->height;return true;}
static uint32_t sample_nearest(const nova_image_t *image,int32_t x_16,int32_t y_16)
{
    int32_t x=x_16>>16,y=y_16>>16;if(x<0)x=0;if(y<0)y=0;
    if((uint32_t)x>=image->width)x=(int32_t)image->width-1;
    if((uint32_t)y>=image->height)y=(int32_t)image->height-1;
    return image->pixels[(uint64_t)y*image->stride+x];
}
static uint32_t sample_bilinear(const nova_image_t *image,int32_t x,int32_t y)
{
    int32_t x0=x>>16,y0=y>>16;uint32_t fx=(uint32_t)x&0xffffu,fy=(uint32_t)y&0xffffu;
    if(x0<0){x0=0;fx=0;}if(y0<0){y0=0;fy=0;}
    int32_t x1=x0+1,y1=y0+1;if((uint32_t)x1>=image->width)x1=(int32_t)image->width-1;
    if((uint32_t)y1>=image->height)y1=(int32_t)image->height-1;
    if((uint32_t)x0>=image->width)x0=x1;
    if((uint32_t)y0>=image->height)y0=y1;
    uint32_t p[4]={image->pixels[(uint64_t)y0*image->stride+x0],
        image->pixels[(uint64_t)y0*image->stride+x1],image->pixels[(uint64_t)y1*image->stride+x0],
        image->pixels[(uint64_t)y1*image->stride+x1]};
    uint64_t wx0=65536u-fx,wy0=65536u-fy,w[4]={wx0*wy0,(uint64_t)fx*wy0,wx0*fy,(uint64_t)fx*fy};
    uint32_t out=0;for(uint8_t shift=0;shift<32;shift+=8){uint64_t sum=0;
        for(uint8_t i=0;i<4;++i)sum+=((p[i]>>shift)&255u)*w[i];
        out|=(uint32_t)((sum+(1ull<<31))>>32)<<shift;}return out;
}
static uint32_t tint(uint32_t pixel,uint32_t color,uint16_t opacity,uint8_t mask)
{
    uint32_t ca=color>>24;uint64_t factor=(uint64_t)ca*opacity*mask;
    if(opacity==1000&&mask==255&&ca==255){
        if((color&0x00ffffffu)==0x00ffffffu)return pixel;
        uint32_t alpha=pixel&0xff000000u;
        uint32_t r=(((pixel>>16)&255u)*((color>>16)&255u)+127u)/255u;
        uint32_t g=(((pixel>>8)&255u)*((color>>8)&255u)+127u)/255u;
        uint32_t b=((pixel&255u)*(color&255u)+127u)/255u;
        return alpha|r<<16|g<<8|b;
    }
    uint64_t divisor=255u*1000u*255u;
    uint32_t alpha=(uint32_t)(((pixel>>24)*factor+divisor/2)/divisor);
    uint32_t r=(uint32_t)((((pixel>>16)&255u)*((color>>16)&255u)*factor+
        divisor*255u/2)/(divisor*255u));
    uint32_t g=(uint32_t)((((pixel>>8)&255u)*((color>>8)&255u)*factor+
        divisor*255u/2)/(divisor*255u));
    uint32_t b=(uint32_t)(((pixel&255u)*(color&255u)*factor+
        divisor*255u/2)/(divisor*255u));return alpha<<24|r<<16|g<<8|b;
}
static uint32_t source_over(uint32_t destination,uint32_t premultiplied_source)
{
    uint32_t sa=premultiplied_source>>24,da=destination>>24,inverse=255u-sa;
    if(sa==255)return premultiplied_source;
    if(da==255){uint32_t result=0xff000000u;
        for(uint8_t shift=0;shift<24;shift+=8){
            uint32_t source=(premultiplied_source>>shift)&255u;
            uint32_t dest=(destination>>shift)&255u;
            result|=(source+(dest*inverse+127u)/255u)<<shift;
        }
        return result;
    }
    uint32_t out_a=sa+(da*inverse+127)/255u;if(!out_a)return 0;
    uint32_t result=out_a<<24;
    for(uint8_t shift=0;shift<24;shift+=8){uint32_t source=(premultiplied_source>>shift)&255u;
        uint32_t dest=(destination>>shift)&255u;
        uint32_t premult=source+(dest*da*inverse+32512u)/65025u;
        uint32_t straight=(premult*255u+out_a/2)/out_a;if(straight>255)straight=255;
        result|=straight<<shift;}return result;
}
nova_image_result_t nova_image_render(const nova_image_t *image,nova_surface_t *surface,
                                      const nova_image_render_options_t *o)
{
    if(!image||!image->used||!surface||!surface->pixels||!o||o->opacity>1000||
       o->sampling>NOVA_IMAGE_SAMPLE_BILINEAR||o->destination.width<=0||
       o->destination.height<=0||o->clip.width<=0||o->clip.height<=0||
       !nova_transform_fixed_valid(o->transform)){++diagnostics.errors;return NOVA_IMAGE_INVALID;}
    nova_fixed_transform2d_t inverse; nova_rect_t transformed;
    if(!nova_transform_fixed_inverse(&inverse,o->transform)||
       !nova_transform_fixed_bounds(o->transform,o->destination,&transformed))return NOVA_IMAGE_INVALID;
    int32_t left=transformed.x>o->clip.x?transformed.x:o->clip.x;
    int32_t top=transformed.y>o->clip.y?transformed.y:o->clip.y;
    int32_t right=transformed.x+transformed.width,clip_right=o->clip.x+o->clip.width;
    int32_t bottom=transformed.y+transformed.height,clip_bottom=o->clip.y+o->clip.height;
    if(right>clip_right)right=clip_right;
    if(bottom>clip_bottom)bottom=clip_bottom;
    if(left<0)left=0;
    if(top<0)top=0;
    if(right>(int32_t)surface->width)right=(int32_t)surface->width;
    if(bottom>(int32_t)surface->height)bottom=(int32_t)surface->height;
    bool direct=o->transform.m11==65536&&o->transform.m12==0&&
        o->transform.m21==0&&o->transform.m22==65536&&o->transform.tx==0&&
        o->transform.ty==0;
    if(direct){
        int32_t sx_step=(int32_t)(((int64_t)image->width*65536)/o->destination.width);
        int32_t sy_step=(int32_t)(((int64_t)image->height*65536)/o->destination.height);
        int32_t sy=(int32_t)(((int64_t)(top-o->destination.y)*65536+32768)*
                             image->height/o->destination.height);
        for(int32_t y=top;y<bottom;++y,sy+=sy_step){
            int32_t sx=(int32_t)(((int64_t)(left-o->destination.x)*65536+32768)*
                                 image->width/o->destination.width);
            for(int32_t x=left;x<right;++x,sx+=sx_step){
                int32_t sample_x=o->mirror_x?
                    (int32_t)((int64_t)image->width*65536-1-sx):sx;
                int32_t sample_y=o->mirror_y?
                    (int32_t)((int64_t)image->height*65536-1-sy):sy;
                uint32_t pixel=o->sampling==NOVA_IMAGE_SAMPLE_BILINEAR?
                    sample_bilinear(image,sample_x,sample_y):
                    sample_nearest(image,sample_x,sample_y);
                uint8_t mask=nova_mask_coverage(o->mask_scope,x,y);if(!mask)continue;
                uint32_t *destination=&surface->pixels[(uint64_t)y*surface->stride+x];
                *destination=source_over(*destination,tint(pixel,o->tint,o->opacity,mask));
                ++diagnostics.pixels_rendered;
            }
        }
        nova_damage_add(surface,(nova_rect_t){left,top,right>left?right-left:0,
                                              bottom>top?bottom-top:0});
        ++diagnostics.renders;return NOVA_IMAGE_OK;
    }
    for(int32_t y=top;y<bottom;++y)for(int32_t x=left;x<right;++x){
        nova_fixed_point2d_t local={x*65536+32768,y*65536+32768};
        if(!nova_transform_fixed_point(inverse,local,&local))continue;
        int64_t lx=(int64_t)local.x-(int64_t)o->destination.x*65536;
        int64_t ly=(int64_t)local.y-(int64_t)o->destination.y*65536;
        if(lx<0||ly<0||lx>=(int64_t)o->destination.width*65536||
           ly>=(int64_t)o->destination.height*65536)continue;
        int32_t sx=(int32_t)(lx*image->width/o->destination.width);
        int32_t sy=(int32_t)(ly*image->height/o->destination.height);
        if(o->mirror_x)sx=(int32_t)((int64_t)image->width*65536-1-sx);
        if(o->mirror_y)sy=(int32_t)((int64_t)image->height*65536-1-sy);
        uint32_t pixel=o->sampling==NOVA_IMAGE_SAMPLE_BILINEAR?
            sample_bilinear(image,sx,sy):sample_nearest(image,sx,sy);
        uint8_t mask=nova_mask_coverage(o->mask_scope,x,y);if(!mask)continue;
        uint32_t *destination=&surface->pixels[(uint64_t)y*surface->stride+x];
        *destination=source_over(*destination,tint(pixel,o->tint,o->opacity,mask));
        ++diagnostics.pixels_rendered;}
    nova_damage_add(surface,(nova_rect_t){left,top,right>left?right-left:0,bottom>top?bottom-top:0});
    ++diagnostics.renders;return NOVA_IMAGE_OK;
}
const nova_image_diagnostics_t *nova_image_diagnostics(void){return &diagnostics;}
