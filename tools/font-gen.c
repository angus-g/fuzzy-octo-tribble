#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "lodepng.h"

//const char *font = "/usr/share/fonts/adobe-source-sans-pro/SourceSansPro-Regular.otf";
const char *font = "WeatherIcons-Regular.otf";
const char *outfile = "weather.png";
const int width = 400, height = 50;
const int font_size = 24;

int main(int argc, char **argv) {
    FT_Library library;
    FT_Face face;
    int error, glyph_index;
    unsigned char *image, *png;
    size_t pngsize;
    LodePNGState state;

    setlocale(LC_ALL, "");

    image = malloc(width * height * 2 * sizeof(unsigned char));

    // white-out image (transparent)
    for (int i = 0; i < width * height; i++) image[i] = 0x00;

    // initialise png
    lodepng_state_init(&state);
    state.info_raw.colortype = LCT_GREY_ALPHA;

    FT_Init_FreeType(&library); // initialise freetype2
    FT_New_Face(library, font, 0, &face); // load font face (index = 0)
    FT_Set_Char_Size(face, 0, font_size*64, 0, 0); // set character size to <font_size>pt for default dpi (72)

    // letters to render
    //const wchar_t *text = L"abcdefghijklmnopqrstuvwxyz'°C0123456789 ";
    //const wchar_t *text = L"abcdefghijklmnopqrstuvwxyz' ";
    //const wchar_t *text = L"°C0123456789";
    const wchar_t *text = L"";
    int l = wcslen(text);

    int pen_y = height - 10, pen_x = 0; // starting position of pen
    FT_GlyphSlot slot = face->glyph;

    for (int c = 0; c < l; c++) {
        // find glyph index, load and render the glyph
        error = FT_Load_Char(face, text[c], FT_LOAD_RENDER);
        if (error) {
            fprintf(stderr, "Error loading glyph %c\n", text[c]);
            return 1;
        }

        FT_Bitmap bmp = slot->bitmap;
        printf("[%d] = {.x = %d, .l = %d, .t = %d, .w = %d, .h = %d, .a = %d}, // %lc\n",
                text[c], pen_x, slot->bitmap_left, slot->bitmap_top, bmp.width, bmp.rows, slot->advance.x >> 6, text[c]);

        for (int j = 0; j < bmp.rows; j++) {
            int y = pen_y - slot->bitmap_top + j;

            for (int i = 0; i < bmp.width; i++) {
                int x = pen_x + slot->bitmap_left + i;
                image[2 * y * width + 2 * x + 0] = bmp.buffer[j * bmp.width + i];
                image[2 * y * width + 2 * x + 1] = bmp.buffer[j * bmp.width + i];
                //printf("%2x ", bmp.buffer[j * bmp.width + i]);
            }
            //printf("\n");
        }

        pen_x += (slot->advance.x >> 6) + 4;
    }

    // write out png
    error = lodepng_encode(&png, &pngsize, image, width, height, &state);
    if (!error) lodepng_save_file(png, pngsize, outfile);
    else fprintf(stderr, "error %u: %s\n", error, lodepng_error_text(error));

    printf("done!\n");

    return 0;
}
