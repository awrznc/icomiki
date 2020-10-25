#include <stdio.h>
#include <stdlib.h>

#define ICOMIKI_COLOR_MIN_VALUE 0
#define ICOMIKI_COLOR_MAX_VALUE 255

#define ICOMIKI_TRUE 1
#define ICOMIKI_FALSE 0


FILE* load_file(const char* fileName) {
    FILE* file_pointer = fopen( fileName, "rb" );
    if( file_pointer == NULL ){
        fputs( "file open error.\n", stderr );
        exit( EXIT_FAILURE );
    }
    return file_pointer;
}

int free_file(FILE* file_pointer) {
    if( fclose( file_pointer ) == EOF ){
        fputs( "file close error.\n", stderr );
        return ICOMIKI_FALSE;
    }
    return ICOMIKI_TRUE;
}

/// Icomiki_PrintColor
/// ```
/// int main() {
///     Icomiki_PrintColor(
///         ICOMIKI_COLOR_MAX_VALUE,    // Red
///         ICOMIKI_COLOR_MIN_VALUE,    // Green
///         ICOMIKI_COLOR_MIN_VALUE     // Blue
///     );
///     // => print red pixel
///
///     return 0;
/// }
/// ```
int Icomiki_PrintColor(unsigned char red, unsigned char green, unsigned char blue) {
    printf("\e[48;2;%d;%d;%dm  \e[m", red, green, blue);
    return 0;
}

struct ImageHeader {
    unsigned char length[4];
    unsigned char chunk_type[4];
    unsigned char width[4];
    unsigned char height[4];
    unsigned char depth[1];
    unsigned char type[1];
    unsigned char compression[1];
    unsigned char filter[1];
    unsigned char interrace[1];
    unsigned char crc[4];
} typedef ImageHeader;

struct PNG {
    unsigned long width;
    unsigned long height;
};

typedef struct PNG PNG, *PNG_Object;

PNG_Object PNG_Initialize() {
    PNG_Object object;
    object = (PNG_Object)calloc(sizeof(PNG_Object), 1);
    return object;
}

int PNG_ByteToInt32(unsigned char* byte, int from, int to) {
    int result=0;
    int char_bit_size = 8;
    for(int index=to, count=0; index>=from; index--, count++){
        result += (unsigned char)*(byte+index) << (count*char_bit_size);
    }
    return result;
}


//  from -> to
// +-----------+
// | 0101 0101 |
// +-----------+
unsigned char read_bit(unsigned char byte, unsigned char from, unsigned char to) {
    unsigned char bit = 0x80;
    unsigned char bit_size = 8;
    unsigned char from_mask = (bit >> from);
    unsigned char to_mask = (bit >> to);
    unsigned char result = 0;

    for (unsigned char mask = from_mask; to_mask <= mask; mask >>= 1) {
        result += byte & mask ? (mask >> bit_size-(to+1)) : 0;
    }
    return result;
}




struct Palette {
    unsigned char length[4];
    unsigned char chunk_type[4];
    unsigned char *chunk_data;
    unsigned char crc[4];
} typedef Palette;

struct Metadata {
    unsigned char length[4];
    unsigned char chunk_type[4];
    unsigned char *chunk_data;
    unsigned char crc[4];
} typedef Metadata;

struct ImageData {
    unsigned char length[4];
    unsigned char chunk_type[4];
    unsigned char *chunk_data;
    unsigned char crc[4];
} typedef ImageData;

struct ImageEnd {
    unsigned char length[4];
    unsigned char chunk_type[4];
    unsigned char crc[4];
} typedef ImageEnd;

struct Icomiki {
    unsigned char signature[8];
    ImageHeader image_header;
    Palette palette;
    Metadata meta_data;
    ImageData image_data;
    ImageEnd image_end;
} typedef Icomiki;

struct FileBit {
    unsigned char* file;
    unsigned int length;
    unsigned char current_byte_index;
    unsigned char current_bit_index;
} typedef FileBit;

// bit stream
unsigned int bfread(FileBit* fbit, int size){
    // printf("\nfbit.length: %d\n", fbit->length);
    // printf("fbit.byte_idx : %d\n", fbit->current_byte_index);
    // printf("fbit.bit_idx : %d\n", fbit->current_bit_index);
    // printf("current : %d\n", fbit->current_byte_index);

    // unsigned char byte_size = 8;
    // // unsigned char add_byte_size = size / byte_size;
    // // unsigned char add_bit_size = (size - (byte_size-fbit->current_bit_index)) % byte_size;
    // unsigned char prefix = (byte_size - fbit->current_bit_index);
    // unsigned char add_byte_size = byte_size-fbit->current_bit_index == 7 ? (size - prefix) / byte_size - 1 : (size - prefix) / byte_size;
    // unsigned char add_bit_size = (size - prefix) % byte_size;
    // unsigned char last_index = add_byte_size + fbit->current_byte_index;
    // printf("prefix:%d, add_byte_size:%d, add_bit_size:%d, last_index:%d\n", prefix, add_byte_size, add_bit_size, last_index);

    // // 4 -> 0, 4
    // // 8 -> 0, 8
    // // 10 -> 1, 2

    // // A: 11       : current_bit_indexが0以外だったら+1
    // // B: 11110000 : (size - (byte_size - current_bit_index - 1)) / byte_size + A
    // // C:       11 : (size - byte_size - current_bit_index - 1) % byte_size

    // // D:   1111

    // unsigned int result = 0;
    // unsigned char count = 0;
    // for (;fbit->current_byte_index <= last_index;fbit->current_byte_index++) {
    //     result += read_bit(fbit->file[fbit->current_byte_index], 0, byte_size - 1 - fbit->current_bit_index) << count;
    //     for (;fbit->current_bit_index < byte_size;fbit->current_bit_index++, count++) {
    //         printf("count: %d\n", count);
    //     }
    //     fbit->current_bit_index = 0;
    // }

    // printf("from: %d, to: %d\n", byte_size - add_bit_size-1, byte_size-1);
    // printf("read: %d\n", read_bit(fbit->file[fbit->current_byte_index], byte_size - add_bit_size, byte_size-1) << count);
    // // result += read_bit(fbit->file[fbit->current_byte_index], byte_size - add_bit_size, byte_size-1) << count;

    // // for (;fbit->current_bit_index < add_bit_size;fbit->current_bit_index++) {
    // //     // printf("%d, ",fbit->current_bit_index);
    // // }

    // // printf("result: %d, conut: %d\n", result, count);

    unsigned int result=0;
    unsigned char byte_size = 8;
    unsigned int max = 1 << (size-1);

    // for (unsigned char mask = 0x80; mask; mask >>= 1) putchar(fbit->file[fbit->current_byte_index] & mask ? '1' : '0'); printf(" \n");

    for (int mask=1<<fbit->current_bit_index,count=0; count < size; mask<<=1,fbit->current_bit_index++,count++) {
        if(fbit->current_bit_index > byte_size-1) {
            fbit->current_byte_index++;
            fbit->current_bit_index = 0;
            mask=1;
        }
        // printf("%d, ", mask);
        // result += fbit->file[fbit->current_byte_index] & mask ? (mask >> byte_size-1-fbit->current_bit_index) : 0;
        result += fbit->file[fbit->current_byte_index] & mask ? 1 << count : 0;
        // printf("fbit->current_bit_index:%d, mask:%d, &:%d\n", fbit->current_bit_index, mask,fbit->file[fbit->current_byte_index] & mask ? 1 << count : 0);
        // putchar(fbit->file[fbit->current_byte_index] & mask ? '1' : '0');
    }
    // printf("\nmax:%d, size:%d\n", max,size);
    // printf("fbit->current_byte_index:%d\n", fbit->current_byte_index);
    // printf("result: %d\n", result);

    return result;
}

int IcomikiDebug_PrintInfo(Icomiki* icomiki) {

    printf("\n=== PNG File Signature ===\n");
    printf("signature:  "); for(int i=0; i<sizeof(icomiki->signature);i++) printf("%02x ", icomiki->signature[i]); printf("\n");

    printf("\n=== Image Header (IHDR) ===\n");
    printf("length:      %d\n", PNG_ByteToInt32(icomiki->image_header.length, 0, 3));
    printf("chunk_type:  "); for(int i=0; i<sizeof(icomiki->image_header.chunk_type);i++) printf("%02x ", icomiki->image_header.chunk_type[i]); printf("\n");
    printf("width:       %d\n", PNG_ByteToInt32(icomiki->image_header.width, 0, 3));
    printf("height:      %d\n", PNG_ByteToInt32(icomiki->image_header.height, 0, 3));
    printf("depth:       %d\n", PNG_ByteToInt32(icomiki->image_header.depth, 0, 0));
    printf("type:        %d\n", PNG_ByteToInt32(icomiki->image_header.type, 0, 0));
    printf("compression: %d\n", PNG_ByteToInt32(icomiki->image_header.compression, 0, 0));
    printf("filter:      %d\n", PNG_ByteToInt32(icomiki->image_header.filter, 0, 0));
    printf("interrace:   %d\n", PNG_ByteToInt32(icomiki->image_header.interrace, 0, 0));
    printf("crc:         "); for(int i=0; i<sizeof(icomiki->image_header.crc);i++) printf("%02x ", icomiki->image_header.crc[i]); printf("\n");

    printf("\n=== Image data (IDAT) ===\n");
    printf("length:      %d byte\n", PNG_ByteToInt32(icomiki->image_data.length, 0, 3));

    //    +--- CMF: 1byte ---+    +--- FLG: 1byte -----------------------+
    // +-------------+----------+--------------+-------------+--------------+-----------------+-------------+-----------------+
    // | CINFO: 4bit | CM: 4bit | FLEVEL: 2bit | FDICT: 1bit | FCHECK: 5bit | (DICTID: 4byte) | Data: Nbyte | Adler-32: 4byte |
    // +-------------+----------+--------------+-------------+--------------+-----------------+-------------+-----------------+
    printf("CMF:         ");
    for (unsigned char mask = 0x80; mask; mask >>= 1) putchar(icomiki->image_data.chunk_data[0] & mask ? '1' : '0');
    printf(" \nFLG:         ");
    for (unsigned char mask = 0x80; mask; mask >>= 1) putchar(icomiki->image_data.chunk_data[1] & mask ? '1' : '0');
    printf(" \n");

    // cinfo: 3
    // cm: 8
    // flevel: 3
    // dict: 0
    // fcheck: 11
    printf("CMF/FLG check: %d (0ならOK)\n", PNG_ByteToInt32(icomiki->image_data.chunk_data, 0, 1)%31);
    // +--------------+-------------+------------+-------------+-------------+
    // | BFINAL: 1bit | BTYPE: 2bit | HLIT: 5bit | HDIST: 5bit | HCLEN: 4bit |
    // +--------------+-------------+------------+-------------+-------------+
    printf("header: %d\n", PNG_ByteToInt32(icomiki->image_data.chunk_data, 2, 2));
    printf("BFINAL: %d\n", read_bit(icomiki->image_data.chunk_data[2], 7, 7) );
    printf("BTYPE: %d\n", read_bit(icomiki->image_data.chunk_data[2], 5, 6) );
    printf("HLIT: %d\n", read_bit(icomiki->image_data.chunk_data[2], 0, 4) );
    printf("HDIST: %d\n", read_bit(icomiki->image_data.chunk_data[3], 2, 7) );

    // 1111 = 15 -> 15+4 = 19
    unsigned char hclen = read_bit(icomiki->image_data.chunk_data[3], 0, 1);
    printf("HCLEN: %d\n", hclen);
    // printf("HCLEN: %d\n", read_bit(icomiki->image_data.chunk_data[4], 6, 7) );

    unsigned char lllllleeeennnn=hclen+4;

    FileBit bit = { icomiki->image_data.chunk_data, sizeof(icomiki->image_data.chunk_data), 3, 6};
    // for (int index=0; index<3; index++) {printf("BIT: %d\n", bfread(&bit, 3) );}
    // printf("BIT: %d, %d\n", bfread(&bit, 10), bit.current_byte_index );
    // printf("BIT: %d, %d\n", bfread(&bit, 8), bit.current_byte_index );
    for(int index=0;index<lllllleeeennnn;index++) {
        printf("BIT: %d\n", bfread(&bit, 3));
    }
    // printf("BIT: %d, %d\n", bfread(&bit, 3), bit.current_byte_index );
    // printf("BIT: %d, %d\n", bfread(&bit, 3), bit.current_byte_index );
    // printf("BIT: %d, %d\n", bfread(&bit, 3), bit.current_byte_index );

    // for (unsigned char mask = 0x80; mask; mask >>= 1) putchar(icomiki->image_data.chunk_data[2] & mask ? '1' : '0'); printf(" \n");
    // for (unsigned char mask = 0x80; mask; mask >>= 1) putchar(icomiki->image_data.chunk_data[3] & mask ? '1' : '0'); printf(" \n");
    // for (unsigned char mask = 0x80; mask; mask >>= 1) putchar(icomiki->image_data.chunk_data[4] & mask ? '1' : '0'); printf(" \n");
    // for (unsigned char mask = 0x80; mask; mask >>= 1) putchar(icomiki->image_data.chunk_data[5] & mask ? '1' : '0'); printf(" \n");
    // for (unsigned char mask = 0x80; mask; mask >>= 1) putchar(icomiki->image_data.chunk_data[6] & mask ? '1' : '0'); printf(" \n");


    // log2(n)−8 = 3
    // log2(n) = 11 (n=2^11, n=2048)
    // 20

    return 0;
}

int PNG_load(PNG_Object object) {
    FILE* fp = load_file("sample.png");

    // icomiki new
    Icomiki icomiki = {0};

    // PNG File Signature
    fread(&icomiki.signature, sizeof(icomiki.signature), 1, fp);

    // Image Header (IHDR)
    fread(&icomiki.image_header, sizeof(icomiki.image_header), 1, fp);

    // Palette (PLTE)
    fread(&icomiki.palette.length, sizeof(icomiki.palette.length), 1, fp);
    fread(&icomiki.palette.chunk_type, sizeof(icomiki.palette.chunk_type), 1, fp);
    int palette_length = PNG_ByteToInt32(icomiki.palette.length, 0, 3);
    icomiki.palette.chunk_data = (unsigned char*)calloc(sizeof(unsigned char), palette_length);
    fread(icomiki.palette.chunk_data, sizeof(unsigned char), palette_length, fp);
    free(icomiki.palette.chunk_data);
    fread(&icomiki.palette.crc, sizeof(icomiki.palette.crc), 1, fp);

    // Meta data (tEXt)
    fread(&icomiki.meta_data.length, sizeof(icomiki.meta_data.length), 1, fp);
    fread(&icomiki.meta_data.chunk_type, sizeof(icomiki.meta_data.chunk_type), 1, fp);
    int meta_data_length = PNG_ByteToInt32(icomiki.meta_data.length, 0, 3);
    icomiki.meta_data.chunk_data = (unsigned char*)calloc(sizeof(unsigned char), meta_data_length);
    fread(icomiki.meta_data.chunk_data, sizeof(unsigned char), meta_data_length, fp);
    free(icomiki.meta_data.chunk_data);
    fread(icomiki.meta_data.crc, sizeof(unsigned char), sizeof(icomiki.meta_data.crc), fp);

    // Image data (IDAT)
    fread(&icomiki.image_data.length, sizeof(icomiki.image_data.length), 1, fp);
    fread(&icomiki.image_data.chunk_type, sizeof(icomiki.image_data.chunk_type), 1, fp);
    int IDAT_length = PNG_ByteToInt32(icomiki.image_data.length, 0, 3);
    icomiki.image_data.chunk_data = (unsigned char*)calloc(sizeof(unsigned char), IDAT_length);
    fread(icomiki.image_data.chunk_data, sizeof(unsigned char), IDAT_length, fp);

    IcomikiDebug_PrintInfo(&icomiki);

    // zlib



    printf("HLIT: %d\n", icomiki.image_data.chunk_data[6]);  // 19
    printf("HDIST: %d\n", icomiki.image_data.chunk_data[7]); // 16
    // LEN: 12 => 12+4 = 16


    free(icomiki.image_data.chunk_data);
    fread(icomiki.image_data.crc, sizeof(unsigned char), sizeof(icomiki.image_data.crc), fp);


    // Image trailer (TEND)
    fread(&icomiki.image_end, sizeof(unsigned char), sizeof(icomiki.image_end), fp);

    for (int i=0 ; i < sizeof(icomiki.image_end.crc); i++) {
        printf("%02x ", icomiki.image_end.crc[i]);
    }

    free_file(fp);

    return 0;
}
