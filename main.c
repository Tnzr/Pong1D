/*
* GccApplication1.c
*
* Created: 3/23/2020 7:28:50 PM
* Author : Braynyac
*/

#include <avr/io.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
//#define F_CPU 16000000UL // CPU frequency
#define NLED 32 // number of LEDS

void displayScores(int *points){
	PORTD=points[0] || points[1]<<2;
}

void blink(int pinN, int times, int milli_dur){
	for (int i = 0; i < times; i++)
	{
		PORTD |= 1<<pinN;
		_delay_ms(milli_dur);
		PORTD &=~(1<<5);
		_delay_ms(milli_dur);
	}
}

void winBlink(int player,int ledPin, int times, int milli_dur){
	for (int i = 0; i < times; i++)
	{
		PORTD |= 1<<ledPin || (1<<(player*2)) || (1<<(player*2+1));
		_delay_ms(milli_dur);
		PORTD &= ~(1<<ledPin || 1<<player*2 || 1<<(player*2+1));
		_delay_ms(milli_dur);
	}
}

void game(int *points, int *position, int *velocity){
	float frequency = 2;
	while (points[0] < 3 || points[1] < 3 ){ // win condition: points == 3
		displayScores(points);
		_delay_ms(500/frequency);
		*position += velocity; // move in current direction
		
		if (*position > 30) {
			points[0]--;
			points[1]++; // outOfBounds for Player 0
			}else if (*position < 1){
			points[1]--;
			points[0]++;// outOfBounds for Player 1
		}
		if (PINB5){
			if (*position >= 1 && *position <= 3 ){// GREEN 1-4
				frequency*=1.1;
				*velocity*=-1;
				}else if (*position >= 4 && *position <= 8){ // YELLOW 5-9
				frequency*=0.9;
				*velocity*=-1;
				}else{ //else RED or ball in opponent's territory so > 8
				points[0]--;
			}
		}
		else if (PINB6){
			if (*position >= 28 && *position <= 30 )// GREEN 1-4
			frequency*=1.1;
			else if (*position >= 23 && *position <= 27) // YELLOW 5-9
			frequency*=0.9;
			else{ //else RED || ball in opponent's territory so < 23
				points[1]--;
			}
		}
		_delay_ms(255);
		winBlink(points[0]==3? 0 : 1,5, 3, 255);
		
	}
}


void reset(int *points, int *position, int *velocity){
	points[0] = 0;
	points[1] = 1;
	*position = 0;
	*velocity = 0;
}

int main(){
	DDRD |= 0b00011111;
	DDRB |= 0xFF;
	
	int *position;
	int *velocity = calloc(1, sizeof(int));
	int *points = calloc(2, sizeof(int));
	
	while (PINB5 && PINB6 )
	{
		if (PINB5 > PINB6)
		{
			*velocity++;
			*position = NLED / 2;
			game(points, position, velocity);
			reset(points, position, velocity);
			}else if(PINB6>PINB5){
			*velocity--;
			*position = (NLED/2)-1;
			game(points, position, velocity);
			reset(points, position,velocity);
		}
		else{
			blink(5, 1, 100);
		}
	}
	return 0;
}