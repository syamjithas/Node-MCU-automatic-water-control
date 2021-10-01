#include <pitches.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

#define trig D3
#define echo D0
#define LEDG D4
#define PUSHBUTTON 9
#define REALY 10

Adafruit_PCD8544 display = Adafruit_PCD8544(14, 13, 5, 15, 4);

const int buzzerPin = D6;
int contrastValue = 50;

bool manualON = false;
bool motorState = false;
int currentLevel = 0;
int counter = 0;
int intialLevel = 0;
bool dryRun = false;

void setup()
{
  pinMode(PUSHBUTTON, INPUT);
  pinMode(REALY, OUTPUT);
  pinMode(trig, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(buzzerPin, OUTPUT);
  display.begin();
  display.setContrast(contrastValue);
  digitalWrite(REALY, 1);
  nokia5110("Motor is OFF");
}

void loop()
{

  waterLevelCalc();
  if (digitalRead(PUSHBUTTON) == HIGH)
  {
    if (dryRun)
    {
      dryRun = false;
      nokia5110("Motor is OFF");
    }
    else
    {
      manualON = true;
    }
  }

  if (dryRun)
  {
    nokia5110("DRY RUN");
    dryRunBeep();
  }
  else
  {
    if (motorState)
    {
      nokia5110("Motor is ON");
    }
    else
    {
      nokia5110("Motor is OFF");
    }

    if (currentLevel >= 100)
    {
      turnOffMotor();
      counter = 0;
    }

    if ((!motorState && currentLevel < 95) && (manualON || currentLevel < 40))
    {
      turnOnMotor();
    }

    if (motorState)
    {
      if (counter == 0)
      {
        intialLevel = currentLevel + 2;
      }

      if (counter > 700)
      {
        if (intialLevel >= currentLevel)
        {
          dryRun = true;
          turnOffMotor();
          counter = 0;
        }
      }
      counter++;
      
    }
  }
  delay(100);
}

void dryRunBeep()
{
  beep(NOTE_DS8, 200);
  delay(4900);
}

void nokia5110(String str)
{
  display.setTextSize(1);
  display.clearDisplay();
  display.setTextColor(BLACK, WHITE);
  display.setCursor(5, 0);
  display.print(str);
  display.drawFastHLine(0, 10, 83, BLACK);
  display.setCursor(5, 15);
  display.print("Water Level");
  display.setTextSize(2);
  display.setCursor(5, 25);
  display.print(currentLevel);
  display.print("%");
  display.setTextSize(2);
  display.display();
}

void waterLevelCalc()
{
  digitalWrite(trig, LOW);
  delayMicroseconds(4);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  long t = pulseIn(echo, HIGH);
  currentLevel = 123 - (t / 29 / 2);
  Serial.println(currentLevel);
}

void turnOffMotor()
{
  if (motorState)
  {
    nokia5110("Motor is OFF");
    digitalWrite(LEDG, 0);
    digitalWrite(REALY, 1);
    beep(NOTE_A4, 100);
    delay(50);
    beep(NOTE_A4, 100);
    delay(50);
    beep(NOTE_A4, 100);
    manualON = false;
    motorState = false;
  }
}

void turnOnMotor()
{
  if (!motorState)
  {
    nokia5110("Motor is ON");
    digitalWrite(LEDG, 1);
    digitalWrite(REALY, 0);
    motorState = true;
    beep(NOTE_C5, 100);
    delay(50);
    beep(NOTE_C5, 100);
    delay(50);
    beep(NOTE_C5, 100);
  }
}

void beep(int note, int duration)
{
  tone(buzzerPin, note, duration);
  delay(duration);
}

void alarm()
{
  beep(NOTE_A4, 500);
}
