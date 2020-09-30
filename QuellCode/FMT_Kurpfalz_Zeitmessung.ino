/*
    Name:       FMT_Kurpfalz_Zeitmessung.ino
    Created:	10.09.2019 22:38:01
    Author:     Florian Daubenthaler
*/
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C LCD(0x27, 20, 4);// Arduino Uno A4 & A5
int Pin_V = 1; //Analogpins
int Pin_Start = 2,Start_CDZ = 0;
int Pin_Stop = 3, Stop_CDZ = 0;
unsigned long Pin_PauseWeiter = 4, PauseWeiter_CDZ = 0;
unsigned long Pin_Reset = 5, Reset_CDZ = 0;
int Input_CD = 500;
//--------------------------------
String Status = "";
unsigned long Mesinterval[2] = { 500,0 };

int MAXPausen = 100;
//Zeit
String Zeit = "00:00:000";
String Last_Zeit = "00:00:000";

void setup()
{
	Serial.begin(9600);
	Serial.println("Starte Arduino");
	StartUpDisplay();
}

// Add the main program code into the continuous loop() function
void loop()
{
	LCD_Spannung_Update();
	if (Start())
	{
		Serial.println("Starte Messung neu");
		StoppUhr();
		Serial.println("Messung wurde beändet");
	}
}

#pragma region Display

void LCD_Info_Update()
{
	LCD.clear();
	Serial.println("Update LCD");
	//Statusleiste
	
	//Last Time
	LCD_Zeit_Update();
	LCD_Spannung_Update();
	//Fußzeile
	LCD.setCursor(0, 3);
	LCD.print("FMT-Kurpfalz");
}
void LCD_Spannung_Update()
{
	//Spannungsanzeige
	if(millis() > Mesinterval[0] + Mesinterval[1])
	{
		LCD.setCursor(14, 3);
		LCD.print(String(Spannung_Messen(Pin_V)) + "V");
		Mesinterval[1] = millis();
		Serial.println("Update Spannung");
	}	
}

void LCD_Zeiten_Update()
{
	//Aktuele zeit
	LCD.setCursor(2, 1);
	LCD.print("Zeit = " + Zeit);
	//Letzte zeit
	LCD.setCursor(2, 2);
	LCD.print("Last = " + Zeit);
}
void LCD_Zeit_Update() 
{
	LCD.setCursor(2, 1);
	LCD.print("Zeit = " + Zeit);
	LCD.setCursor(2, 2);
	LCD.print("Last = " + Last_Zeit);
}

float Spannung_Messen(int Pin)
{
	float res = -1;
	float A_In = analogRead(Pin);
	Serial.print("Spannungs Messung : " + String(A_In) + " = ");
	res = float_map(A_In, 0, 1023, 0, 20);
	Serial.println(String(res) +"V");
	return res;
}

float float_map(float x, float in_min, float in_max, float out_min, float out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void StartUpDisplay()
{
	LCD.init();
	LCD.clear();
	Serial.println("Init LCD Screen");
	LCD.backlight();
	Serial.println("Starte Backlight");
	LCD.setCursor(1, 0);
	LCD.print("Zeit Messung");
	LCD.setCursor(1, 1);
	LCD.print("FMT-Kurpfalz");
	LCD.setCursor(8, 2);
	LCD.print("Daubenthaler");
	LCD.setCursor(8, 3);
	LCD.print("Modelbau");
	delay(3000);
	//User test aufvorderung
	LCD.clear();
	LCD.setCursor(0,0);
	LCD.print("Funktionstest:");
	LCD.setCursor(3, 1);
	LCD.print("Bitte Start sie");
	LCD.setCursor(3, 2);
	LCD.print("eine Messung!");
	while (Start() == false) //nur test = true
	{

	}
	LCD.clear();
	LCD.setCursor(0, 0);
	LCD.print("Funktionstest:");
	LCD.setCursor(3, 1);
	LCD.print("Start ist");
	LCD.setCursor(3, 2);
	LCD.print("einsatzbereit!");
	LCD.setCursor(3, 3);
	LCD.print("Weiter mit Weiter Taste");
	while (PauseWeiter() == false)//nur test = true
	{

	}
	LCD.clear();
	LCD.setCursor(0, 0);
	LCD.print("Funktionstest:");
	LCD.setCursor(3, 1);
	LCD.print("Bitte Stoppen sie");
	LCD.setCursor(3, 2);
	LCD.print("die Messung!");
	while (Stop() == false) //nur test = true
	{

	}
	LCD.clear();
	LCD.setCursor(0, 0);
	LCD.print("Funktionstest:");
	LCD.setCursor(3, 1);
	LCD.print("Ziel ist");
	LCD.setCursor(3, 2);
	LCD.print("einsatzbereit!");
	LCD.setCursor(3, 3);
	LCD.print("Weiter mit Weiter Taste");
	while (PauseWeiter() == false)//nur test = true
	{

	}

	LCD_Info_Update();
}
#pragma endregion

//Lichtschranke abfrage Zum Starten
boolean Start()
{
	boolean push = false;

	if (digitalRead(Pin_Start) == HIGH && (millis() - Start_CDZ) >= Input_CD)
	{
		push = true;
		Serial.println("Start Wurde gedrückt.");
		Start_CDZ = millis();
	}
	return push;
}

//Lichtschranke abfrage Zum Stoppen
boolean Stop()
{
	boolean push = false;

	if (digitalRead(Pin_Stop) == HIGH && (millis() - Stop_CDZ) >= Input_CD)
	{
		push = true;
		Serial.println("Stop Wurde gedrückt.");
		Stop_CDZ = millis();
	}
	return push;
}

//Tasten abfrage Zum Reseten
boolean Reset() 
{
	boolean push = false;

	if (digitalRead(Pin_Reset) == HIGH && (millis() - Reset_CDZ) >= Input_CD)
	{
		push = true;
		Serial.println("Reset Wurde gedrückt.");
		Reset_CDZ = millis();
	}
	return push;
}

//Tasten abfrage Zum Pausieren oder weiter messen
boolean PauseWeiter()
{
	boolean push = false;

	if (digitalRead(Pin_PauseWeiter) == HIGH && (millis() - PauseWeiter_CDZ) >= Input_CD)
	{
		push = true;
		Serial.println("PauseWeiter Wurde gedrückt.");
		PauseWeiter_CDZ = millis();
	}
	return push;
}



void StoppUhr()
{
	int counter = 0;
	boolean run = true;
	unsigned long zeitof = millis();
	unsigned long messung = 0;
	unsigned long oldSec = 0;

	while (true)
	{
		LCD_Zeit_Update();
		LCD_Spannung_Update();
		if (PauseWeiter() && counter < MAXPausen)
		{
			run = true;
			zeitof = millis();
		}
		else if (PauseWeiter())
		{
			//Fehler meldung ausgeben
		}
		while (run)
		{
			LCD_Spannung_Update();
			//Messe Zeit
			messung += millis() - zeitof;
			zeitof = millis();
			//Berechnen der Zeit
			int ml = 0;
			int Sec = messung / 1000;
			int Min = Sec / 60;
			if(Sec >= oldSec && Sec >= oldSec + 60)
			{
				oldSec += 60;
			}
			Sec = Sec - oldSec;
			String m = String(messung);
			if (m.length() >= 3) 
			{
				ml = m.length() - 3;
			}
			String Mil = m.substring(ml);
			if (Min >= 10 && Sec >= 10)
			{
				Zeit = String(Min) + ":" + String(Sec) + ":" + Mil;
			}
			else if(Min < 10 && Sec >= 10)
			{
				Zeit = "0" + String(Min) + ":" + String(Sec) + ":" + Mil;
			}
			else if (Min >= 10 && Sec < 10)
			{
				Zeit = String(Min) + ":" + "0" + String(Sec) + ":" + Mil;
			}
			else
			{
				Zeit = "0" + String(Min) + ":" + "0" + String(Sec) + ":" + Mil;
			}
			Serial.println(Zeit);

			//Pausieren der Messung
			if (PauseWeiter() || Stop())
			{
				if (counter < MAXPausen)
				{
					counter++;
				}
				run = false;
			}
			LCD_Zeit_Update();
			LCD_Spannung_Update();
		}
		if (Reset()) 
		{
			Last_Zeit = Zeit;
			zeitof = 0;
			Zeit = "00:00:000";
			Serial.println("Zeit = " + Zeit);
			Serial.print("Last_Zeit = " + Last_Zeit);
			LCD_Zeit_Update();
			break;
		}
	}
}