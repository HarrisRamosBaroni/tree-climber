const int trigPin = 5;
const int echoPin = 18;

// Define sound speed in cm/uS
#define SOUND_SPEED 0.034

const int samples = 1000;   // Total number of samples
const int window_min = 350; // Hypothesized period length
const int window_max = 500; // falls between window
const int window_incr = 10;

long duration;
float distanceCm;
float samplesArray[samples];

void setup() {
  Serial.begin(115200);     // Starts the serial communication
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT);  // Sets the echoPin as an Input
}

void loop() {
  captureSamples();

  int estimated_period = estimatePeriod();
  Serial.print("Estimated Period: ");
  Serial.println(estimated_period);
}

// Obtain distance profile using the ultrasonic sensor. 1000 samples * 50ms delay = 50s for full scan
void captureSamples() {
  for (int i = 0; i < samples; ++i) {
    // Clears the trigPin
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echoPin, HIGH);

    // Calculate the distance
    distanceCm = duration * SOUND_SPEED / 2;

    // Store the distance in the array
    samplesArray[i] = distanceCm;
    delay(50); // Delay between each sample
  }
}

// Estimate the period of a scan using the computed samples
int estimatePeriod() {
  float dissimilarities[window_max / window_incr];
  float first_window[window_min];

  // Create the first window
  for (int i = 0; i < window_min; ++i) {
    first_window[i] = samplesArray[i];
  }

  // Calculate dissimilarities for different window sizes
  for (int win_size = window_min, idx = 0; win_size < window_max; win_size += window_incr, ++idx) {
    if (2 * win_size > samples)
      break;

    float current_window[win_size];
    for (int i = 0; i < win_size; ++i) {
      current_window[i] = samplesArray[i + win_size];
    }

    float sum = 0.0;
    for (int i = 0; i < win_size; ++i) {
      sum += pow(first_window[i] - current_window[i], 2);
    }
    dissimilarities[idx] = sum / win_size; // Mean squared error
  }

  // Obtain the smallest dissimilarity (smallest mse)
  float min_dissimilarity = dissimilarities[0];
  int min_dissimilarity_idx = 0;
  for (int i = 1; i < window_max / window_incr; ++i) {
    if (dissimilarities[i] < min_dissimilarity) {
      min_dissimilarity = dissimilarities[i];
      min_dissimilarity_idx = i;
    }
  }
  return window_min + min_dissimilarity_idx * window_incr;
}
