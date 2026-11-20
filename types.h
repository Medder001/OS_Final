#pragma once

typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int 	   u32;
typedef unsigned long long u64;

typedef signed char   	   i8;
typedef signed short  	   i16;
typedef signed int 	       i32;
typedef signed long long   i64;

#if defined(RV32)

  typedef u32 usize;
  typedef i32 isize;

# define _LIT_U32 UL
# define _LIT_U64 ULL

#elif defined(RV64)

  typedef u64 usize;
  typedef i64 isize;

#else
# error "RV32 xor RV64 needs to be defined!"
#endif

