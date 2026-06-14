#include <Arduino.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(6, 7, 8, 9, A0, A1);

#define BTN_PIZZA 2
#define BTN_BURGER 3
#define BTN_PASTA 4
#define BTN_PROCESS 5
#define BTN_DONE 13

#define LED_WAITING 10
#define LED_READY 11
#define BUZZER 12

const int MAX_ORDERS = 10;

int orderNumbers[MAX_ORDERS];
String orderItems[MAX_ORDERS];

int front = 0;
int rear = 0;
int orderCount = 0;
int nextOrderNumber = 1;

bool preparing = false;
int currentNumber = 0;
String currentItem = "";

bool isFull() {
  return orderCount == MAX_ORDERS;
}

bool isEmpty() {
  return orderCount == 0;
}

void beep(int frequency, int duration) {
  tone(BUZZER, frequency);
  delay(duration);
  noTone(BUZZER);
}

void showHomeScreen() {
  lcd.clear();

  if (preparing) {
    lcd.print("Cooking:#");
    lcd.print(currentNumber);
    lcd.print(" ");
    lcd.print(currentItem);

    lcd.setCursor(0, 1);

    if (!isEmpty()) {
      lcd.print("Next:#");
      lcd.print(orderNumbers[front]);
      lcd.print(" ");
      lcd.print(orderItems[front]);
    } else {
      lcd.print("Next: none");
    }
  } 
  else {
    if (isEmpty()) {
      lcd.print("No orders");
      lcd.setCursor(0, 1);
      lcd.print("Press button");
    } else {
      lcd.print("Next:#");
      lcd.print(orderNumbers[front]);

      lcd.setCursor(0, 1);
      lcd.print(orderItems[front]);
      lcd.print(" Q:");
      lcd.print(orderCount);
    }
  }
}

void addOrder(String item) {
  if (isFull()) {
    lcd.clear();
    lcd.print("Queue Full!");
    beep(500, 300);
    delay(700);
    showHomeScreen();
    return;
  }

  orderNumbers[rear] = nextOrderNumber;
  orderItems[rear] = item;

  rear = (rear + 1) % MAX_ORDERS;
  orderCount++;
  nextOrderNumber++;

  digitalWrite(LED_WAITING, HIGH);

  lcd.clear();
  lcd.print("Added:");
  lcd.setCursor(0, 1);
  lcd.print(item);

  beep(1000, 150);
  delay(800);

  showHomeScreen();
}

void processOrder() {
  if (preparing) {
    lcd.clear();
    lcd.print("Still cooking");
    lcd.setCursor(0, 1);
    lcd.print("Press DONE");
    delay(1000);
    showHomeScreen();
    return;
  }

  if (isEmpty()) {
    lcd.clear();
    lcd.print("No orders");
    delay(1000);
    showHomeScreen();
    return;
  }

  currentNumber = orderNumbers[front];
  currentItem = orderItems[front];

  front = (front + 1) % MAX_ORDERS;
  orderCount--;

  if (isEmpty()) {
    digitalWrite(LED_WAITING, LOW);
  }

  preparing = true;

  showHomeScreen();
}

void finishOrder() {
  if (!preparing) {
    lcd.clear();
    lcd.print("No order is");
    lcd.setCursor(0, 1);
    lcd.print("preparing");
    delay(1000);
    showHomeScreen();
    return;
  }

  lcd.clear();
  lcd.print("Order #");
  lcd.print(currentNumber);
  lcd.setCursor(0, 1);
  lcd.print("READY!");

  digitalWrite(LED_READY, HIGH);

  beep(2000, 300);
  delay(200);
  beep(2000, 300);

  delay(1500);

  digitalWrite(LED_READY, LOW);

  preparing = false;
  currentNumber = 0;
  currentItem = "";

  showHomeScreen();
}

void setup() {
  lcd.begin(16, 2);

  pinMode(BTN_PIZZA, INPUT_PULLUP);
  pinMode(BTN_BURGER, INPUT_PULLUP);
  pinMode(BTN_PASTA, INPUT_PULLUP);
  pinMode(BTN_PROCESS, INPUT_PULLUP);
  pinMode(BTN_DONE, INPUT_PULLUP);

  pinMode(LED_WAITING, OUTPUT);
  pinMode(LED_READY, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  digitalWrite(LED_WAITING, LOW);
  digitalWrite(LED_READY, LOW);

  lcd.print("Restaurant");
  lcd.setCursor(0, 1);
  lcd.print("Order System");

  delay(2000);

  showHomeScreen();
}

void loop() {
  if (digitalRead(BTN_PIZZA) == LOW) {
    addOrder("Pizza");
    delay(300);
  }

  if (digitalRead(BTN_BURGER) == LOW) {
    addOrder("Burger");
    delay(300);
  }

  if (digitalRead(BTN_PASTA) == LOW) {
    addOrder("Pasta");
    delay(300);
  }

  if (digitalRead(BTN_PROCESS) == LOW) {
    processOrder();
    delay(300);
  }

  if (digitalRead(BTN_DONE) == LOW) {
    finishOrder();
    delay(300);
  }
}
