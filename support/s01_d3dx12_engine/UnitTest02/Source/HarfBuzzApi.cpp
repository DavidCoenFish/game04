#include "UnitTestPCH.h"

#include <hb.h>

//Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsNotNull( pResultType );
//Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(9, pResultType->Get<int>() );

#if 0
   #define LOGGER(message) Microsoft::VisualStudio::CppUnitTestFramework::Logger::WriteMessage(message)
#else
   #define LOGGER(message) (void)0
#endif


namespace HarfBuzzApi
{
   TEST_CLASS(Basic)
   {

   public:
 
       //https://harfbuzz.github.io/a-simple-shaping-example.html
    TEST_METHOD(SimpleShapingExample)
    {
        //create bufer
        hb_buffer_t *pBuffer = hb_buffer_create();
        const char* const pText = "Lorem ipsum dolor sit amet";
        hb_buffer_add_utf8(pBuffer, pText, -1, 0, -1);
        Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsNotNull( pBuffer );

        //set script, language, direction
        hb_buffer_set_direction(pBuffer, HB_DIRECTION_LTR);
        hb_buffer_set_script(pBuffer, HB_SCRIPT_LATIN);
        hb_buffer_set_language(pBuffer, hb_language_from_string("en", -1));

        //create face and font
        hb_blob_t *pBlob = hb_blob_create_from_file( "Data\\OpenSans.ttf" ); /* or hb_blob_create_from_file_or_fail() */
        Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsNotNull( pBlob );
        hb_face_t *pFace = hb_face_create(pBlob, 0);
        //FT_Set_Char_Size(

        Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsNotNull( pFace );
        hb_font_t *pFont = hb_font_create(pFace);
        Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsNotNull( pFont );

        //hb_font_set_scale(pFont, 32 * 64, 32 * 64); 

        hb_shape(pFont, pBuffer, NULL, 0);

        unsigned int glyph_count = 0;
        hb_glyph_info_t *glyph_info    = hb_buffer_get_glyph_infos(pBuffer, &glyph_count);
        hb_glyph_position_t *glyph_pos = hb_buffer_get_glyph_positions(pBuffer, &glyph_count);

        Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsNotNull( glyph_info );
        Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsNotNull( glyph_pos );
        Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreNotEqual<unsigned int>(0, glyph_count );

        hb_position_t cursor_x = 0;
        hb_position_t cursor_y = 0;
        for (unsigned int i = 0; i < glyph_count; i++) {
            hb_codepoint_t glyphid  = glyph_info[i].codepoint;
            hb_position_t x_offset  = glyph_pos[i].x_offset;
            hb_position_t y_offset  = glyph_pos[i].y_offset;
            hb_position_t x_advance = glyph_pos[i].x_advance;
            hb_position_t y_advance = glyph_pos[i].y_advance;
        /* draw_glyph(glyphid, cursor_x + x_offset, cursor_y + y_offset); */
            cursor_x += x_advance;
            cursor_y += y_advance;
        }


        hb_buffer_destroy(pBuffer);
        hb_font_destroy(pFont);
        hb_face_destroy(pFace);
        hb_blob_destroy(pBlob);
    }

   };
}
