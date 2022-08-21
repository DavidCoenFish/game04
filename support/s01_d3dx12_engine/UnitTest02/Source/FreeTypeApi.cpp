#include "UnitTestPCH.h"

#include <ft2build.h>
#include FT_FREETYPE_H

//#include FT_ERROR_DEFINITIONS_H
//#include <FreeType\fterrdef.h>
//#undef FTERRORS_H_
//#define FT_ERRORDEF(e, v, s) { e, s },
//#define FT_ERROR_START_LIST     {
//#define FT_ERROR_END_LIST       { 0, NULL } };

//const struct
//{
//int          err_code;
// const char* err_msg;
//
//} ft_errors[] =
//
//#include <freetype/fterrors.h>

//Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsNotNull( pResultType );
//Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(9, pResultType->Get<int>() );

#if 0
   #define LOGGER(message) Microsoft::VisualStudio::CppUnitTestFramework::Logger::WriteMessage(message)
#else
   #define LOGGER(message) (void)0
#endif


namespace FreeTypeApi
{
   TEST_CLASS(Basic)
   {

   public:
 
       //file:///C:/Users/David/Downloads/ftdoc2121/freetype-2.12.1/docs/tutorial/example1.c
    TEST_METHOD(LoadFont)
    {
        FT_Library    library;
        FT_Face       face;
        FT_GlyphSlot  slot;
        FT_Error      error;

        error = FT_Init_FreeType(&library);
        Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(0, error);

        error = FT_New_Face(library, "Data\\OpenSans.ttf", 0, &face);
        Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(0, error);

        error = FT_Set_Char_Size(face, 50 * 64, 0,
            100, 0);
        Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(0, error);

        slot = face->glyph;

        error = FT_Load_Char(face, 'L', FT_LOAD_RENDER);
        Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(0, error);

        Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreNotEqual<int>(0, slot->bitmap.width);
        Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreNotEqual<int>( 0, slot->bitmap.rows );

        FT_Done_Face(face);
        FT_Done_FreeType(library);
    }

   };
}
