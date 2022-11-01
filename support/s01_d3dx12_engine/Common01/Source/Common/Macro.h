#pragma once

#define ARRAY_LITERAL_SIZE(DATA) (sizeof(DATA)/sizeof(DATA[0]))

#define TOKEN_PAIR(TOKEN) TOKEN,#TOKEN
#define ENUM_TOKEN_PAIR(ENUM, TOKEN) ENUM::TOKEN,#TOKEN

#if defined(_DEBUG)
   #define DSC_ASSERT(CONDITION) assert(CONDITION)
   #define DSC_ASSERT_ALWAYS assert(false)
#else
   #define DSC_ASSERT(CONDITION) (void)0
   #define DSC_ASSERT_ALWAYS (void)0
#endif

