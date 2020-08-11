#include <avr/io.h>
#include <stdio.h>
#include "main_init.h"
#include "lcd.h"
#include "ds3231.h"
int h, i, b, d,m,y, w, zeit;
BYTE time[3] = {0};
BYTE data[4] = {0};
BYTE string[20] = {0};
SBYTE temperatura = 0;

// ----- Timer setting ----- //
void zeit_einstellung(void){
	while (PINB&0x10){               // When button PIN 4 is pressed, the loop pause is started to set time. Button must be printed at setting.
		if(PINB&0x40){               // When button PIN 6 is pressed, counts hour
			for (i=0; i<1; i++){
				h++;
				if(h > 23){
					h=0;
				}
				_delay_ms(300);
			}
		}
		else if (PINB&0x20){         // When button PIN 5 is pressed, counts minutes
			for (i=0; i<1; i++){
				b++;
				if(b > 59){
					b=0;
				}
				_delay_ms(150);
			}
		}
		ds3231_write_time(DS3231_24, h,b,0);
		sprintf(string, "%02i:%02i", h,b);
		lcd_gotoxy(0,0);
		lcd_puts(string);
	}
}
// ----- Data settine ----- //
void data_einstellung(void){
	while (PINB&0x80){ // When button PIN 7 is pressed, the loop pause is started to set date. Button must be printed at setting.
		if(PINB&0x10){ // Button Pin 4 day setting
			for (i=0; i<1; i++){
				d++;
				if(d > 31){
					d=0;
				}
				_delay_ms(300);
			}
		}
		else if (PINB&0x20){ // Button Pin 5 month setting
			for (i=0; i<1; i++){
				m++;
				if(m > 12){
					m=0;
				}
				_delay_ms(150);
			}
		}
		else if (PINB&0x40){ // Button Pin P6 year setting
			for (i=0; i<1; i++){
				y++;
				if(y >= 30){
					y=19;
				}
				_delay_ms(500);
			}
		}
		ds3231_write_data(w, d, m, y); //week day month Year
		sprintf(string, "%02i.%02i.20%02i", d, m, y);
		lcd_gotoxy(0,1);
		lcd_puts(string);
	}
}


// ----- Timer funktion ----- //
void time_anzeige (){
	_delay_ms(3);
	// Zeit lesen
	ds3231_read_time(time);
	// Zeit zeigen
	sprintf(string, "%02i:%02i:%02i", time[0], time[1], time[2]);

	lcd_gotoxy(0,0);
	lcd_puts(string);

	// Datei lesen
	ds3231_read_data(data);
	// Datei speichern
	sprintf(string, "%i.%i.20%02i", data[1], data[2], data[3]);

	lcd_gotoxy(0,1);
	lcd_puts(string);

	// Leysen Temper
	temperatura = ds3231_read_temp();
	// Anzeigen Temper
	sprintf(string, "%3i", temperatura);
	lcd_gotoxy(13,0);
	lcd_puts(string);

}
int main(void)
{
	//DDRB = 0xF8;
	//DDRD = 0xE0;
	unsigned char pwm_state=0;
	init_PWM_timer1(); // OCR0  weiss
	init_PWM_timer2(); // OCR1A rot
    init_PWM_timer3(); // OCR2  blau

	//zeit_einstellung(a);
	lcd_init(LCD_DISP_ON);
	// UHR ini
	ds3231_init();




	while(1)
	{
		data_einstellung();
		zeit_einstellung();
		//time_anzeige();

		if(time[1] >=1 && time[1] < 2 ){ //6.00 to 7.00
			zeit  = time[2];
			OCR0  = 253 - zeit;
			OCR1A =0;
			OCR2  = 210 - zeit;
		}
		else if(time[1] >=2 && time[1] < 3){ // 7.00 to 8.00
			zeit  = time[2];
			OCR0  = 193 - zeit*2;
			OCR1A = 133 - zeit*2;
			OCR2  = 150 - zeit*2;
		}
		else if(time[1] >=3 && time[1] < 4){ //8.00 to 9.00
			zeit  = time[2];
			OCR0  = 73 - zeit;
			OCR1A = 0;
			OCR2  = 0;
			//relai on
		}
		else if(time[1] >=4 && time[1] < 5){ // 9.00 to 13.00
			OCR0  = 0;
			OCR1A = 0;
			OCR2  = 0;
		}
		else if(time[1] >= 5 && time[1] < 6){ // 13.00 -14.00
			zeit  = time[2];
			OCR0  = zeit*4;
			OCR1A = zeit*4;
			OCR2  = zeit*4;
			// relai off
		}
		else if(time[1] >= 6 && time[1] < 7){ // 14.00 - 15.00
			OCR0  = 253;
			OCR1A = 253;
			OCR2  = 253;
		}
		else if(time[1] >= 7 && time[1] < 8){ //15.00 to 16.00
			zeit  = time[2];
			OCR0  = 253 - zeit*4;
			OCR1A = 253 - zeit*4;
			OCR2  = 253 - zeit*4;
			//relai on
		}
		else if(time[1] >=8 && time[1] < 9){ // 16.00 to 20.00
			OCR0  = 0;
			OCR1A = 0;
			OCR2  = 0;
		}
		else if(time[1] >=9 && time[1] < 10){ // 20.00 to 21.00
			OCR0  = 0;
			OCR1A = 0;
			OCR2  = 0;
			//relai off
		}
		else if(time[1] >=10 && time[1] < 11){ // 21.00 to 22.00
			zeit  = time[2];
			OCR0  = 0 + zeit*4;
			OCR1A = 0 + zeit*4;
			OCR2  = 0 + zeit*3;
		}
		else if(time[1] >=11 && time[2] <=59 ){ // 22.00 to 06.00
			OCR0  = 253;
			OCR1A = 253;
			OCR2  = 180;
		}
		else if(time[1] >= 0 && time[1] < 1){ // 22.00 to 06.00
			OCR0  = 253;
			OCR1A = 253;
			OCR2  = 190;
		}

	}

}
