/*
NarrowFont based on Creep by Romeo Van Snick
https://github.com/romeovs/creep

Alternations by Aaron Eiche 2024
*/

#include <stdint.h>

/*  
Some characters needed 5 bytes :/
*/
static const char nfont_sp[] = {0x00, 0x00, 0x00, 0x00, 0x00};
static const char nfont_exclamation[] = {0xbc, 0x00, 0x00, 0x00, 0x00};          // ASCII 33  !
static const char nfont_dblquote[] = {0x0e, 0x00, 0x0e, 0x00, 0x00};             // ASCII 34  "
static const char nfont_hash[] = {0x50, 0xf8, 0x50, 0xf8, 0x50};                 // ASCII 35  #
static const char nfont_dollar[] = {0x48, 0x54, 0xfe, 0x54, 0x24};               // ASCII 36  $
static const char nfont_percent[] = {0x4c, 0x2c, 0xd0, 0xc8, 0x00};              // ASCII 37  %
static const char nfont_ampersand[] = {0x6c, 0x92, 0xac, 0xc0, 0xa0};            // ASCII 38  &
static const char nfont_sngquote[] = {0x0e, 0x00, 0x00, 0x00, 0x00};             // ASCII 39  '
static const char nfont_lparen[] = {0x3c, 0x42, 0x81, 0x00, 0x00};               // ASCII 40  (
static const char nfont_rparen[] = {0x81, 0x42, 0x3c, 0x00, 0x00};               // ASCII 41  )
static const char nfont_asterisk[] = {0x50, 0x20, 0x50, 0x00, 0x00};             // ASCII 42  *
static const char nfont_plus[] = {0x20, 0x70, 0x20, 0x00, 0x00};                 // ASCII 43  +
static const char nfont_comma[] = {0x80, 0x60, 0x00, 0x00, 0x00};                // ASCII 44  ,
static const char nfont_dash[] = {0x20, 0x20, 0x20, 0x00, 0x00};                 // ASCII 45  -
static const char nfont_period[] = {0x40, 0x00, 0x00, 0x00, 0x00};               // ASCII 46  .
static const char nfont_slash[] = {0xc0, 0x30, 0x0c, 0x03, 0x00};                // ASCII 47  /

static const char nfont_A[] = {0xfc, 0x12, 0x12, 0xfc}; // ASCII 65 'A'
static const char nfont_B[] = {0xfe, 0x92, 0x9c, 0xf0}; // ASCII 66 'B'
static const char nfont_C[] = {0x7c, 0x82, 0x82, 0x44}; // ASCII 67 'C'
static const char nfont_D[] = {0xfe, 0x82, 0x44, 0x38}; // ASCII 68 'D'
static const char nfont_E[] = {0xfe, 0x92, 0x82, 0x82}; // ASCII 69 'E'
static const char nfont_F[] = {0xfe, 0x12, 0x12, 0x02}; // ASCII 70 'F'
static const char nfont_G[] = {0x7c, 0x82, 0xa2, 0x64}; // ASCII 71 'G'
static const char nfont_H[] = {0xfe, 0x08, 0x08, 0xfe}; // ASCII 72 'H'
static const char nfont_I[] = {0x82, 0xfe, 0x82, 0x00}; // ASCII 73 'I'
static const char nfont_J[] = {0x40, 0x82, 0x82, 0xfe}; // ASCII 74 'J'
static const char nfont_K[] = {0xfe, 0x08, 0x34, 0xe2}; // ASCII 75 'K'
static const char nfont_L[] = {0xfe, 0x80, 0x80, 0x80}; // ASCII 76 'L'
static const char nfont_M[] = {0xfe, 0x0c, 0x0c, 0xfe}; // ASCII 77 'M'
static const char nfont_N[] = {0xfe, 0x0c, 0x30, 0xfe}; // ASCII 78 'N'
static const char nfont_O[] = {0x7c, 0x82, 0x82, 0x7c}; // ASCII 79 'O'
static const char nfont_P[] = {0xfe, 0x12, 0x12, 0x0c}; // ASCII 80 'P'
static const char nfont_Q[] = {0x7c, 0x82, 0xc2, 0xfc}; // ASCII 81 'Q'
static const char nfont_R[] = {0xfe, 0x32, 0x52, 0x8c}; // ASCII 82 'R'
static const char nfont_S[] = {0x4c, 0x92, 0x92, 0x64}; // ASCII 83 'S'
static const char nfont_T[] = {0xfd, 0x01, 0xfd, 0xff}; // ASCII 84 'T'
static const char nfont_U[] = {0x7e, 0x80, 0x80, 0xfe}; // ASCII 85 'U'
static const char nfont_V[] = {0x7e, 0x80, 0x80, 0x7e}; // ASCII 86 'V'
static const char nfont_W[] = {0xfe, 0x60, 0x60, 0xfe}; // ASCII 87 'W'
static const char nfont_X[] = {0xf6, 0x08, 0x08, 0xf6}; // ASCII 88 'X'
static const char nfont_Y[] = {0x4e, 0x90, 0x90, 0xfe}; // ASCII 89 'Y'
static const char nfont_Z[] = {0xc2, 0xa2, 0x92, 0x8e}; // ASCII 90 'Z'

static const char nfont_lbracket[] = {0xfe, 0x82, 0x00, 0x00};     // ASCII 91 [
static const char nfont_backslash[] = {0x03, 0x0c, 0x30, 0xc0};  // ASCII 92 backslash 
static const char nfont_rbracket[] = {0x82, 0xfe, 0x00, 0x00}; // ASCII 93 ]
static const char nfont_caret[] = {0x02, 0x01, 0x02, 0x00};      // ASCII 94 ^
static const char nfont_underscore[] = {0x80, 0x80, 0x80, 0x80}; // ASCII 95 _
static const char nfont_backtick[] = {0x03, 0x04, 0x00, 0x00};   // ASCII 96 `

static const char nfont_a[] = {0x60, 0x90, 0x90, 0xf0}; // ASCII 'a'
static const char nfont_b[] = {0xfe, 0x88, 0x88, 0x70}; // ASCII 'b'
static const char nfont_c[] = {0x70, 0x88, 0x88, 0x50}; // ASCII 'c'
static const char nfont_d[] = {0x70, 0x88, 0x88, 0xfe}; // ASCII 'd'
static const char nfont_e[] = {0x70, 0xa8, 0xa8, 0xb0}; // ASCII 'e'
static const char nfont_f[] = {0x10, 0xfc, 0x12, 0x04}; // ASCII 'f'
static const char nfont_g[] = {0x18, 0xa4, 0xa4, 0x7c}; // ASCII 'g'
static const char nfont_h[] = {0xfe, 0x10, 0x10, 0xe0}; // ASCII 'h'
static const char nfont_i[] = {0x08, 0xfa, 0x80, 0x00}; // ASCII 'i'
static const char nfont_j[] = {0x00, 0x88, 0x7a, 0x00}; // ASCII 'j'
static const char nfont_k[] = {0xfe, 0x20, 0x50, 0x88}; // ASCII 'k'
static const char nfont_l[] = {0x7e, 0x80, 0x00, 0x00}; // ASCII 'l'
static const char nfont_m[] = {0xf0, 0x20, 0x20, 0xf0}; // ASCII 'm'
static const char nfont_n[] = {0xf0, 0x10, 0x10, 0xe0}; // ASCII 'n'
static const char nfont_o[] = {0x60, 0x90, 0x90, 0x60}; // ASCII 'o'
static const char nfont_p[] = {0xf8, 0x28, 0x28, 0x30}; // ASCII 'p'
static const char nfont_q[] = {0x18, 0x24, 0x24, 0xfc}; // ASCII 'q'
static const char nfont_r[] = {0xf8, 0x08, 0x08, 0x10}; // ASCII 'r'
static const char nfont_s[] = {0x90, 0xa8, 0xa8, 0x48}; // ASCII 's'
static const char nfont_t[] = {0x08, 0x7c, 0x88, 0x00}; // ASCII 't'
static const char nfont_u[] = {0x78, 0x80, 0x80, 0xf8}; // ASCII 'u'
static const char nfont_v[] = {0x78, 0x80, 0x80, 0x78}; // ASCII 'v'
static const char nfont_w[] = {0xf8, 0x40, 0x40, 0xf8}; // ASCII 'w'
static const char nfont_x[] = {0xd8, 0x20, 0x20, 0xd8}; // ASCII 'x'
static const char nfont_y[] = {0x5c, 0xa0, 0xa0, 0x7c}; // ASCII 'y'
static const char nfont_z[] = {0xc8, 0xa8, 0x98, 0x88}; // ASCII 'z'

static const char nfont_1[] = {0x04, 0xfe, 0x00, 0x00}; // 1
static const char nfont_2[] = {0xc4, 0xa2, 0x92, 0x8c};
static const char nfont_3[] = {0x42, 0x92, 0x9a, 0x66};
static const char nfont_4[] = {0x38, 0x26, 0xfe, 0x20};
static const char nfont_5[] = {0x5e, 0x8a, 0x8a, 0x72};
static const char nfont_6[] = {0x7c, 0x92, 0x92, 0x62};
static const char nfont_7[] = {0x02, 0xc2, 0x32, 0x0e};
static const char nfont_8[] = {0x6c, 0x92, 0x92, 0x6c};
static const char nfont_9[] = {0x4c, 0x92, 0x92, 0x7c};
static const char nfont_0[] = {0x7c, 0xa2, 0x92, 0x7c}; // 0

static const char nfont_colon[] = {0x00, 0x28, 0x00, 0x00};
static const char nfont_semicolon[] = {0x40, 0x28, 0x00, 0x00};

static const char nfont_lt[] = {0x20, 0x50, 0x88, 0x00};
static const char nfont_gt[] = {0x88, 0x50, 0x20, 0x00};
static const char nfont_eq[] = {0x28, 0x28, 0x28, 0x28};

static const char nfont_question[] = {0x04, 0xb4, 0x14, 0x18};
static const char nfont_at[] = {0xfc, 0x84, 0xb4, 0xbc};

static const char nfont_lcurly[] = {0x10, 0x6c, 0x82, 0x00};
static const char nfont_pipe[] = {0x00, 0x00, 0xfe, 0x00};
static const char nfont_rcurly[] = {0x82, 0x6c, 0x10, 0x00};
static const char nfont_tilde[] = {0x08, 0x04, 0x08, 0x04};

char *getChar(uint8_t code)
{
    switch (code)
    {
    case 32:
        return nfont_sp;
    case 33: // ASCII code for '!' 33:
        return nfont_exclamation;
    case 34:
        return nfont_dblquote; // ASCII code for '"'
    case 35:
        return nfont_hash; // ASCII code for '#'
    case 36:
        return nfont_dollar; // ASCII code for '$'
    case 37:
        return nfont_percent; // ASCII code for '%'
    case 38:
        return nfont_ampersand; // ASCII code for '&'
    case 39:
        return nfont_sngquote; // ASCII code for '\''
    case 40:
        return nfont_lparen; // ASCII code for '('
    case 41:
        return nfont_rparen; // ASCII code for ')'
    case 42:
        return nfont_asterisk; // ASCII code for '*'
    case 43:
        return nfont_plus; // ASCII code for '+'
    case 44:
        return nfont_comma; // ASCII code for ','
    case 45:
        return nfont_dash; // ASCII code for '-'
    case 46:
        return nfont_period; // ASCII code for '.'
    case 47:
        return nfont_slash; // ASCII code for '/'

    case 48:
        return nfont_0; // ASCII code for '0'
    case 49:
        return nfont_1; // ASCII code for '1'
    case 50:
        return nfont_2; // ASCII code for '2'
    case 51:
        return nfont_3; // ASCII code for '3'
    case 52:
        return nfont_4; // ASCII code for '4'
    case 53:
        return nfont_5; // ASCII code for '5'
    case 54:
        return nfont_6; // ASCII code for '6'
    case 55:
        return nfont_7; // ASCII code for '7'
    case 56:
        return nfont_8; // ASCII code for '8'
    case 57:
        return nfont_9; // ASCII code for '9'
    case 58:
        return nfont_colon; // ASCII code for ':'
    case 59:
        return nfont_semicolon; // ASCII code for ';'
    case 60:
        return nfont_lt; // ASCII code for '<'
    case 61:
        return nfont_eq; // ASCII code for '='
    case 62:
        return nfont_gt; // ASCII code for '>'
    case 63:
        return nfont_question; // ASCII code for '?'
    case 64:
        return nfont_at; // ASCII code for '@'

    // Uppercase letters
    case 65:
        return nfont_A; // ASCII code for 'A'
    case 66:
        return nfont_B; // ASCII code for 'B'
    case 67:
        return nfont_C; // ASCII code for 'C'
    case 68:
        return nfont_D; // ASCII code for 'D'
    case 69:
        return nfont_E; // ASCII code for 'E'
    case 70:
        return nfont_F; // ASCII code for 'F'
    case 71:
        return nfont_G; // ASCII code for 'G'
    case 72:
        return nfont_H; // ASCII code for 'H'
    case 73:
        return nfont_I; // ASCII code for 'I'
    case 74:
        return nfont_J; // ASCII code for 'J'
    case 75:
        return nfont_K; // ASCII code for 'K'
    case 76:
        return nfont_L; // ASCII code for 'L'
    case 77:
        return nfont_M; // ASCII code for 'M'
    case 78:
        return nfont_N; // ASCII code for 'N'
    case 79:
        return nfont_O; // ASCII code for 'O'
    case 80:
        return nfont_P; // ASCII code for 'P'
    case 81:
        return nfont_Q; // ASCII code for 'Q'
    case 82:
        return nfont_R; // ASCII code for 'R'
    case 83:
        return nfont_S; // ASCII code for 'S'
    case 84:
        return nfont_T; // ASCII code for 'T'
    case 85:
        return nfont_U; // ASCII code for 'U'
    case 86:
        return nfont_V; // ASCII code for 'V'
    case 87:
        return nfont_W; // ASCII code for 'W'
    case 88:
        return nfont_X; // ASCII code for 'X'
    case 89:
        return nfont_Y; // ASCII code for 'Y'
    case 90:
        return nfont_Z; // ASCII code for 'Z'
    
    case 91:
    return nfont_lbracket; // ASCII code for '['
    case 92:
    return nfont_backslash; // ASCII code for '\'
    case 93:
    return nfont_rbracket; // ASCII code for ']'
    case 94:
    return nfont_caret; // ASCII code for '^'
    case 95:
    return nfont_underscore; // ASCII code for '_'
    case 96:
    return nfont_backtick; // ASCII code for '`'
    
    // Lowercase letters
    case 97:
        return nfont_a; // ASCII code for 'a'
    case 98:
        return nfont_b; // ASCII code for 'b'
    case 99:
        return nfont_c; // ASCII code for 'c'
    case 100:
        return nfont_d; // ASCII code for 'd'
    case 101:
        return nfont_e; // ASCII code for 'e'
    case 102:
        return nfont_f; // ASCII code for 'f'
    case 103:
        return nfont_g; // ASCII code for 'g'
    case 104:
        return nfont_h; // ASCII code for 'h'
    case 105:
        return nfont_i; // ASCII code for 'i'
    case 106:
        return nfont_j; // ASCII code for 'j'
    case 107:
        return nfont_k; // ASCII code for 'k'
    case 108:
        return nfont_l; // ASCII code for 'l'
    case 109:
        return nfont_m; // ASCII code for 'm'
    case 110:
        return nfont_n; // ASCII code for 'n'
    case 111:
        return nfont_o; // ASCII code for 'o'
    case 112:
        return nfont_p; // ASCII code for 'p'
    case 113:
        return nfont_q; // ASCII code for 'q'
    case 114:
        return nfont_r; // ASCII code for 'r'
    case 115:
        return nfont_s; // ASCII code for 's'
    case 116:
        return nfont_t; // ASCII code for 't'
    case 117:
        return nfont_u; // ASCII code for 'u'
    case 118:
        return nfont_v; // ASCII code for 'v'
    case 119:
        return nfont_w; // ASCII code for 'w'
    case 120:
        return nfont_x; // ASCII code for 'x'
    case 121:
        return nfont_y; // ASCII code for 'y'
    case 122:
        return nfont_z; // ASCII code for 'z'
    
    case 123:
    return nfont_lcurly; // ASCII code for '{'
    case 124:
    return nfont_pipe; // ASCII code for '|'
    case 125:
    return nfont_rcurly; // ASCII code for '}'
    case 126:
    return nfont_tilde; // ASCII code for '~'
    
    }
};