#include <Arduino.h>
/*
  Restaurant Order System - DOIT ESP32 DEVKIT V1
  Circular queue, no LCD. Controlled by:
    1) Physical push buttons
    2) Web page buttons (phone/PC on the same WiFi)
  Both paths call the same logic functions.
*/

#include <WiFi.h>
#include <WebServer.h>

// ---------- WiFi credentials (EDIT THESE) ----------
const char* WIFI_SSID = "Wifi_Name";
const char* WIFI_PASS = "Wifi_Pass";

// ---------- Pin assignments (DOIT ESP32 DEVKIT V1) ----------
// All button pins use the internal pull-up, so each button just goes to GND.
#define BTN_PIZZA   13
#define BTN_BURGER  14
#define BTN_PASTA   27
#define BTN_PROCESS 26
#define BTN_DONE    25

#define LED_WAITING 16
#define LED_READY   17
#define BUZZER      18   // passive piezo buzzer

// ---------- Circular queue state ----------
const int MAX_ORDERS = 10;
int    orderNumbers[MAX_ORDERS];
String orderItems[MAX_ORDERS];
int    front = 0;
int    rear = 0;
int    orderCount = 0;
int    nextOrderNumber = 1;

bool   preparing = false;
int    currentNumber = 0;
String currentItem = "";
String statusMessage = "System ready";

WebServer server(80);

// ---------- Physical button handling (non-blocking debounce) ----------
const int NUM_BTNS = 5;
const int btnPins[NUM_BTNS]   = { BTN_PIZZA, BTN_BURGER, BTN_PASTA, BTN_PROCESS, BTN_DONE };
int       lastBtnState[NUM_BTNS];
unsigned long lastBtnTime[NUM_BTNS] = { 0, 0, 0, 0, 0 };
const unsigned long DEBOUNCE_MS = 200;

// ---------- Queue helpers ----------
bool isFull()  { return orderCount == MAX_ORDERS; }
bool isEmpty() { return orderCount == 0; }

void beep(int frequency, int duration) {
  tone(BUZZER, frequency);
  delay(duration);
  noTone(BUZZER);
}

// ---------- Core logic (LCD output replaced by statusMessage) ----------
void addOrder(String item) {
  if (isFull()) {
    statusMessage = "Queue full!";
    beep(500, 250);
    return;
  }
  orderNumbers[rear] = nextOrderNumber;
  orderItems[rear]   = item;
  rear = (rear + 1) % MAX_ORDERS;
  orderCount++;#include <Arduino.h>
/*
  Restaurant Order System - DOIT ESP32 DEVKIT V1
  Circular queue, no LCD. Controlled by:
    1) Physical push buttons
    2) Web page buttons (phone/PC on the same WiFi)
  Both paths call the same logic functions.
*/

#include <WiFi.h>
#include <WebServer.h>

// ---------- WiFi credentials (EDIT THESE) ----------
const char* WIFI_SSID = "Q=mcθ";
const char* WIFI_PASS = "NailIrfan4406";

// ---------- Pin assignments (DOIT ESP32 DEVKIT V1) ----------
// All button pins use the internal pull-up, so each button just goes to GND.
#define BTN_PIZZA   13
#define BTN_BURGER  14
#define BTN_PASTA   27
#define BTN_PROCESS 26
#define BTN_DONE    25

#define LED_WAITING 16
#define LED_READY   17
#define BUZZER      18   // passive piezo buzzer

// ---------- Circular queue state ----------
const int MAX_ORDERS = 10;
int    orderNumbers[MAX_ORDERS];
String orderItems[MAX_ORDERS];
int    front = 0;
int    rear = 0;
int    orderCount = 0;
int    nextOrderNumber = 1;

bool   preparing = false;
int    currentNumber = 0;
String currentItem = "";
String statusMessage = "System ready";

WebServer server(80);

// ---------- Physical button handling (non-blocking debounce) ----------
const int NUM_BTNS = 5;
const int btnPins[NUM_BTNS]   = { BTN_PIZZA, BTN_BURGER, BTN_PASTA, BTN_PROCESS, BTN_DONE };
int       lastBtnState[NUM_BTNS];
unsigned long lastBtnTime[NUM_BTNS] = { 0, 0, 0, 0, 0 };
const unsigned long DEBOUNCE_MS = 200;

// ---------- Queue helpers ----------
bool isFull()  { return orderCount == MAX_ORDERS; }
bool isEmpty() { return orderCount == 0; }

void beep(int frequency, int duration) {
  tone(BUZZER, frequency);
  delay(duration);
  noTone(BUZZER);
}

// ---------- Core logic (LCD output replaced by statusMessage) ----------
void addOrder(String item) {
  if (isFull()) {
    statusMessage = "Queue full!";
    beep(500, 250);
    return;
  }
  orderNumbers[rear] = nextOrderNumber;
  orderItems[rear]   = item;
  rear = (rear + 1) % MAX_ORDERS;
  orderCount++;

  statusMessage = "Added #" + String(nextOrderNumber) + " " + item;
  nextOrderNumber++;

  digitalWrite(LED_WAITING, HIGH);
  beep(1000, 120);
}

void processOrder() {
  if (preparing) {
    statusMessage = "Still cooking #" + String(currentNumber) + " - press DONE";
    return;
  }
  if (isEmpty()) {
    statusMessage = "No orders to process";
    return;
  }
  currentNumber = orderNumbers[front];
  currentItem   = orderItems[front];
  front = (front + 1) % MAX_ORDERS;
  orderCount--;

  if (isEmpty()) {
    digitalWrite(LED_WAITING, LOW);
  }
  preparing = true;
  statusMessage = "Cooking #" + String(currentNumber) + " " + currentItem;
}

void finishOrder() {
  if (!preparing) {
    statusMessage = "No order is preparing";
    return;
  }
  statusMessage = "Order #" + String(currentNumber) + " READY!";
  digitalWrite(LED_READY, HIGH);
  beep(2000, 200);
  delay(120);
  beep(2000, 200);
  digitalWrite(LED_READY, LOW);

  preparing = false;
  currentNumber = 0;
  currentItem = "";
}

// ---------- Build JSON status for the web page ----------
String buildStatusJson() {
  String json = "{";
  json += "\"preparing\":" + String(preparing ? "true" : "false") + ",";
  json += "\"currentNumber\":" + String(currentNumber) + ",";
  json += "\"currentItem\":\"" + currentItem + "\",";
  json += "\"orderCount\":" + String(orderCount) + ",";
  json += "\"message\":\"" + statusMessage + "\",";
  json += "\"queue\":[";
  for (int i = 0; i < orderCount; i++) {
    int idx = (front + i) % MAX_ORDERS;
    if (i > 0) json += ",";
    json += "{\"num\":" + String(orderNumbers[idx]) + ",\"item\":\"" + orderItems[idx] + "\"}";
  }
  json += "]}";
  return json;
}

// ---------- Web page ----------
String buildHtml() {
  return R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Order System</title>
<style>
  :root{--bg:#0f1115;--card:#1b1f27;--accent:#ff7a18;--txt:#e8e8e8;--muted:#8a8f98;}
  *{box-sizing:border-box;}
  body{margin:0;font-family:system-ui,Segoe UI,Roboto,sans-serif;background:var(--bg);color:var(--txt);}
  .wrap{max-width:520px;margin:0 auto;padding:20px;}
  h1{font-size:20px;margin:0 0 4px;}
  .sub{color:var(--muted);font-size:13px;margin-bottom:18px;}
  .status{background:var(--card);border-radius:14px;padding:16px;margin-bottom:18px;}
  .msg{font-size:15px;margin-bottom:12px;font-weight:600;}
  .row{display:flex;justify-content:space-between;padding:6px 0;border-top:1px solid #262b34;font-size:14px;}
  .row span:first-child{color:var(--muted);}
  .grid{display:grid;grid-template-columns:1fr 1fr 1fr;gap:10px;margin-bottom:10px;}
  button{border:none;border-radius:12px;padding:16px 10px;font-size:15px;font-weight:600;color:#fff;cursor:pointer;}
  .food{background:#2a3340;}
  .proc{background:var(--accent);}
  .done{background:#1f9d55;}
  .full{width:100%;margin-bottom:10px;}
  .queue{margin-top:10px;}
  .qitem{background:#11151c;border-radius:8px;padding:8px 12px;margin-top:6px;font-size:13px;display:flex;justify-content:space-between;}
  .empty{color:var(--muted);font-size:13px;margin-top:8px;}
</style>
</head>
<body>
<div class="wrap">
  <h1>Restaurant Order System</h1>
  <div class="sub">ESP32 circular-queue demo</div>

  <div class="status">
    <div class="msg" id="msg">Loading...</div>
    <div class="row"><span>Cooking</span><span id="cooking">-</span></div>
    <div class="row"><span>In queue</span><span id="count">0</span></div>
    <div class="queue" id="queue"></div>
  </div>

  <div class="grid">
    <button class="food" onclick="send('/add?item=Pizza')">Pizza</button>
    <button class="food" onclick="send('/add?item=Burger')">Burger</button>
    <button class="food" onclick="send('/add?item=Pasta')">Pasta</button>
  </div>
  <button class="proc full" onclick="send('/process')">Process next order</button>
  <button class="done full" onclick="send('/done')">Mark current as DONE</button>
</div>

<script>
async function send(p){ try{ await fetch(p); } finally { refresh(); } }
async function refresh(){
  try{
    const s = await (await fetch('/status')).json();
    document.getElementById('msg').textContent = s.message;
    document.getElementById('cooking').textContent =
      s.preparing ? ('#' + s.currentNumber + ' ' + s.currentItem) : 'none';
    document.getElementById('count').textContent = s.orderCount;
    const q = document.getElementById('queue');
    if(s.queue.length === 0){ q.innerHTML = '<div class="empty">Queue empty</div>'; }
    else { q.innerHTML = s.queue.map(o =>
      '<div class="qitem"><span>#'+o.num+'</span><span>'+o.item+'</span></div>').join(''); }
  }catch(e){ document.getElementById('msg').textContent = 'Connection lost...'; }
}
setInterval(refresh, 1000);
refresh();
</script>
</body>
</html>
)rawliteral";
}

// ---------- Web handlers ----------
void handleRoot()    { server.send(200, "text/html", buildHtml()); }
void handleStatus()  { server.send(200, "application/json", buildStatusJson()); }
void handleProcess() { processOrder(); server.send(200, "text/plain", "ok"); }
void handleDone()    { finishOrder();  server.send(200, "text/plain", "ok"); }
void handleAdd() {
  String item = server.arg("item");
  if (item == "Pizza" || item == "Burger" || item == "Pasta") addOrder(item);
  server.send(200, "text/plain", "ok");
}

// ---------- Physical buttons ----------
void triggerButton(int i) {
  if (i == 0) addOrder("Pizza");
  else if (i == 1) addOrder("Burger");
  else if (i == 2) addOrder("Pasta");
  else if (i == 3) processOrder();
  else if (i == 4) finishOrder();
}
#include <Arduino.h>
/*
  Restaurant Order System - DOIT ESP32 DEVKIT V1
  Circular queue, no LCD. Controlled by:
    1) Physical push buttons
    2) Web page buttons (phone/PC on the same WiFi)
  Both paths call the same logic functions.
*/

#include <WiFi.h>
#include <WebServer.h>

// ---------- WiFi credentials (EDIT THESE) ----------
const char* WIFI_SSID = "Q=mcθ";
const char* WIFI_PASS = "NailIrfan4406";

// ---------- Pin assignments (DOIT ESP32 DEVKIT V1) ----------
// All button pins use the internal pull-up, so each button just goes to GND.
#define BTN_PIZZA   13
#define BTN_BURGER  14
#define BTN_PASTA   27
#define BTN_PROCESS 26
#define BTN_DONE    25

#define LED_WAITING 16
#define LED_READY   17
#define BUZZER      18   // passive piezo buzzer

// ---------- Circular queue state ----------
const int MAX_ORDERS = 10;
int    orderNumbers[MAX_ORDERS];
String orderItems[MAX_ORDERS];
int    front = 0;
int    rear = 0;
int    orderCount = 0;
int    nextOrderNumber = 1;

bool   preparing = false;
int    currentNumber = 0;
String currentItem = "";
String statusMessage = "System ready";

WebServer server(80);

// ---------- Physical button handling (non-blocking debounce) ----------
const int NUM_BTNS = 5;
const int btnPins[NUM_BTNS]   = { BTN_PIZZA, BTN_BURGER, BTN_PASTA, BTN_PROCESS, BTN_DONE };
int       lastBtnState[NUM_BTNS];
unsigned long lastBtnTime[NUM_BTNS] = { 0, 0, 0, 0, 0 };
const unsigned long DEBOUNCE_MS = 200;

// ---------- Queue helpers ----------
bool isFull()  { return orderCount == MAX_ORDERS; }
bool isEmpty() { return orderCount == 0; }

void beep(int frequency, int duration) {
  tone(BUZZER, frequency);
  delay(duration);
  noTone(BUZZER);
}

// ---------- Core logic (LCD output replaced by statusMessage) ----------
void addOrder(String item) {
  if (isFull()) {
    statusMessage = "Queue full!";
    beep(500, 250);
    return;
  }
  orderNumbers[rear] = nextOrderNumber;
  orderItems[rear]   = item;
  rear = (rear + 1) % MAX_ORDERS;
  orderCount++;

  statusMessage = "Added #" + String(nextOrderNumber) + " " + item;
  nextOrderNumber++;

  digitalWrite(LED_WAITING, HIGH);
  beep(1000, 120);
}

void processOrder() {
  if (preparing) {
    statusMessage = "Still cooking #" + String(currentNumber) + " - press DONE";
    return;
  }
  if (isEmpty()) {
    statusMessage = "No orders to process";
    return;
  }
  currentNumber = orderNumbers[front];
  currentItem   = orderItems[front];
  front = (front + 1) % MAX_ORDERS;
  orderCount--;

  if (isEmpty()) {
    digitalWrite(LED_WAITING, LOW);
  }
  preparing = true;
  statusMessage = "Cooking #" + String(currentNumber) + " " + currentItem;
}

void finishOrder() {
  if (!preparing) {
    statusMessage = "No order is preparing";
    return;
  }
  statusMessage = "Order #" + String(currentNumber) + " READY!";
  digitalWrite(LED_READY, HIGH);
  beep(2000, 200);
  delay(120);
  beep(2000, 200);
  digitalWrite(LED_READY, LOW);

  preparing = false;
  currentNumber = 0;
  currentItem = "";
}

// ---------- Build JSON status for the web page ----------
String buildStatusJson() {
  String json = "{";
  json += "\"preparing\":" + String(preparing ? "true" : "false") + ",";
  json += "\"currentNumber\":" + String(currentNumber) + ",";
  json += "\"currentItem\":\"" + currentItem + "\",";
  json += "\"orderCount\":" + String(orderCount) + ",";
  json += "\"message\":\"" + statusMessage + "\",";
  json += "\"queue\":[";
  for (int i = 0; i < orderCount; i++) {
    int idx = (front + i) % MAX_ORDERS;
    if (i > 0) json += ",";
    json += "{\"num\":" + String(orderNumbers[idx]) + ",\"item\":\"" + orderItems[idx] + "\"}";
  }
  json += "]}";
  return json;
}

// ---------- Web page ----------
String buildHtml() {
  return R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Order System</title>
<style>
  :root{--bg:#0f1115;--card:#1b1f27;--accent:#ff7a18;--txt:#e8e8e8;--muted:#8a8f98;}
  *{box-sizing:border-box;}
  body{margin:0;font-family:system-ui,Segoe UI,Roboto,sans-serif;background:var(--bg);color:var(--txt);}
  .wrap{max-width:520px;margin:0 auto;padding:20px;}
  h1{font-size:20px;margin:0 0 4px;}
  .sub{color:var(--muted);font-size:13px;margin-bottom:18px;}
  .status{background:var(--card);border-radius:14px;padding:16px;margin-bottom:18px;}
  .msg{font-size:15px;margin-bottom:12px;font-weight:600;}
  .row{display:flex;justify-content:space-between;padding:6px 0;border-top:1px solid #262b34;font-size:14px;}
  .row span:first-child{color:var(--muted);}
  .grid{display:grid;grid-template-columns:1fr 1fr 1fr;gap:10px;margin-bottom:10px;}
  button{border:none;border-radius:12px;padding:16px 10px;font-size:15px;font-weight:600;color:#fff;cursor:pointer;}
  .food{background:#2a3340;}
  .proc{background:var(--accent);}
  .done{background:#1f9d55;}
  .full{width:100%;margin-bottom:10px;}
  .queue{margin-top:10px;}
  .qitem{background:#11151c;border-radius:8px;padding:8px 12px;margin-top:6px;font-size:13px;display:flex;justify-content:space-between;}
  .empty{color:var(--muted);font-size:13px;margin-top:8px;}
</style>
</head>
<body>
<div class="wrap">
  <h1>Restaurant Order System</h1>
  <div class="sub">ESP32 circular-queue demo</div>

  <div class="status">
    <div class="msg" id="msg">Loading...</div>
    <div class="row"><span>Cooking</span><span id="cooking">-</span></div>
    <div class="row"><span>In queue</span><span id="count">0</span></div>
    <div class="queue" id="queue"></div>
  </div>

  <div class="grid">
    <button class="food" onclick="send('/add?item=Pizza')">Pizza</button>
    <button class="food" onclick="send('/add?item=Burger')">Burger</button>
    <button class="food" onclick="send('/add?item=Pasta')">Pasta</button>
  </div>
  <button class="proc full" onclick="send('/process')">Process next order</button>
  <button class="done full" onclick="send('/done')">Mark current as DONE</button>
</div>

<script>
async function send(p){ try{ await fetch(p); } finally { refresh(); } }
async function refresh(){
  try{
    const s = await (await fetch('/status')).json();
    document.getElementById('msg').textContent = s.message;
    document.getElementById('cooking').textContent =
      s.preparing ? ('#' + s.currentNumber + ' ' + s.currentItem) : 'none';
    document.getElementById('count').textContent = s.orderCount;
    const q = document.getElementById('queue');
    if(s.queue.length === 0){ q.innerHTML = '<div class="empty">Queue empty</div>'; }
    else { q.innerHTML = s.queue.map(o =>
      '<div class="qitem"><span>#'+o.num+'</span><span>'+o.item+'</span></div>').join(''); }
  }catch(e){ document.getElementById('msg').textContent = 'Connection lost...'; }
}
setInterval(refresh, 1000);
refresh();
</script>
</body>
</html>
)rawliteral";
}

// ---------- Web handlers ----------
void handleRoot()    { server.send(200, "text/html", buildHtml()); }
void handleStatus()  { server.send(200, "application/json", buildStatusJson()); }
void handleProcess() { processOrder(); server.send(200, "text/plain", "ok"); }
void handleDone()    { finishOrder();  server.send(200, "text/plain", "ok"); }
void handleAdd() {
  String item = server.arg("item");
  if (item == "Pizza" || item == "Burger" || item == "Pasta") addOrder(item);
  server.send(200, "text/plain", "ok");
}

// ---------- Physical buttons ----------
void triggerButton(int i) {
  if (i == 0) addOrder("Pizza");
  else if (i == 1) addOrder("Burger");
  else if (i == 2) addOrder("Pasta");
  else if (i == 3) processOrder();
  else if (i == 4) finishOrder();
}

void handleButtons() {
  unsigned long now = millis();
  for (int i = 0; i < NUM_BTNS; i++) {
    int reading = digitalRead(btnPins[i]);
    if (reading == LOW && lastBtnState[i] == HIGH && (now - lastBtnTime[i]) > DEBOUNCE_MS) {
      lastBtnTime[i] = now;
      triggerButton(i);
    }
    lastBtnState[i] = reading;
  }
}

void setup() {
  Serial.begin(115200);

  for (int i = 0; i < NUM_BTNS; i++) {
    pinMode(btnPins[i], INPUT_PULLUP);
    lastBtnState[i] = HIGH;
  }
  pinMode(LED_WAITING, OUTPUT);
  pinMode(LED_READY, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  digitalWrite(LED_WAITING, LOW);
  digitalWrite(LED_READY, LOW);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected. Open this in a browser: http://");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/add", handleAdd);
  server.on("/process", handleProcess);
  server.on("/done", handleDone);
  server.on("/status", handleStatus);
  server.begin();
}

void loop() {
  server.handleClient();
  handleButtons();
}
void handleButtons() {
  unsigned long now = millis();
  for (int i = 0; i < NUM_BTNS; i++) {
    int reading = digitalRead(btnPins[i]);
    if (reading == LOW && lastBtnState[i] == HIGH && (now - lastBtnTime[i]) > DEBOUNCE_MS) {
      lastBtnTime[i] = now;
      triggerButton(i);
    }
    lastBtnState[i] = reading;
  }
}

void setup() {
  Serial.begin(115200);

  for (int i = 0; i < NUM_BTNS; i++) {
    pinMode(btnPins[i], INPUT_PULLUP);
    lastBtnState[i] = HIGH;
  }
  pinMode(LED_WAITING, OUTPUT);
  pinMode(LED_READY, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  digitalWrite(LED_WAITING, LOW);
  digitalWrite(LED_READY, LOW);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected. Open this in a browser: http://");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/add", handleAdd);
  server.on("/process", handleProcess);
  server.on("/done", handleDone);
  server.on("/status", handleStatus);
  server.begin();
}

void loop() {
  server.handleClient();
  handleButtons();
}

  statusMessage = "Added #" + String(nextOrderNumber) + " " + item;
  nextOrderNumber++;

  digitalWrite(LED_WAITING, HIGH);
  beep(1000, 120);
}

void processOrder() {
  if (preparing) {
    statusMessage = "Still cooking #" + String(currentNumber) + " - press DONE";
    return;
  }
  if (isEmpty()) {
    statusMessage = "No orders to process";
    return;
  }
  currentNumber = orderNumbers[front];
  currentItem   = orderItems[front];
  front = (front + 1) % MAX_ORDERS;
  orderCount--;

  if (isEmpty()) {
    digitalWrite(LED_WAITING, LOW);
  }
  preparing = true;
  statusMessage = "Cooking #" + String(currentNumber) + " " + currentItem;
}

void finishOrder() {
  if (!preparing) {
    statusMessage = "No order is preparing";
    return;
  }
  statusMessage = "Order #" + String(currentNumber) + " READY!";
  digitalWrite(LED_READY, HIGH);
  beep(2000, 200);
  delay(120);
  beep(2000, 200);
  digitalWrite(LED_READY, LOW);

  preparing = false;
  currentNumber = 0;
  currentItem = "";
}

// ---------- Build JSON status for the web page ----------
String buildStatusJson() {
  String json = "{";
  json += "\"preparing\":" + String(preparing ? "true" : "false") + ",";
  json += "\"currentNumber\":" + String(currentNumber) + ",";
  json += "\"currentItem\":\"" + currentItem + "\",";
  json += "\"orderCount\":" + String(orderCount) + ",";
  json += "\"message\":\"" + statusMessage + "\",";
  json += "\"queue\":[";
  for (int i = 0; i < orderCount; i++) {
    int idx = (front + i) % MAX_ORDERS;
    if (i > 0) json += ",";
    json += "{\"num\":" + String(orderNumbers[idx]) + ",\"item\":\"" + orderItems[idx] + "\"}";
  }
  json += "]}";
  return json;
}

// ---------- Web page ----------
String buildHtml() {
  return R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Order System</title>
<style>
  :root{--bg:#0f1115;--card:#1b1f27;--accent:#ff7a18;--txt:#e8e8e8;--muted:#8a8f98;}
  *{box-sizing:border-box;}
  body{margin:0;font-family:system-ui,Segoe UI,Roboto,sans-serif;background:var(--bg);color:var(--txt);}
  .wrap{max-width:520px;margin:0 auto;padding:20px;}
  h1{font-size:20px;margin:0 0 4px;}
  .sub{color:var(--muted);font-size:13px;margin-bottom:18px;}
  .status{background:var(--card);border-radius:14px;padding:16px;margin-bottom:18px;}
  .msg{font-size:15px;margin-bottom:12px;font-weight:600;}
  .row{display:flex;justify-content:space-between;padding:6px 0;border-top:1px solid #262b34;font-size:14px;}
  .row span:first-child{color:var(--muted);}
  .grid{display:grid;grid-template-columns:1fr 1fr 1fr;gap:10px;margin-bottom:10px;}
  button{border:none;border-radius:12px;padding:16px 10px;font-size:15px;font-weight:600;color:#fff;cursor:pointer;}
  .food{background:#2a3340;}
  .proc{background:var(--accent);}
  .done{background:#1f9d55;}
  .full{width:100%;margin-bottom:10px;}
  .queue{margin-top:10px;}
  .qitem{background:#11151c;border-radius:8px;padding:8px 12px;margin-top:6px;font-size:13px;display:flex;justify-content:space-between;}
  .empty{color:var(--muted);font-size:13px;margin-top:8px;}
</style>
</head>
<body>
<div class="wrap">
  <h1>Restaurant Order System</h1>
  <div class="sub">ESP32 circular-queue demo</div>

  <div class="status">
    <div class="msg" id="msg">Loading...</div>
    <div class="row"><span>Cooking</span><span id="cooking">-</span></div>
    <div class="row"><span>In queue</span><span id="count">0</span></div>
    <div class="queue" id="queue"></div>
  </div>

  <div class="grid">
    <button class="food" onclick="send('/add?item=Pizza')">Pizza</button>
    <button class="food" onclick="send('/add?item=Burger')">Burger</button>
    <button class="food" onclick="send('/add?item=Pasta')">Pasta</button>
  </div>
  <button class="proc full" onclick="send('/process')">Process next order</button>
  <button class="done full" onclick="send('/done')">Mark current as DONE</button>
</div>

<script>
async function send(p){ try{ await fetch(p); } finally { refresh(); } }
async function refresh(){
  try{
    const s = await (await fetch('/status')).json();
    document.getElementById('msg').textContent = s.message;
    document.getElementById('cooking').textContent =
      s.preparing ? ('#' + s.currentNumber + ' ' + s.currentItem) : 'none';
    document.getElementById('count').textContent = s.orderCount;
    const q = document.getElementById('queue');
    if(s.queue.length === 0){ q.innerHTML = '<div class="empty">Queue empty</div>'; }
    else { q.innerHTML = s.queue.map(o =>
      '<div class="qitem"><span>#'+o.num+'</span><span>'+o.item+'</span></div>').join(''); }
  }catch(e){ document.getElementById('msg').textContent = 'Connection lost...'; }
}
setInterval(refresh, 1000);
refresh();
</script>
</body>
</html>
)rawliteral";
}

// ---------- Web handlers ----------
void handleRoot()    { server.send(200, "text/html", buildHtml()); }
void handleStatus()  { server.send(200, "application/json", buildStatusJson()); }
void handleProcess() { processOrder(); server.send(200, "text/plain", "ok"); }
void handleDone()    { finishOrder();  server.send(200, "text/plain", "ok"); }
void handleAdd() {
  String item = server.arg("item");
  if (item == "Pizza" || item == "Burger" || item == "Pasta") addOrder(item);
  server.send(200, "text/plain", "ok");
}

// ---------- Physical buttons ----------
void triggerButton(int i) {
  if (i == 0) addOrder("Pizza");
  else if (i == 1) addOrder("Burger");
  else if (i == 2) addOrder("Pasta");
  else if (i == 3) processOrder();
  else if (i == 4) finishOrder();
}

void handleButtons() {
  unsigned long now = millis();
  for (int i = 0; i < NUM_BTNS; i++) {
    int reading = digitalRead(btnPins[i]);
    if (reading == LOW && lastBtnState[i] == HIGH && (now - lastBtnTime[i]) > DEBOUNCE_MS) {
      lastBtnTime[i] = now;
      triggerButton(i);
    }
    lastBtnState[i] = reading;
  }
}

void setup() {
  Serial.begin(115200);

  for (int i = 0; i < NUM_BTNS; i++) {
    pinMode(btnPins[i], INPUT_PULLUP);
    lastBtnState[i] = HIGH;
  }
  pinMode(LED_WAITING, OUTPUT);
  pinMode(LED_READY, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  digitalWrite(LED_WAITING, LOW);
  digitalWrite(LED_READY, LOW);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected. Open this in a browser: http://");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/add", handleAdd);
  server.on("/process", handleProcess);
  server.on("/done", handleDone);
  server.on("/status", handleStatus);
  server.begin();
}

void loop() {
  server.handleClient();
  handleButtons();
}
