#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_INA219.h>
#include <SPI.h>
#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiAvrI2c.h"

#define TIME_BETWEEN_READS 100
#define HOW_MANY_READS 500

char codeVersion[] = "V1.02";
#define I2C_ADDRESS 0x3C
#define RST_PIN -1

SSD1306AsciiAvrI2c oled;
Adafruit_INA219 ina219;
File testFile;

char fileName[] = "test00.txt";

float shuntvoltage = 0;
float busvoltage = 0;
float current_mA = 0;
float loadvoltage = 0;
float power_mW = 0;

void displayFileName(){
	oled.clear();
  	oled.println(fileName);
}

void displayTestState(int readCounter){
	displayFileName();
	oled.print("Read: ");
	oled.println(readCounter);
}

void testEndedDisplay(){
	displayFileName();
  	oled.println("Ended");
}

void readINA219Values(){
	shuntvoltage = ina219.getShuntVoltage_mV();
	busvoltage = ina219.getBusVoltage_V();
	current_mA = ina219.getCurrent_mA();
	power_mW = ina219.getPower_mW();
	loadvoltage = busvoltage + (shuntvoltage / 1000);
}

void printValuesToFile(){
	testFile.print(busvoltage); 
	testFile.print("\t");
	testFile.print(shuntvoltage); 
	testFile.print("\t");
	testFile.print(loadvoltage); 
	testFile.print("\t");
	testFile.print(current_mA);
	testFile.print("\t");
	testFile.print(power_mW);
	testFile.println("");
}

void test(){
	testFile = SD.open(fileName, FILE_WRITE);
  	for(int i = 0; i<HOW_MANY_READS; i++){
		displayTestState(i);

		readINA219Values();
		printValuesToFile();
		delay(TIME_BETWEEN_READS);
  	}
  	testFile.close();
	testEndedDisplay();
}

void initSdCard(){
	pinMode(4, OUTPUT);
 	if (!SD.begin(4)) {
		Serial.println("SD problem");
		delay(1000);
	}
	Serial.println("SD Ok");
}

void findFreeFileName(){
	for(int i=1; i<100; i++){
		fileName[4] = i/10 +'0';
		fileName[5] = i%10 + '0';
		if(!SD.exists(fileName)){
			Serial.print(fileName);
			Serial.println(" is ava...");
			break;
		}
		Serial.print(fileName);
		Serial.println(" exists");
	}
}

void initFile(){
	findFreeFileName();
	testFile = SD.open(fileName, FILE_WRITE);
	testFile.println(codeVersion);
	testFile.close();
}

void initDisplay(){
	#if RST_PIN >= 0
		oled.begin(&Adafruit128x32, I2C_ADDRESS, RST_PIN);
	#else // RST_PIN >= 0
		oled.begin(&Adafruit128x32, I2C_ADDRESS);
	#endif // RST_PIN >= 0
	oled.setFont(Adafruit5x7);
	oled.clear();
	oled.println("working");
}

void initINA219(){
	ina219.begin();
	ina219.setCalibration_32V_2A();
}

void initAll(){
	Serial.begin(9600);
	initDisplay();
	initSdCard();
	initFile();
	initINA219();
}

void setup(void){
	initAll();
	test();
  	Serial.println("OK");
}

void loop(void) 
{
}

