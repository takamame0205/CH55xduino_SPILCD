// SPI_LCD ライブラリ version 0.10
//    2025.3.23 Programmed by Kyoro
#include <SPI_LCD.h>

// グローバル変数
uint8_t LCD_flag = 0;   // 表示制御フラグ(bit 2:表示On/Off, bit 1:カーソルOn/Off, bit 0:ブロックカーソルOn/Off.)

void LCD_begin() {
  // 初期設定

  // IO初期化
  pinMode(LCD_RS_PIN, OUTPUT);
  pinMode(LCD_RST_PIN, OUTPUT);
  digitalWrite(LCD_RS_PIN, LOW);
  digitalWrite(LCD_RST_PIN, HIGH);
  SPI_begin();
  SPI_beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE3));

  // リセット
  delayMicroseconds(LCD_RSTPW);
  digitalWrite(LCD_RST_PIN, LOW);
  delayMicroseconds(LCD_RSTPW);
  digitalWrite(LCD_RST_PIN, HIGH);
  delay(LCD_PWDLY);
  
  // LCDC別初期化コマンド実行
  __LCD_fs(0);
  #ifdef __LCD_INIT
  uint8_t cmd[]=__LCD_INIT;
  uint8_t i,d;

  for(i=0; i<sizeof cmd; i++) {
    d=cmd[i];
    if(!d){
      __LCD_fs(cmd[++i]);
    }
    else if(d==0xff) {
      delay(LCD_IWAIT);
    }
    else {
      LCD_sendCmd(d);
    }
  }
  #endif
  
  // 初期設定
  LCD_flag=LCD_BLKCUR;  // ブロックカーソル使用時には1
  __LCD_onOff();
  __LCD_fs(0);
  LCD_display();
  LCD_clear();
  __LCD_em();
}

void LCD_clear(){
  // 画面クリア
  LCD_sendCmd(0b00000001);
  delay(LCD_WAITRH);
  LCD_setCursor(0, 0);
}

void LCD_home(){
  // ホーム復帰
  LCD_sendCmd(0b00000010);
  delay(LCD_WAITRH);
  LCD_setCursor(0, 0);
}

void LCD_setCursor(uint8_t x, uint8_t y) {
  // カーソル位置指定
  //  x=x座標, y=y座標
  uint8_t a;
  a=__LCD_calcDdramAddr(x, y);
  if(a!=0xff) {
    __LCD_setDdramAddr(a);
  }
}

uint8_t LCD_write(uint8_t data){
  // 1文字表示(＝RAMデータ書き込み)
  //  data=書き込むデータ
  //  戻り値：書き込んだバイト数(常に1)
  digitalWrite(LCD_RS_PIN, HIGH);
  SPI_transfer(data);
  delayMicroseconds(LCD_WAIT);
  return 1;
}

uint8_t LCD_prints(char *str){
  // 文字列表示
  //  string=表示する文字列
  //  戻り値：表示した文字数
  #ifdef LCD_USEPRINT
    // Print.c使用時
    return Print_print_s((writefunc_p)LCD_write, str);
  #else
    // Print.c未使用時
    uint8_t c;
    uint8_t n=0;
    
    if (!str) return 0;
    while (c=*str++) {
      if(LCD_write(c)) n++;
      else break;
    }
    return n;
  #endif
}

#ifdef LCD_USEPRINT
uint8_t LCD_printb(uint32_t data, uint8_t base) {
  // 整数値表示(基数指定)
  //  data=表示する数値(32bit long)
  //  base=基数(BIN, OCT, DEC, HEX)
  //  戻り値：表示した文字数
  return Print_print_ib((writefunc_p)LCD_write, data, base);
}

uint8_t LCD_printi(long data) {
  // 符号付整数値表示
  //  data=表示する数値(32bit long)
  //  戻り値：表示した文字数
  return Print_print_i((writefunc_p)LCD_write, data);
}
#endif

void LCD_display() {
  // 表示ON
  LCD_flag |= 0b0100;
  __LCD_onOff();
}

void LCD_noDisplay() {
  // 表示OFF
  LCD_flag &= 0b1011;
  __LCD_onOff();
}

void LCD_cursor() {
  // カーソル表示ON
  LCD_flag |= 0b10;
  __LCD_onOff();
}

void LCD_noCursor() {
  // カーソル表示OFF
  LCD_flag &= 0b1101;
  __LCD_onOff();
}

void LCD_sendCmd(uint8_t cmd){
  // コマンド送信
  //  data=送信するコマンド
  digitalWrite(LCD_RS_PIN, LOW);
  SPI_transfer(cmd);
  delayMicroseconds(LCD_WAIT);
}

uint8_t __LCD_calcDdramAddr(uint8_t x, uint8_t y) {
  // DDRAMアドレス計算
  //  x=x座標, y=y座標
  //  戻り値：DDRAMアドレス(エラーのとき 0xff)
  if(x>=LCD_DIGITS || y>=LCD_LINES) {
    return 0xff;  // エラー
  }

  /* 以下は論理的には8桁2行の16桁1行LCD用計算式
  #if LCD_LINES==1 && LCD_DIGITS==16
    // 1行16桁のLCD
    if(x>7) {
      return 56+x;
    }
    else {
      return x;
    }
  #endif
  */

  #if LCD_LINES<3
    // 通常の1-2行LCD
    return (y<<6)+x;
  #elif defined(ST7036) && LCD_LINES==3
    // ST7036の3行モード
    return (y<<4)+x;
  #else
    // 通常の3-4行LCD(20桁まで)
    if(y<2) {
      return (y<<6)+x;
    }
    else {
      return (y<<6)-128+LCD_DIGITS+x;
    }
  #endif
}
