#pragma once

#define RENGINE_NOT_USED(x) ( (void)(x) )

// Description
//	Text and expression quoting
#define RENGINE_QUOTE_(expression) #expression
#define RENGINE_QUOTE(expression) RENGINE_QUOTE_(expression)

// Description
//	Expression combiner
#define RENGINE_COMBINE_(expression_0, expression_1) expression_0##expression_1
#define RENGINE_COMBINE(expression_0, expression_1) RENGINE_COMBINE_(expression_0, expression_1)

// Description
//	Unique ids
#define RENGINE_UNIQUE(expression) RENGINE_COMBINE(expression, __COUNTER__)
#define RENGINE_UNIQUE_LINE(expression) RENGINE_COMBINE(expression, __LINE__)

// Description
// Converts a enum value name to text
#define RENGINE_QUOTE_ENUM_BEGIN(output, variable) 	std::string output; switch (variable) {
#define RENGINE_QUOTE_ENUM(output, enum_name) case enum_value: { output = #enum_name; } break;
#define RENGINE_QUOTE_ENUM_NS(output, name_space, enum_name) case name_space::enum_name: { output = #enum_name; } break;
#define RENGINE_QUOTE_ENUM_END(output, missing_value) default: { output = missing_value; } break; }

//
// assert
//
#if defined(_DEBUG) || !defined(NDEBUG)
	#include <cassert>
	#define RENGINE_ASSERT(expression) assert(expression)
#else
	#define RENGINE_ASSERT(expression)
#endif 

//
// Inline
//
#define RENGINE_INLINE inline

//
// int64
//
#if defined( _MSC_VER )
	#define RG_INT64 signed __int64
	#define RG_UINT64  unsigned __int64
	#define RENGINE_INT64_MASK "I64"
#else
	#define RG_INT64 long long int
	#define RG_UINT64 unsigned long long int
	#define RENGINE_INT64_MASK "ll"
#endif


//
// Global language code
//
namespace re
{

	typedef float Real32;
	typedef double Real64;
	typedef float Real;

	typedef signed char Int8;
	typedef signed short Int16;
	typedef signed int Int32;
	typedef RG_INT64 Int64;

	typedef unsigned char Uint8;
	typedef unsigned short Uint16;
	typedef unsigned int Uint32;
	typedef RG_UINT64 Uint64;

	typedef unsigned char Uchar;
	typedef char Char;

	typedef Uint16 Ushort;
	typedef Int16 Short;

	typedef Uint32 Uint;
	typedef Int32 Int;

	typedef Uint64 Ulong;
	typedef Int64 Long;

	typedef Uint8 Byte;
	typedef bool Bool;


} // end of namespace

