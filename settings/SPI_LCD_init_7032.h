// SPI_LCD ライブラリ用設定ファイル(COG1602A)
#ifndef SPI_LCD_init_h
#define SPI_LCD_init_h

// LCD_Printサポート(有効化するとプログラムメモリ使用量が約1.3KB増加する)
#define LCD_USEPRINT    // LCD_Print(), LCD_Printi(), LCD_Printb()を使用するときに定義
// ピン番号指定(SS/CSはオプション)
#define LCD_RS_PIN 14       // RS信号
#define LCD_RST_PIN 34      // RESET信号
// #define LCD_SS_PIN 34       // SS(LATCH)信号
// LCD桁数/行数/表示設定
#define LCD_DIGITS 16   // 16桁
#define LCD_LINES 2     // 2行
#define LCD_DH 0        // 縦倍角表示/10px表示(通常は使用しない)
#define LCD_BLKCUR 0    // ブロックカーソル使用時 1
// ST7032/7036用設定値
#define LCD_CONT 25     // コントラスト(0-63)
#define LCD_RAB 4       // フォロワー増幅率(0-7)
#define LCD_7032        // ST7032使用
// #define LCD_7036        // ST7036使用

#endif