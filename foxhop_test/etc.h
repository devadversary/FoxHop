#pragma once
/**
    @brief �÷����� ����ü pack Ű���� ����
*/
#ifdef WIN32
#define STRUCT_PACK_START #pragma pack(1)
#define STRUCT_PACK_END   #pragma pack()
#define STRUCT_PACK_TRAILER
#elif __linux__
#define STRUCT_PACK_START
#define STRUCT_PACK_END
#define STRUCT_PACK_TRAILER __attribute__((packed))
#elif AIX
#define STRUCT_PACK_START #pragma options align=packed
#define STRUCT_PACK_END   #pragma options align=reset
#define STRUCT_PACK_TRAILER
#elif HP
#define STRUCT_PACK_START #pragma pack 1
#define STRUCT_PACK_END   #pragma pack 8
#define STRUCT_PACK_TRAILER
#elif COMPAQ
#define STRUCT_PACK_START #pragma pack 1
#define STRUCT_PACK_END   #pragma pack 0
#define STRUCT_PACK_TRAILER
#elif SUN
#define STRUCT_PACK_START #pragma pack(1)
#define STRUCT_PACK_END   #pragma pack()
#define STRUCT_PACK_TRAILER
#endif
/**
    @brief IP ���
*/
#pragma pack(1)
typedef struct _st_ipv4
{
#if 1 /*TODO : �򿣵�� ��� �ؾ���.*/
    unsigned char  HdrLen : 4; /**< ��� ���� 1�� 4����Ʈ (��� 5 : 20����Ʈ)*/
    unsigned char  Version : 4;
    unsigned char  ToS;
    unsigned short TotalLen;
    unsigned short ID;
    unsigned short Frag : 13;
    unsigned short MF : 1;
    unsigned short DF : 1;
    unsigned short Reserved : 1;
    unsigned char  TTL;
    unsigned char  Protocol;
    unsigned short HdrChecksum;
    unsigned int   SrcAddr;
    unsigned int   DstAddr;
#elif
#endif
}IPv4;
#pragma pack()
