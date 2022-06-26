#pragma once

#define ARRAY_LITERAL_SIZE(DATA) (sizeof(DATA)/sizeof(DATA[0]))

#define TOKEN_PAIR(TOKEN) TOKEN,#TOKEN
#define ENUM_TOKEN_PAIR(ENUM, TOKEN) ENUM::TOKEN,#TOKEN

#if defined(_DEBUG)
   #define DSC_ASSERT(CONDITION) assert(CONDITION)
#else
   #define DSC_ASSERT(CONDITION) (void)0
#endif

