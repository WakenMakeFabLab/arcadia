#define _STATES 150

int lastState = 0;
double acc = 0;

void setup() {
  pinMode(A0, INPUT);
  Serial.begin(9600);
 
}

void muu() {
  for (int i = 0; i < 30; i++) {
    Serial.println("MUUUUUUUUUUU");
  }
}

void loop() {
  acc = 0;
  int stateHistory[_STATES];
  for (int i = 0; i < _STATES; i++) {
    stateHistory[i] = analogRead(A0);
    delay(2);
  }
  for (int i = 0; i < _STATES; i++) {
    acc += stateHistory[i];
  }
  acc /= _STATES;

  if (acc < 1010) {
    lastState = 1;
    Serial.print(acc);
    Serial.print("\t");
    Serial.print("Son dritto!\t");
    Serial.println(lastState);
  } else {
    Serial.print(acc);
    Serial.print("\t");
    Serial.print("Son ribaltato!\t");
    Serial.println(lastState);
    if (lastState != 0) {
      muu();
    }
    lastState = 0;
  }
  delay(1000);
}
