// SPI_LCD ライブラリ version 0.20
//    2025.3.23 Programmed by Kyoro
#ifndef SPI_LCD_h
#define SPI_LCD_h
#include <SPI.h>
#include "SPI_LCD_init.h"
#ifndef SPI_LCD_init_h
#include <SPI_LCD_init_d.h>
#endif

// LCD待ち時間設定
#define LCD_WAIT 100    // LCDの待ち時間(us)
#define LCD_WAITRH 2    // LCDの待ち時間(ms,クリア時)
#define LCD_PWDLY 40    // LCDの待ち時間(ms,リセット時)
#define LCD_RSTPW 100   // LCDリセット信号のパルス幅(us)
#define LCD_IWAIT 200   // LCD初期設定中の待ち時間(ms):初期化コマンド列で指定するもの
#define LCD_FSWAIT 4    // LCD初期設定中の待ち時間(ms):初回FS実行後の待ち時間(パラレルLCD用)

// LCDC初期化パラメータ
#define __LCD_CONH ((LCD_CONT>>4) & 0b11)   // Contrast(H)
#define __LCD_CONL (LCD_CONT & 0x0f)        // Contrast(L)
#define __LCD_BON 4                         // Booster ON(IS=1)
#define __LCD_UD 8                          // DH Upper Line(IS=2)

// LCDC初期化コマンド列
//  0x00:次の1バイトをパラメータとして __LCD_fs() 呼ぶ
//   (主に 0x00, i として、IS=i とするのに使用する)
//  0xff:200ms(LCD_IWAIT定義値)待つ
//  その他:そのままコマンドとして LCD_sendCmd()で送る
#if defined(LCD_7032)
#define __LCD_INIT { 0x00, 0x01,        /* IS=1 */\
        0x14,                           /* 1/5 Bias, F2-0=0b100 (192Hz) */\
        0x70 | __LCD_CONL,              /* Set C3-C0 */\
        0x58 | __LCD_BON | __LCD_CONH,  /* ICON ON, Booster ON, Set C5-C4 */\
        0x68 | LCD_RAB,                 /* Follower ON, Set Rab */\
        0xff,                           /* Wait 200ms */\
        0x00, 0x00                      /* IS=0 */\
    }
#elif defined(LCD_7036)
#if LCD_LINES==3
#define __LCD_INIT { 0x00, 0x01,        /* IS=1 */\
        0x15,                           /* 1/5 Bias, 3Line */\
        0x70 | __LCD_CONL,              /* Set C3-C0 */\
        0x58 | __LCD_CONH,              /* ICON ON, Booster OFF, Set C5-C4 */\
        0x68 | LCD_RAB,                 /* Follower ON, Set Rab */\
        0xff,                           /* Wait 200ms */\
        0x00, 0x00                      /* IS=0 */\
    }
#else
#define __LCD_INIT { 0x00, 0x01,        /* IS=1 */\
        0x14,                           /* 1/5 Bias, 1-2Line */\
        0x70 | __LCD_CONL,              /* Set C3-C0 */\
        0x58 | __LCD_CONH,              /* ICON ON, Booster OFF, Set C5-C4 */\
        0x68 | LCD_RAB,                 /* Follower ON, Set Rab */\
        0xff,                           /* Wait 200ms */\
        0x00, 0x00                      /* IS=0 */\   
    }
#endif
#endif

// プロトタイプ
void LCD_begin();
void LCD_clear();
void LCD_home();
void LCD_setCursor(uint8_t x, uint8_t y);
uint8_t LCD_write(uint8_t data);
uint8_t LCD_prints(char *str);
void LCD_display();
void LCD_noDisplay();
void LCD_cursor();
void LCD_noCursor();
// void LCD_blink();  未実装
// void LCD_noBlink();
// void LCD_scrollDisplayLeft();
// void LCD_scrollDisplayRight();
// void LCD_autoscroll();
// void LCD_noAutoscroll();
// void LCD_createChar();
void LCD_sendCmd(uint8_t cmd);
uint8_t __LCD_calcDdramAddr(uint8_t x, uint8_t y);

// 関数マクロ
#define __LCD_onOff() (LCD_sendCmd(LCD_flag | 0b00001000))                  // 表示ON/OFF
#define __LCD_em() (LCD_sendCmd(0b00000110))                                // エントリーモードセット
#define __LCD_setDdramAddr(address) (LCD_sendCmd(0x80 | (address & 0x7f)))  // DDRAMアドレス指定
#define __LCD_setCgramAddr(address) (LCD_sendCmd(0x40 | (address & 0x3ff))) // CGRAMアドレス指定
#if LCD_LINES==1
#define __LCD_fs(data) (LCD_sendCmd(0b00110000 | (LCD_DH << 2) | data))     // ファンクションセット
#else
#define __LCD_fs(data) (LCD_sendCmd(0b00111000 | (LCD_DH << 2) | data))
#endif

// LCD_print()サポート
//  LCD_print(char) →　文字コードcharの文字を1文字表示
//  LCD_print(int/short/long) →　符号付整数値を表示
//  LCD_print(符号なし整数型) →　整数値を表示(longは符号付に変換される)
//  LCD_print(*char) →　文字列を表示
//  LCD_print(整数型, base) → 符号なし整数値を基数baseで表示
// 戻り値は表示した文字数
#ifdef LCD_USEPRINT
uint8_t LCD_printb(uint32_t data, uint8_t base);
uint8_t LCD_printi(long data);
#define LCD_print(...) LCD_SELECT(__VA_ARGS__)(__VA_ARGS__)
#define LCD_SELECT(...)                                           \
  CONCAT(LCD_SELECT_, NARG(__VA_ARGS__))(__VA_ARGS__)
#define LCD_SELECT_0() printNothing
#define LCD_SELECT_1(_1)                                          \
  _Generic((_1),                                                  \
      char: LCD_write,                                            \
      int: LCD_printi,                                            \
      short: LCD_printi,                                          \
      long: LCD_printi,                                           \
      unsigned char: LCD_printi,                                  \
      unsigned int: LCD_printi,                                   \
      unsigned short: LCD_printi,                                 \
      unsigned long: LCD_printi,                                  \
      __code char *: LCD_prints,                                  \
      __data char *: LCD_prints,                                  \
      __xdata char *: LCD_prints)
#define LCD_SELECT_2(_1, _2) LCD_printb
/*
  _Generic((_1),                                                  \
      char: _Generic((_2), default: LCD_printb),                  \
      int: _Generic((_2), default: LCD_printb),                   \
      short: _Generic((_2), default: LCD_printb),                 \
      long: _Generic((_2), default: LCD_printb),                  \
      unsigned char: _Generic((_2), default: LCD_printb),         \
      unsigned int: _Generic((_2), default: LCD_printb),          \
      unsigned short: _Generic((_2), default: LCD_printb),        \
      unsigned long: _Generic((_2), default: LCD_printb)
*/
#endif

#endif