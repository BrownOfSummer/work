#ifndef __INCLUDE_VERSION_H__
#define __INCLUDE_VERSION_H__

#define VERSION 4.1

//zamieniæ 127 na (MaxTypeValue<PixelType>()/2-1)

#define POZNAN_DEPTH_BLEND  //New blending mode

#define POZNAN_DEPTH_PROJECT2COMMON

#define OUTPUT_COMPUTATIONAL_TIME

#define POZNAN_GENERAL_HOMOGRAPHY

//#define POZNAN_16BIT_DEPTH

#define POZNAN_DEPTHMAP_CHROMA_FORMAT 420

#ifdef POZNAN_16BIT_DEPTH
typedef unsigned short DepthType;
typedef unsigned char ImageType;
typedef unsigned char HoleType;
#define MAX_HOLE 256
#define MAX_DEPTH (256*256)
#define MAX_LUMA 256

#else
typedef unsigned char DepthType;
typedef unsigned char ImageType;
typedef unsigned char HoleType;
#define MAX_HOLE 256
#define MAX_DEPTH 256
#define MAX_LUMA 256
#endif                                                

template <typename type>  __inline  int   MaxTypeValue                  ()                              { return 0; };
template <>               __inline  int   MaxTypeValue <unsigned short> ()                              { return (256*256); };
template <>               __inline  int   MaxTypeValue <unsigned char>  ()                              { return (256); };

#endif
