#include <EEPROM.h>
#include <Keyboard.h>

#define TURBIDITY 0
#define PH 1
#define CONDUCTIVITY 2
#define DISSOLVED_OXYGEN 3
#define ORP 4
#define TDS 5

#define NUMBER_OF_SENSORS 6

#define TURBIDITY_URL "https://wiki.dfrobot.com/Gravity__Analog_Dissolved_Oxygen_Sensor_SKU_SEN0237"
#define PH_URL "https://wiki.dfrobot.com/PH_meter_SKU__SEN0161_"
#define CONDUCTIVITY_URL "https://wiki.dfrobot.com/Gravity__Analog_Electrical_Conductivity_Sensor___Meter_V2__K=1__SKU_DFR0300"
#define DISSOLVED_OXYGEN_URL "https://wiki.dfrobot.com/Gravity__Analog_Dissolved_Oxygen_Sensor_SKU_SEN0237"
#define ORP_URL "https://wiki.dfrobot.com/Analog_ORP_Meter_SKU_SEN0165_"
#define TDS_URL "https://wiki.dfrobot.com/Gravity__Analog_TDS_Sensor___Meter_For_Arduino_SKU__SEN0244"

#define JUMPER_PIN 3

#define TURBIDITY_JUMPER_IN 4
#define PH_JUMPER_IN 5
#define CONDUCTIVITY_JUMPER_IN 6
#define DISSOLVED_OXYGEN_JUMPER_IN 7
#define ORP_JUMPER_IN 8
#define TDS_JUMPER_IN 9

#define VALUE_ARRAY_LENGTH 50
#define SENSOR_PIN A0
#define LED_PIN 2
#define ON_SWITCH_PIN 10


#define VOLTAGE 5.00    //system voltage
#define OFFSET 0        //zero drift voltage


const float SaturationValues[41] = {      //saturation dissolved oxygen concentrations at various temperatures
  14.46, 14.22, 13.82, 13.44, 13.09,
  12.74, 12.42, 12.11, 11.81, 11.53,
  11.26, 11.01, 10.77, 10.53, 10.30,
  10.08, 9.86,  9.66,  9.46,  9.27,
  9.08,  8.90,  8.73,  8.57,  8.41,
  8.25,  8.11,  7.96,  7.82,  7.69,
  7.56,  7.43,  7.30,  7.18,  7.07,
  6.95,  6.84,  6.73,  6.63,  6.53,
  6.41,
};

int sensorType;
float sensorOffset = 0;
int sensorValueArray[VALUE_ARRAY_LENGTH];
int sensorValueArrayIndex = 0;
int previousSensorType = 0;

float SaturationDoVoltage = 1127.6;   //default voltage:1127.6mv
float SaturationDoTemperature = 25.0;   //default temperature is 25^C

float timeAtLastBlink = 0;
int timeBetweenBlinks = 1000;
bool isLightOn = false;

float timeBetweenReadings = 20;
float timeAtLastRead = -1000;

float delayBetweenKeyboardInputs = 50;
float delayBetweenLines = 500;

float currentAverage = -1;
bool previousSwitchState = false;

void setup() {
  pinMode(JUMPER_PIN, OUTPUT);
  pinMode(TDS_JUMPER_IN, INPUT_PULLUP);
  pinMode(TURBIDITY_JUMPER_IN, INPUT_PULLUP);
  pinMode(PH_JUMPER_IN, INPUT_PULLUP);
  pinMode(CONDUCTIVITY_JUMPER_IN, INPUT_PULLUP);
  pinMode(DISSOLVED_OXYGEN_JUMPER_IN, INPUT_PULLUP);
  pinMode(ORP_JUMPER_IN, INPUT_PULLUP);


  pinMode(ON_SWITCH_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  Keyboard.begin();
  Serial.begin(9600); //Baud rate: 9600
  sensorType = getSensorType();

  switch (sensorType) {
    case TURBIDITY: {
        setup_turbidity();
        break;
      }
    case PH: {
        setup_pH();
        break;
      }
    case CONDUCTIVITY: {
        setup_conductivity();
        break;
      }
    case DISSOLVED_OXYGEN: {
        setup_dissolved_oxygen();
        break;
      }
    case ORP: {
        setup_orp();
        break;
      }
    case TDS: {
        setup_tds();
        break;
      }
  }
  delay(1000);
}

int getSensorType() {
  digitalWrite(JUMPER_PIN, LOW);
  for (int pinNumber = TURBIDITY_JUMPER_IN; pinNumber < TURBIDITY_JUMPER_IN + NUMBER_OF_SENSORS + 1; pinNumber++) {
    int jumper_read_value = digitalRead(pinNumber);
    if (jumper_read_value == LOW) {
      Serial.print("READ HIGH WITH sensorType = ");
      Serial.println(pinNumber - TURBIDITY_JUMPER_IN);
      return (pinNumber - TURBIDITY_JUMPER_IN);
    }
  }
  return 0;
}

void setup_tds(){
  Serial.println("TDS Sensor.");
  Serial.println("This function is a placeholder for any ORP sensor-specific setup code.");
  
}

void setup_turbidity() {
  Serial.println("Turbidity Sensor.");
  Serial.println("This function is a placeholder for any ORP sensor-specific setup code.");
}

void setup_conductivity() {
  Serial.println("Electrical Conductivity Sensor.");
  Serial.println("This function is a placeholder for any EC sensor-specific setup code.");
}

void setup_dissolved_oxygen() {
  Serial.println("Dissolved Oxygen Sensor.");
  Serial.println("This function is a placeholder for any DO sensor-specific setup code.");
}

void setup_orp() {
  Serial.println("ORP Sensor.");
  Serial.println("This function is a placeholder for any ORP sensor-specific setup code.");
}

void setup_pH() {
  Serial.println("pH Sensor.");
  Serial.println("This function is a placeholder for any pH sensor-specific setup code.");
}


void loop() {
  takeSample();
  blinkLED();
}

void blinkLED() {
  float timeDelta = millis() - timeAtLastBlink;
  float timeBetweenBlinks = currentAverage;
  if (timeDelta > timeBetweenBlinks) {
    isLightOn = !isLightOn;
    digitalWrite(LED_PIN, isLightOn);
    timeAtLastBlink = millis();
  }
}

void outputStandardColumns(float voltage) {
  Keyboard.press(KEY_LEFT_CTRL);
  Keyboard.print(";");
  Keyboard.releaseAll();
  Keyboard.print("\t");
  Keyboard.press(KEY_LEFT_CTRL);
  Keyboard.print(":");
  Keyboard.releaseAll();
  Keyboard.print("\t");
  Keyboard.print("AnalogRead: \t");
  Keyboard.print(currentAverage);
  Keyboard.print("\t");
  Keyboard.print("Voltage: \t");
  Keyboard.print(voltage);
  Keyboard.print("\t");
}

void endLine() {
  Keyboard.releaseAll();
  delay(50);
  Keyboard.press(KEY_DOWN_ARROW);
  Keyboard.press(KEY_LEFT_CTRL);
  Keyboard.press(KEY_LEFT_ARROW);
  Keyboard.releaseAll();
  delay(50);

}

void output_TDS_columns(float voltage){
  float temperature = 25;
  float compensationCoefficient=1.0+0.02*(temperature-25.0);    //temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
  float compensationVolatge=voltage/compensationCoefficient;  //temperature compensation
  float tdsValue=(133.42*compensationVolatge*compensationVolatge*compensationVolatge - 255.86*compensationVolatge*compensationVolatge + 857.39*compensationVolatge)*0.5; //convert voltage value to tds value
  Keyboard.print("TDS: \t");
  Keyboard.print(tdsValue);
  Keyboard.print("\t");   
}

void output_pH_columns(float voltage) {
  Keyboard.print("pH: \t");
  Keyboard.print(3.5 * voltage + OFFSET);
  Keyboard.print("\t");
}

void output_conductivity_columns(float voltage) {
  Keyboard.print("Conductivity: \t");
  Keyboard.print(getConductivityValue(voltage));
  Keyboard.print("\t");
}

void output_dissolved_oxygen_columns(float voltage) {
  Keyboard.print("Dissolved Oxygen: \t");
  Keyboard.print(get_DO_value(voltage));
  Keyboard.print("\t");
}

void output_ORP_columns(float analogReadValue) {
  Keyboard.print("ORP: \t");
  Keyboard.print(get_ORP_value(analogReadValue));
  Keyboard.print("\t");
}

void output_turbidity_columns(float analogReadValue) {
  Keyboard.print("Turbidity \t");
}

float get_ORP_value(float analogReadValue) {
  return (((30 * (double)VOLTAGE * 1000) - (75 * analogReadValue * VOLTAGE * 1000 / 1024)) / 75 - OFFSET);
}

float get_DO_value(float voltage) {
  float temperature = 25;
  float doValue = voltage * SaturationValues[(int)temperature] / SaturationDoVoltage;
  return doValue;
}

void outputSensorColumns(float voltage) {
  switch (sensorType) {
    case TURBIDITY: {
        output_turbidity_columns(voltage);
        break;
      }
    case PH: {
        output_pH_columns(voltage);
        break;
      }
    case CONDUCTIVITY: {
        output_conductivity_columns(voltage);
        break;
      }
    case DISSOLVED_OXYGEN: {
        output_dissolved_oxygen_columns(voltage);
        break;
      }
    case ORP: {
        output_ORP_columns(currentAverage);
        break;
      }
    case TDS: {
        output_TDS_columns(voltage);
        break;
      }
  }
}

void beginLogging() {
  Keyboard.println("Beginning data-logging.");
  Keyboard.println("The data coming from this sensor-platform is uncalibrated.  It should only be used in comparison with other data from this sensor.");
  Keyboard.println("To calibrate this sensor you may need to upload new code to the Arduino.");
  Keyboard.println("For more information about using and calibrating this sensor see:");
  
  switch (sensorType) {
    case TURBIDITY: {
      Keyboard.println(TURBIDITY_URL);
        break;
      }
    case PH: {
      Keyboard.println(PH_URL);
        break;
      }
    case CONDUCTIVITY: {
      Keyboard.println(CONDUCTIVITY_URL);
        break;
      }
    case DISSOLVED_OXYGEN: {
      Keyboard.println(DISSOLVED_OXYGEN_URL);
        break;
      }
    case ORP: {      
      Keyboard.println(ORP_URL);
        break;
      }
    case TDS: {      
      Keyboard.println(TDS_URL);
        break;
      }
  }
}

void keyboardOutput() {
  previousSensorType = sensorType;
  sensorType = getSensorType();
  Serial.println("sensorType = ");
  Serial.println(sensorType);
  bool switch_state = digitalRead(ON_SWITCH_PIN);
  bool activateKeyboard = !switch_state;
  if (activateKeyboard) {
    if (previousSwitchState == true || sensorType != previousSensorType) {
      beginLogging();
    }
    float voltage = currentAverage / 1024 * 5.0f;
    outputStandardColumns(voltage);
    outputSensorColumns(voltage);
    endLine();
  }
  previousSwitchState = switch_state;
}

void takeSample() {
  unsigned long currentTime = millis();
  if (currentTime - timeAtLastRead > timeBetweenReadings) {
    sensorValueArray[sensorValueArrayIndex++] = analogRead(SENSOR_PIN);
    if (sensorValueArrayIndex == VALUE_ARRAY_LENGTH) {
      currentAverage = averageArray(sensorValueArray, VALUE_ARRAY_LENGTH);
      sensorValueArrayIndex = 0;
      keyboardOutput();
    }
    timeAtLastRead = currentTime;

  }
}


double averageArray(int* arr, int number) {
  int i;
  int max, min;
  double avg;
  long amount = 0;
  if (number <= 0) {
    printf("Error number for the array to avraging!/n");
    return 0;
  }
  if (number < 5) { //less than 5, calculated directly statistics
    for (i = 0; i < number; i++) {
      amount += arr[i];
    }
    avg = amount / number;
    return avg;
  } else {
    if (arr[0] < arr[1]) {
      min = arr[0]; max = arr[1];
    }
    else {
      min = arr[1]; max = arr[0];
    }
    for (i = 2; i < number; i++) {
      if (arr[i] < min) {
        amount += min;      //arr<min
        min = arr[i];
      } else {
        if (arr[i] > max) {
          amount += max;  //arr>max
          max = arr[i];
        } else {
          amount += arr[i]; //min<=arr<=max
        }
      }//if
    }//for
    avg = (double)amount / (number - 2);
  }//if
  return avg;
}

float getConductivityValue(float voltage) {
  float temperature = 25.0;
  float TempCoefficient = 1.0 + 0.0185 * (temperature - 25.0); //temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.0185*(fTP-25.0));
  float ECcurrent = 1;
  float CoefficientVolatge = (float)voltage / TempCoefficient;
  if (CoefficientVolatge < 150) {
    Serial.println("No solution!");   //25^C 1413us/cm<-->about 216mv  if the voltage(compensate)<150,that is <1ms/cm,out of the range
  }
  else if (CoefficientVolatge > 3300) {
    Serial.println("Out of the range!");  //>20ms/cm,out of the range
  }
  else
  {
    if (CoefficientVolatge <= 448) {
      ECcurrent = 6.84 * CoefficientVolatge - 64.32; //1ms/cm<EC<=3ms/cm
    }
    else if (CoefficientVolatge <= 1457) {
      ECcurrent = 6.98 * CoefficientVolatge - 127; //3ms/cm<EC<=10ms/cm
    }
    else {
      ECcurrent = 5.3 * CoefficientVolatge + 2278;                     //10ms/cm<EC<20ms/cm
    }
    ECcurrent /= 1000;  //convert us/cm to ms/cm
    Serial.print(ECcurrent, 2); //two decimal
    Serial.println("ms/cm");
    return ECcurrent;
  }
}
