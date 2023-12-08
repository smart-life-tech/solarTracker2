#include <Servo.h>
#include <LiquidCrystal_I2C.h>

// Pin configuration
const int ldrPins[] = {A0, A1, A2, A3}; // LDR pins
const int servoHorizontalPin = 9;       // Horizontal servo control pin
const int servoVerticalPin = 10;        // Vertical servo control pin

// LCD configuration
LiquidCrystal_I2C lcd(0x27, 20, 4); // I2C address 0x27, 16 column and 2 rows

// Servo objects
Servo servoHorizontal;
Servo servoVertical;

void setup()
{
    // Initialize the LCD
    lcd.begin(16, 2);
    lcd.print("Solar Tracker");

    // Attach servos to pins
    servoHorizontal.attach(servoHorizontalPin);
    servoVertical.attach(servoVerticalPin);
    // lets test the servos;
    for (int i = 0; i < 180; ++)
    {
        servoHorizontal.write(i);
        delay(20);
    }
    for (int i = 180; i > 0; --)
    {
        servoHorizontal.write(i);
        delay(20);
    }

    for (int i = 0; i < 180; i++)
    {
        servoVertical.write(i);
        delay(20);
    }
    for (int i = 180; i > 0; i--)
    {
        servoVertical.write(i);
        delay(20);
    }
}

void loop()
{
    // Read LDR values
    int ldrValues[4];
    for (int i = 0; i < 4; ++i)
    {
        ldrValues[i] = analogRead(ldrPins[i]);
        Serial.print("raw analog values:  ");
        Serial.print(ldrValues[i]);
    }

    // Calculate light intensity for each side
    int intensities[4];
    for (int i = 0; i < 4; ++i)
    {
        intensities[i] = map(ldrValues[i], 0, 1023, 0, 100); // Map the analog value to a percentage
        Serial.print("calculated  values:  ");
        Serial.print(intensities[i]);
    }

    // Determine optimal position
    int optimalPosition = determineOptimalPosition(intensities);

    // Update servo positions
    updateServoPositions(optimalPosition);

    // Display information on LCD
    displayLCD(intensities, optimalPosition);

    // Add a delay to prevent constant updates
    delay(1000);
}

int determineOptimalPosition(int intensities[])
{
    // Find the index of the maximum intensity
    int maxIndex = 0;
    for (int i = 1; i < 4; ++i)
    {
        if (intensities[i] > intensities[maxIndex])
        {
            maxIndex = i;
        }
    }
    return maxIndex;
}

void updateServoPositions(int optimalPosition)
{
    // Adjust horizontal servo
    if (optimalPosition == 0 || optimalPosition == 1)
    {
        servoHorizontal.write(90); // Face east or west
    }
    else if (optimalPosition == 2)
    {
        servoHorizontal.write(0); // Face north
    }
    else
    {
        servoHorizontal.write(180); // Face south
    }

    // Adjust vertical servo
    if (optimalPosition == 2 || optimalPosition == 3)
    {
        servoVertical.write(90); // Face up or down
    }
    else
    {
        servoVertical.write(0); // Face level
    }
}

void displayLCD(int intensities[], int optimalPosition)
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Intensities: ");
    for (int i = 0; i < 4; ++i)
    {
        lcd.print(intensities[i]);
        lcd.print("% ");
    }

    lcd.setCursor(0, 1);
    lcd.print("Optimal Position: ");
    if (optimalPosition == 0)
        lcd.print("East");
    else if (optimalPosition == 1)
        lcd.print("West");
    else if (optimalPosition == 2)
        lcd.print("North");
    else
        lcd.print("South");
}
