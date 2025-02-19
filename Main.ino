#include <arduinoFFT.h> 

const int outputMic = A0;  // Microphone pin
const int led1 = 2;        
const int led2 = 3;       
const int led3 = 4;       
const int samples = 128;    // Number of samples 
const double samplingFrequency = 10000; // Sampling frequency in Hz
const double dcOffset = 1.7;  // DC offset value (1.7V when silence)

double vReal[samples]; // Real part of the FFT input
double vImag[samples]; // Imaginary part 

ArduinoFFT FFT = ArduinoFFT(vReal, vImag, samples, samplingFrequency);


bool led1State = false;
bool led2State = false;
bool led3State = false;

void setup() {
  Serial.begin(9600);
  pinMode(led1, OUTPUT); 
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
}

void loop() {
  // Collect samples from the microphone
  for (int i = 0; i < samples; i++) {
    int readMic = analogRead(outputMic);    // Read microphone
    vReal[i] = (5.0 * readMic / 1023.0) - dcOffset;  // Convert to voltage and subtract DC offset
    vImag[i] = 0;                           // Imaginary part is 0
    delayMicroseconds(1000000 / samplingFrequency); // Wait e
  }

  // FFT
  FFT.windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);  // Apply a window function
  FFT.compute(FFT_FORWARD);                        // Compute the FFT
  FFT.complexToMagnitude();                        // Compute magnitudes

  double scaleFactor = 100;  // scale data
  double maxMagnitude = 0;   // store maximum magnitude
  int maxIndex = 0;         // store index of maximum frequency

  // Find the frequency with the highest magnitude
  for (int i = 0; i < (samples / 2); i++) {
    if (vReal[i] > maxMagnitude) {
      maxMagnitude = vReal[i];
      maxIndex = i;
    }
  }

  // Calculate the frequency corresponding to the peak magnitude
  double frequency = (maxIndex * samplingFrequency) / samples;

  // Print frequency and max magnitude 
  Serial.print("Frequency: ");
  Serial.print(frequency, 1);
  Serial.print(" Hz, Max Magnitude: ");
  Serial.println(maxMagnitude, 6);

  // Toggle LEDs based on frequency range
  if (frequency >= 50 && frequency < 500) {
    // LED1
    led1State = !led1State;  // Toggle the state of LED1
    digitalWrite(led1, led1State ? HIGH : LOW);  // Set LED1 based on the state
    digitalWrite(led2, LOW);  // Turn off LED2
    digitalWrite(led3, LOW);  // Turn off LED3
  } 
  else if (frequency >= 500 && frequency < 700) {
    // LED2 
    led2State = !led2State;  // Toggle the state of LED2
    digitalWrite(led2, led2State ? HIGH : LOW);  // Set LED2 based on the state
    digitalWrite(led1, LOW);  // Turn off LED1
    digitalWrite(led3, LOW);  // Turn off LED3
  } 
  else if (frequency >= 700 && frequency < 1500) {
    // LED3 
    led3State = !led3State;  // Toggle the state of LED3
    digitalWrite(led3, led3State ? HIGH : LOW);  // Set LED3 based on the state
    digitalWrite(led1, LOW);  // Turn off LED1
    digitalWrite(led2, LOW);  // Turn off LED2
  } 
  else {
    // If the frequency is outside the defined ranges, turn off all LEDs
    digitalWrite(led1, LOW);
    digitalWrite(led2, LOW);
    digitalWrite(led3, LOW);
  }

  delay(100); // Short delay between measurements
}
