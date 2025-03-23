#include <SPI_LCD.h>

void setup() {
  // put your setup code here, to run once:
  char msg[]="CH55xduino!! ";
  
  LCD_begin();

  LCD_cursor();
  LCD_clear();
  LCD_display();

  #ifdef LCD_USEPRINT
    char c;
    uint8_t i=0;
    
    LCD_setCursor(1,0);
    LCD_print("Welcome to..");  // *char
    while (c=msg[i++]) {
      delay(500);
      LCD_setCursor(14,LCD_LINES-1);
      LCD_print(i,HEX);         // HEX
      // LCD_print(i-12);
      LCD_setCursor(1+i,1);
      if(LCD_print(c));         // char
      else break;
    }
    LCD_setCursor(14,LCD_LINES-1);
    LCD_print("  ");
  #else
    LCD_setCursor(1,0);
    LCD_prints("Welcome to..");
    LCD_setCursor(2,1);
    delay(500);
    LCD_prints(msg);
  #endif
  
  LCD_noCursor();
  pinMode(11, OUTPUT); // Lチカ
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(11, HIGH);
  LCD_display();
  delay(500);
  digitalWrite(11, LOW);
  LCD_noDisplay();
  delay(500);
}
