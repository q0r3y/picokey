
int USAGE_COUNTER = 1;
const long DEVICE_ID = 486197;
const unsigned long SECRET_KEY = 3679243789;

void setup() {
  Serial.begin(115200);
}

void loop() {
  for (;;) {
    delay(2000);
    Serial.println("486197d7f1a6c30cc051");
  }
}
