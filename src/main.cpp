#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <Preferences.h>
#include <ESPmDNS.h>

// ════════════════════════════════════════════════════════════════════════════
// WEB PANEL HTML
// ════════════════════════════════════════════════════════════════════════════
const char WEB_PANEL_HTML[] = R"rawliteral(<!DOCTYPE html>
<html lang=en>
<head>
<meta charset=UTF-8>
<meta name=viewport content="width=device-width,initial-scale=1">
<title>Elegoo Monitor</title>
<style>
*{box-sizing:border-box;margin:0;padding:0}
body{font-family:Arial,sans-serif;background:#111;color:#e0e0e0;
     display:flex;justify-content:center;min-height:100vh;padding:16px}
.card{background:#1e1e1e;border-radius:4px;width:100%;max-width:460px;
      align-self:flex-start;margin-top:16px;overflow:hidden;border:1px solid #2a2a2a}
.brand{padding:14px 20px;background:#181818;border-bottom:1px solid #2a2a2a;
       display:flex;align-items:center;gap:10px}
.brand-logo{font-size:1rem;font-weight:700;color:#fff;letter-spacing:.05em}
.brand-logo span{color:#4caf50}
.brand-sub{font-size:.75rem;color:#666;margin-top:1px}
.tabs{display:flex;background:#181818;border-bottom:1px solid #2a2a2a}
.tab{flex:1;padding:10px 4px;text-align:center;cursor:pointer;font-size:.75rem;
     color:#555;border-bottom:2px solid transparent;margin-bottom:-1px;transition:.15s}
.tab.active{color:#4caf50;border-bottom-color:#4caf50}
.tab:hover{color:#aaa}
.tab-icon{font-size:.95rem;display:block;margin-bottom:2px}
.panel{display:none;padding:16px}
.panel.active{display:block}
label{display:block;margin-bottom:4px;color:#888;font-size:.8rem}
input[type=text],input[type=password],input[type=number]{
  width:100%;padding:8px 10px;border:1px solid #333;border-radius:3px;
  background:#252525;color:#e0e0e0;font-size:.88rem;margin-bottom:8px;outline:none}
input:focus{border-color:#4caf50}
.hint{font-size:.72rem;color:#555;margin-bottom:10px}
.net-list{margin-bottom:8px;max-height:180px;overflow-y:auto;
          border:1px solid #2a2a2a;border-radius:3px;background:#161616}
.net-item{display:flex;align-items:center;padding:8px 12px;cursor:pointer;
          border-bottom:1px solid #222;transition:.1s}
.net-item:last-child{border-bottom:none}
.net-item:hover{background:#252525}
.net-item.selected{background:#1a2a1a;border-left:3px solid #4caf50}
.net-ssid{flex:1;font-size:.88rem;color:#e0e0e0}
.net-info{font-size:.72rem;color:#666;display:flex;gap:4px;align-items:center}
.manual-toggle{color:#4caf50;font-size:.78rem;cursor:pointer;margin-bottom:10px;
               text-decoration:none;display:block;text-align:right}
#manual-ssid{display:none}
.bar{display:inline-flex;gap:2px;align-items:flex-end;height:13px}
.bar span{width:3px;background:#333;border-radius:1px}
.bar span.on{background:#4caf50}
.btn{width:100%;padding:9px;border:none;border-radius:3px;font-size:.88rem;
     font-weight:bold;cursor:pointer;transition:.15s;margin-top:4px;letter-spacing:.03em}
.btn-primary{background:#4caf50;color:#000}
.btn-primary:hover{background:#43a047}
.btn-danger{background:#c62828;color:#fff}
.btn-danger:hover{background:#b71c1c}
.scanning{color:#555;font-size:.8rem;padding:10px;text-align:center}
.refresh{color:#4caf50;font-size:.75rem;cursor:pointer;float:right;text-decoration:none}
.msg{margin:8px 0 0;font-size:.82rem;border-radius:3px;padding:8px;text-align:center;display:none}
.tile{background:#252525;border-radius:3px;padding:12px;margin-bottom:8px;border:1px solid #2a2a2a}
.tile-row{display:flex;justify-content:space-between;align-items:center;margin-bottom:3px}
.tile-label{font-size:.75rem;color:#666;text-transform:uppercase;letter-spacing:.05em}
.tile-val{font-weight:bold;font-size:.95rem;color:#e0e0e0}
.big-pct{font-size:2.6rem;font-weight:700;color:#4caf50;letter-spacing:-1px;line-height:1}
.big-pct small{font-size:1rem;color:#444}
.hero{display:flex;align-items:center;gap:12px;margin-bottom:10px}
.fname{font-size:.82rem;color:#bbb;white-space:nowrap;overflow:hidden;
       text-overflow:ellipsis;max-width:240px;margin-bottom:3px}
.badge{font-size:.7rem;padding:2px 7px;border-radius:2px;background:#252525;
       color:#666;border:1px solid #333;text-transform:uppercase;letter-spacing:.05em}
.badge.printing{background:#1a2a1a;color:#4caf50;border-color:#4caf50}
.badge.idle{background:#1a1a2a;color:#64b5f6;border-color:#64b5f6}
.badge.complete{background:#2a2a1a;color:#ffb300;border-color:#ffb300}
.prog{height:3px;background:#2a2a2a;margin-bottom:12px}
.pf{height:100%;background:#4caf50;transition:width .8s;width:0}
.g2{display:grid;grid-template-columns:1fr 1fr;gap:8px;margin-bottom:8px}
.g3{display:grid;grid-template-columns:repeat(3,1fr);gap:8px}
.temp-box{background:#252525;border:1px solid #2a2a2a;border-radius:3px;padding:10px 12px}
.temp-box .lbl{font-size:.72rem;color:#666;text-transform:uppercase;letter-spacing:.05em;margin-bottom:4px}
.temp-box .val{font-size:1.1rem;font-weight:bold;color:#e0e0e0}
.temp-box .tgt{font-size:.7rem;color:#555;margin-top:2px}
.temp-box.hot .val{color:#ef5350}
.temp-box.warm .val{color:#ffb300}
.status-bar{background:#141414;padding:5px 14px;font-size:.7rem;color:#888;
            display:flex;justify-content:space-between;border-top:1px solid #222}
hr{border:none;border-top:1px solid #2a2a2a;margin:12px 0}
</style>
</head>
<body>
<div class=card>
  <div class=brand>
    <div>
      <div class=brand-logo>ELEG<span>OO</span> Monitor</div>
      <div class=brand-sub id=hdr>CC1 — ESP32-C3</div>
    </div>
  </div>

  <div class=tabs id=tabbar>
    <div class="tab active" id=t1 onclick="showTab(1)">
      <span class=tab-icon>&#9648;</span>Print
    </div>
    <div class=tab id=t2 onclick="showTab(2)">
      <span class=tab-icon>&#128246;</span>WiFi
    </div>
    <div class=tab id=t3 onclick="showTab(3)">
      <span class=tab-icon>&#9881;</span>Settings
    </div>
  </div>

  <!-- TAB 1: Print Status -->
  <div class="panel active" id=tab1>
    <div class=tile>
      <div class=hero>
        <div class=big-pct id=pc>–<small>%</small></div>
        <div style="flex:1;min-width:0">
          <div class=fname id=fn>No active print</div>
          <span class=badge id=badge>OFFLINE</span>
        </div>
      </div>
      <div class=prog><div class=pf id=pf></div></div>
      <div class=g2>
        <div>
          <div class=tile-label>Elapsed</div>
          <div class=tile-val id=te>--:--</div>
        </div>
        <div>
          <div class=tile-label>Remaining</div>
          <div class=tile-val id=tr>--:--</div>
        </div>
      </div>
    </div>
    <div class=g3>
      <div class=temp-box id=bN>
        <div class=lbl>Nozzle</div>
        <div class=val id=vN>—°</div>
        <div class=tgt id=tN></div>
      </div>
      <div class=temp-box id=bB>
        <div class=lbl>Bed</div>
        <div class=val id=vB>—°</div>
        <div class=tgt id=tB></div>
      </div>
      <div class=temp-box id=bC>
        <div class=lbl>Chamber</div>
        <div class=val id=vC>—°</div>
        <div class=tgt></div>
      </div>
    </div>
  </div>

  <!-- TAB 2: WiFi -->
  <div class=panel id=tab2>
    <label>Available networks
      <span class=refresh onclick=scanNets()>&#8635; Refresh</span>
    </label>
    <div class=net-list id=net-list>
      <div class=scanning>Loading...</div>
    </div>
    <a class=manual-toggle onclick=toggleManual()>&#9999; Enter SSID manually</a>
    <div id=manual-ssid>
      <label>SSID (hidden network)</label>
      <input type=text id=ssid-manual placeholder="Network name" maxlength=32>
    </div>
    <label>Password</label>
    <input type=password id=pass placeholder=Password maxlength=64>
    <div class=hint>Leave empty for open networks</div>
    <button class="btn btn-primary" onclick=doSaveWifi()>Save &amp; Connect</button>
    <div id=msg2 class=msg></div>
  </div>

  <!-- TAB 3: Printer -->
  <div class=panel id=tab3>
    <label>Printer IP address</label>
    <input type=text id=fi placeholder=192.168.1.xxx>
    <div class=hint>Find it on the printer display or your router</div>
    <label>Refresh interval (seconds)</label>
    <input type=number id=fr min=2 max=60 value=5>
    <label>OLED rotation</label>
    <select id=frot style="width:100%;padding:8px 10px;border:1px solid #333;border-radius:3px;background:#252525;color:#e0e0e0;font-size:.88rem;margin-bottom:8px;outline:none">
      <option value=0>0° — normal</option>
      <option value=2>180° — flipped</option>
    </select>
    <button class="btn btn-primary" onclick=doSavePrinter()>Save</button>
    <hr>
    <button class="btn btn-danger" onclick=doReset()>Factory Reset</button>
    <div id=msg3 class=msg></div>
  </div>

  <div class=status-bar id=sb>
    <span id=sb-l>—</span><span id=sb-r>—</span>
  </div>
</div>

<script>
var selSSID='',manualMode=false;

function showTab(n){
  for(var i=1;i<=3;i++){
    document.getElementById('t'+i).classList.toggle('active',i===n);
    document.getElementById('tab'+i).classList.toggle('active',i===n);
  }
  if(n===1) startPolling();
  if(n===2) scanNets();
}

function rssiToBars(rssi){
  var b=rssi>-55?4:rssi>-65?3:rssi>-75?2:1;
  var h='<span class=bar>';
  for(var i=1;i<=4;i++) h+='<span style="height:'+(i*3+2)+'px" class="'+(i<=b?'on':'')+'"></span>';
  return h+'</span>';
}

function renderNets(nets){
  var el=document.getElementById('net-list');
  if(!nets||!nets.length){el.innerHTML='<div class=scanning>No networks found.</div>';return;}
  nets.sort(function(a,b){return b.rssi-a.rssi;});
  el.innerHTML=nets.map(function(n){
    return '<div class=net-item onclick="selectNet(this,\''+n.ssid.replace(/'/g,"\\'")+'\')">'+
      '<span class=net-ssid>'+n.ssid+'</span>'+
      '<span class=net-info>'+rssiToBars(n.rssi)+(n.open?'':'&#128274;')+'</span></div>';
  }).join('');
  if(selSSID) document.querySelectorAll('.net-item').forEach(function(e){
    if(e.querySelector('.net-ssid').textContent===selSSID) e.classList.add('selected');
  });
}

function scanNets(){
  document.getElementById('net-list').innerHTML='<div class=scanning>Scanning...</div>';
  fetch('/api/scan').then(function(r){return r.json();}).then(renderNets)
    .catch(function(){document.getElementById('net-list').innerHTML='<div class=scanning>Error.</div>';});
}

function selectNet(el,ssid){
  document.querySelectorAll('.net-item').forEach(function(e){e.classList.remove('selected');});
  el.classList.add('selected');
  selSSID=ssid;
  document.getElementById('pass').focus();
}

function toggleManual(){
  manualMode=!manualMode;
  document.getElementById('manual-ssid').style.display=manualMode?'block':'none';
  if(manualMode){selSSID='';document.querySelectorAll('.net-item').forEach(function(e){e.classList.remove('selected');});}
}

function showMsg(id,txt,ok){
  var el=document.getElementById('msg'+id);
  el.textContent=txt;
  el.style.display='block';
  el.style.background=ok?'#1a2a1a':'#2a1a1a';
  el.style.color=ok?'#4caf50':'#ef5350';
  el.style.border='1px solid '+(ok?'#2e4a2e':'#4a2e2e');
}

async function doSaveWifi(){
  var ssid=manualMode?document.getElementById('ssid-manual').value.trim():selSSID;
  var pass=document.getElementById('pass').value;
  if(!ssid){showMsg(2,'Select a network or enter SSID manually.',false);return;}
  showMsg(2,'Saving...', true);
  try{
    var d=await fetch('/api/save',{method:'POST',body:new URLSearchParams({ssid:ssid,password:pass})}).then(r=>r.json());
    if(d.ok) showMsg(2,'Saved. ESP32 restarting...', true);
  }catch(e){showMsg(2,'Save error.',false);}
}

async function doSavePrinter(){
  var ip=document.getElementById('fi').value.trim();
  var ref=document.getElementById('fr').value;
  var rot=document.getElementById('frot').value;
  if(!ip){showMsg(3,'Enter printer IP address.',false);return;}
  try{
    var d=await fetch('/api/save',{method:'POST',body:new URLSearchParams({printerIP:ip,refreshSec:ref,oledRot:rot})}).then(r=>r.json());
    if(d.ok){showMsg(3,'Saved!',true);setTimeout(function(){showTab(1);},800);}
  }catch(e){showMsg(3,'Save error.',false);}
}

async function doReset(){
  if(!confirm('Factory reset? All settings will be erased.')) return;
  await fetch('/api/reset');
  showMsg(3,'Reset complete. Restarting...', true);
}

function fmt(s){
  if(!s||s<=0)return'--:--';
  var h=Math.floor(s/3600),m=Math.floor((s%3600)/60),sc=s%60,p=function(n){return('0'+n).slice(-2);};
  return h?h+':'+p(m)+':'+p(sc):p(m)+':'+p(sc);
}

var pollTimer=null;
function startPolling(){if(pollTimer)return;fetchPrinter();pollTimer=setInterval(fetchPrinter,3000);}

async function fetchPrinter(){
  try{
    var d=await fetch('/api/printer').then(r=>r.json());
    document.getElementById('pc').innerHTML=(d.printing?d.progress.toFixed(0):'–')+'<small>%</small>';
    document.getElementById('pf').style.width=(d.printing?d.progress:0)+'%';
    document.getElementById('fn').textContent=d.filename||'No active print';
    var b=document.getElementById('badge');
    b.textContent=(d.state||'offline').toUpperCase();
    b.className='badge '+(d.printing?'printing':d.connected?'idle':'');
    document.getElementById('te').textContent=fmt(d.elapsed);
    document.getElementById('tr').textContent=fmt(d.remaining);
    function st(id,v,t){
      document.getElementById('v'+id).textContent=v>0?v.toFixed(1)+'°':'—°';
      document.getElementById('t'+id).textContent=t>0?'target '+t.toFixed(0)+'°':'';
      document.getElementById('b'+id).className='temp-box'+(v>150?' hot':v>50?' warm':'');
    }
    st('N',d.tempNozzle,d.tempNozzleT);
    st('B',d.tempBed,d.tempBedT);
    st('C',d.tempChamber,0);
    document.getElementById('sb-l').textContent='ESP: '+(window._espIP||'—');
    document.getElementById('sb-r').textContent='PRT: '+(window._prtIP||'—');
  }catch(e){}
}

async function init(){
  try{
    var d=await fetch('/api/config').then(r=>r.json());
    document.getElementById('fi').value=d.printerIP||'';
    document.getElementById('fr').value=d.refreshSec||5;
    document.getElementById('frot').value=d.oledRot||0;
    if(d.wifiStatus==='connected'){
      document.getElementById('hdr').textContent='CC1 — ESP32-C3';
      window._espIP=d.espIP;
      window._prtIP=d.printerIP;
      document.getElementById('sb-l').textContent='ESP: '+d.espIP;
      document.getElementById('sb-r').textContent='PRT: '+(d.printerIP||'—');
      showTab(1);
    } else {
      document.getElementById('t1').style.display='none';
      document.getElementById('t3').style.display='none';
      showTab(2);
      scanNets();
    }
  }catch(e){showTab(2);scanNets();}
}
init();
</script>
</body>
</html>)rawliteral";

// ════════════════════════════════════════════════════════════════════════════
// OLED
// ════════════════════════════════════════════════════════════════════════════
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT  64
#define OLED_RESET     -1
#define OLED_ADDRESS  0x3C
#define I2C_SDA 10
#define I2C_SCL 9

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ════════════════════════════════════════════════════════════════════════════
// WEB SERVER
// ════════════════════════════════════════════════════════════════════════════
WebServer server(80);

// ════════════════════════════════════════════════════════════════════════════
// CONFIG
// ════════════════════════════════════════════════════════════════════════════
Preferences prefs;

struct Config {
  char ssid[64]      = "";
  char password[64]  = "";
  char printerIP[32] = "";
  int  refreshSec    = 5;
  int  oledRotation  = 0;
} cfg;

// ════════════════════════════════════════════════════════════════════════════
// PRINTER STATE
// ════════════════════════════════════════════════════════════════════════════
struct PrinterStatus {
  bool   connected         = false;
  bool   printing          = false;
  String filename          = "";
  float  progress          = 0.0f;
  int    elapsedSec        = 0;
  int    remainingSec      = 0;
  float  tempNozzle        = 0.0f;
  float  tempNozzleTarget  = 0.0f;
  float  tempBed           = 0.0f;
  float  tempBedTarget     = 0.0f;
  float  tempChamber       = 0.0f;
  int    currentLayer      = 0;
  int    totalLayer        = 0;
  String stateText         = "Offline";
  unsigned long lastUpdate = 0;
} printer;

// ════════════════════════════════════════════════════════════════════════════
// TIMERS & MODE
// ════════════════════════════════════════════════════════════════════════════
int  animFrame          = 0;
unsigned long lastAnim  = 0;
unsigned long lastFetch = 0;
unsigned long lastSerialLog = 0;
#define SERIAL_LOG_INTERVAL 5000

enum AppMode { MODE_AP_CONFIG, MODE_CONNECTING, MODE_RUNNING };
AppMode appMode = MODE_AP_CONFIG;

// ════════════════════════════════════════════════════════════════════════════
// PROTOTYPES
// ════════════════════════════════════════════════════════════════════════════
void loadConfig();
void saveConfig();
void startAPMode();
void startSTAMode();
void setupWebServer();
bool fetchPrinterStatus();
void parseSdcpStatus(const String& payload);
void drawPrintingScreen();
void drawPausedScreen();
void drawCompleteScreen();
void drawIdleScreen();
void drawConnectingScreen();
void drawAPScreen();
void renderCurrentScreen();
void tickOledAnimation();
String formatTime(int s);
String formatTimeCompact(int s);
void serialPrintStatus();

// ════════════════════════════════════════════════════════════════════════════
// SETUP
// ════════════════════════════════════════════════════════════════════════════
void setup() {
  Serial.begin(115200);
  Serial.setTxTimeoutMs(0);  // don't block when there's no USB Serial connection
  delay(200);

  // Disable BT — not used, frees RF/RAM resources
  btStop();

  Serial.println("\n\n========================================");
  Serial.println("   Elegoo CC1 Monitor — starting");
  Serial.println("========================================");

  Wire.begin(I2C_SDA, I2C_SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println("[OLED] Not found! Check wiring and address.");
    while (true) delay(1000);
  }
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.display();
  Serial.println("[OLED] OK");

  loadConfig();  // load config first — to get the correct rotation
  display.setRotation(cfg.oledRotation);  // then apply rotation from flash

  if (strlen(cfg.ssid) == 0) {
    startAPMode();
  } else {
    appMode = MODE_CONNECTING;
    startSTAMode();
  }

  setupWebServer();
}

// ════════════════════════════════════════════════════════════════════════════
// LOOP
// ════════════════════════════════════════════════════════════════════════════
void loop() {
  server.handleClient();

  unsigned long now = millis();

  if (appMode == MODE_RUNNING && (now - lastFetch) >= (unsigned long)(cfg.refreshSec * 1000)) {
    lastFetch = now;
    fetchPrinterStatus();
  }

  if (appMode == MODE_RUNNING && (now - lastSerialLog) >= SERIAL_LOG_INTERVAL) {
    lastSerialLog = now;
    serialPrintStatus();
  }

  tickOledAnimation();
}

// ════════════════════════════════════════════════════════════════════════════
// OLED ANIMATION TICK — also called from fetchPrinterStatus() while it
// blocks waiting on the WebSocket, so the display never freezes mid-poll.
// ════════════════════════════════════════════════════════════════════════════
void renderCurrentScreen() {
  display.clearDisplay();
  switch (appMode) {
    case MODE_AP_CONFIG:  drawAPScreen();         break;
    case MODE_CONNECTING: drawConnectingScreen();  break;
    case MODE_RUNNING:
      if (printer.stateText == "Paused" || printer.stateText == "Pausing...")
                                         drawPausedScreen();
      else if (printer.stateText == "Complete") drawCompleteScreen();
      else if (printer.printing)         drawPrintingScreen();
      else                               drawIdleScreen();
      break;
  }
  display.display();
}

void tickOledAnimation() {
  unsigned long now = millis();
  if (now - lastAnim >= 80) {
    lastAnim = now;
    animFrame = (animFrame + 1) % 60;
    renderCurrentScreen();
  }
}

// ════════════════════════════════════════════════════════════════════════════
// CONFIG — save/load flash
// ════════════════════════════════════════════════════════════════════════════
void loadConfig() {
  prefs.begin("elegoo_mon", false);
  prefs.getString("ssid",      cfg.ssid,      sizeof(cfg.ssid));
  prefs.getString("password",  cfg.password,  sizeof(cfg.password));
  prefs.getString("printerIP", cfg.printerIP, sizeof(cfg.printerIP));
  cfg.refreshSec   = prefs.getInt("refreshSec", 5);
  cfg.oledRotation = prefs.getInt("oledRot", 0);
  prefs.end();
  Serial.printf("[Config] SSID=%s  IP=%s  refresh=%ds\n", cfg.ssid, cfg.printerIP, cfg.refreshSec);
}

void saveConfig() {
  prefs.begin("elegoo_mon", false);
  prefs.putString("ssid",      cfg.ssid);
  prefs.putString("password",  cfg.password);
  prefs.putString("printerIP", cfg.printerIP);
  prefs.putInt("refreshSec",  cfg.refreshSec);
  prefs.putInt("oledRot",     cfg.oledRotation);
  prefs.end();
  Serial.println("[Config] Saved");
}

// ════════════════════════════════════════════════════════════════════════════
// WIFI
// ════════════════════════════════════════════════════════════════════════════
void startAPMode() {
  appMode = MODE_AP_CONFIG;
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  WiFi.mode(WIFI_AP_STA);
  // Must be set AFTER WiFi.mode() — the radio isn't started before that, so
  // setTxPower() silently no-ops if called earlier (as it used to be in setup()).
  // Limits TX power — ESP32-C3 SuperMini antenna/power circuit can't handle full power.
  WiFi.setTxPower(WIFI_POWER_8_5dBm);
  WiFi.softAP("ElegooMonitor", "elegoo123");
  delay(500);
  Serial.printf("[AP] SSID: ElegooMonitor  Pass: elegoo123  IP: %s\n", WiFi.softAPIP().toString().c_str());
}

void startSTAMode() {
  WiFi.mode(WIFI_STA);
  // See comment in startAPMode() — must come after WiFi.mode() to actually apply.
  WiFi.setTxPower(WIFI_POWER_8_5dBm);
  WiFi.begin(cfg.ssid, cfg.password);
  Serial.printf("[WiFi] Connecting to \"%s\"...\n", cfg.ssid);

  int tries = 0;
  while (WiFi.status() != WL_CONNECTED && tries < 40) {
    delay(500);
    tries++;
    display.clearDisplay();
    drawConnectingScreen();
    display.display();
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("[WiFi] Connected! IP: %s\n", WiFi.localIP().toString().c_str());
    Serial.printf("[WiFi] Panel: http://%s\n", WiFi.localIP().toString().c_str());
    Serial.println("[WiFi] Panel: http://elegoo-monitor.local");
    if (MDNS.begin("elegoo-monitor")) {
      Serial.println("[mDNS] OK — elegoo-monitor.local");
    }
    appMode = MODE_RUNNING;
  } else {
    Serial.println("[WiFi] Connection failed — switching to AP mode");
    startAPMode();
  }
}

// ════════════════════════════════════════════════════════════════════════════
// WEB SERVER
// ════════════════════════════════════════════════════════════════════════════
void handleRoot() {
  server.sendHeader("Content-Encoding", "identity");
  server.send(200, "text/html", WEB_PANEL_HTML);
}

void handleApiScan() {
  WiFi.mode(WIFI_AP_STA);
  int n = WiFi.scanNetworks();
  String json = "[";
  if (n > 0) json.reserve(n * 48 + 8);
  for (int i = 0; i < n; i++) {
    if (i > 0) json += ",";
    String ssid = WiFi.SSID(i);
    ssid.replace("\"", "\\\"");
    json += "{\"ssid\":\"" + ssid + "\",\"rssi\":" + WiFi.RSSI(i) +
            ",\"open\":" + (WiFi.encryptionType(i) == WIFI_AUTH_OPEN ? "true" : "false") + "}";
  }
  json += "]";
  WiFi.scanDelete();
  server.send(200, "application/json", json);
}

void handleApiConfig() {
  JsonDocument doc;
  doc["ssid"]       = cfg.ssid;
  doc["printerIP"]  = cfg.printerIP;
  doc["refreshSec"] = cfg.refreshSec;
  doc["oledRot"]    = cfg.oledRotation;
  doc["wifiStatus"] = (WiFi.status() == WL_CONNECTED) ? "connected" : "disconnected";
  doc["espIP"]      = (WiFi.status() == WL_CONNECTED) ? WiFi.localIP().toString() : "";
  String json;
  serializeJson(doc, json);
  server.send(200, "application/json", json);
}

void handleApiSave() {
  bool ssidChanged = false;
  if (server.hasArg("ssid") && server.arg("ssid").length() > 0) {
    if (server.arg("ssid") != cfg.ssid) ssidChanged = true;
    strncpy(cfg.ssid,     server.arg("ssid").c_str(),     sizeof(cfg.ssid)-1);
  }
  if (server.hasArg("password") && server.arg("password").length() > 0) {
    strncpy(cfg.password, server.arg("password").c_str(), sizeof(cfg.password)-1);
  }
  if (server.hasArg("printerIP")) {
    strncpy(cfg.printerIP, server.arg("printerIP").c_str(), sizeof(cfg.printerIP)-1);
  }
  if (server.hasArg("refreshSec")) {
    cfg.refreshSec = server.arg("refreshSec").toInt();
    if (cfg.refreshSec < 2) cfg.refreshSec = 2;
  }
  if (server.hasArg("oledRot")) {
    cfg.oledRotation = server.arg("oledRot").toInt();
    display.setRotation(cfg.oledRotation);
  }
  saveConfig();
  server.send(200, "application/json", "{\"ok\":true}");
  delay(1000);
  if (ssidChanged) {
    Serial.println("[Config] SSID changed — restarting");
    ESP.restart();
  }
}

void handleApiPrinter() {
  JsonDocument doc;
  doc["connected"]   = printer.connected;
  doc["printing"]    = printer.printing;
  doc["filename"]    = printer.filename;
  doc["progress"]    = printer.progress;
  doc["elapsed"]     = printer.elapsedSec;
  doc["remaining"]   = printer.remainingSec;
  doc["tempNozzle"]  = printer.tempNozzle;
  doc["tempNozzleT"] = printer.tempNozzleTarget;
  doc["tempBed"]     = printer.tempBed;
  doc["tempBedT"]    = printer.tempBedTarget;
  doc["tempChamber"]   = printer.tempChamber;
  doc["state"]         = printer.stateText;
  doc["currentLayer"]  = printer.currentLayer;
  doc["totalLayer"]    = printer.totalLayer;
  String json;
  serializeJson(doc, json);
  server.send(200, "application/json", json);
}

void handleApiReset() {
  prefs.begin("elegoo_mon", false);
  prefs.clear();
  prefs.end();
  server.send(200, "application/json", "{\"ok\":true}");
  Serial.println("[Reset] Flash cleared — restarting");
  delay(500);
  ESP.restart();
}

void setupWebServer() {
  server.on("/",            HTTP_GET,  handleRoot);
  server.on("/api/scan",    HTTP_GET,  handleApiScan);
  server.on("/api/config",  HTTP_GET,  handleApiConfig);
  server.on("/api/printer", HTTP_GET,  handleApiPrinter);
  server.on("/api/save",    HTTP_POST, handleApiSave);
  server.on("/api/reset",   HTTP_GET,  handleApiReset);
  server.on("/generate_204",        HTTP_GET, handleRoot);
  server.on("/hotspot-detect.html", HTTP_GET, handleRoot);
  server.on("/fwlink",              HTTP_GET, handleRoot);
  server.onNotFound(handleRoot);
  server.begin();
  Serial.println("[Web] Server started on port 80");
}

// ════════════════════════════════════════════════════════════════════════════
// ELEGOO SDCP — WebSocket port 3030
// ════════════════════════════════════════════════════════════════════════════
void parseSdcpStatus(const String& payload) {
  JsonDocument doc;
  if (deserializeJson(doc, payload)) {
    Serial.println("[SDCP] JSON parse error");
    return;
  }

  // Frame 2 structure: {"Status":{...}, "MainboardID":..., "Topic":...}
  JsonObject status = doc["Status"];
  if (status.isNull()) return;

  printer.connected = true;

  // CurrentStatus is an array — first element is machine state
  int printStatus = 0;
  if (status["CurrentStatus"].is<JsonArray>()) {
    printStatus = status["CurrentStatus"][0];
  } else {
    printStatus = status["CurrentStatus"] | 0;
  }
  // PrintInfo.Status — full map per SDCP v3 + Elegoo FDM-specific values
  int piStatus = status["PrintInfo"]["Status"] | 0;
  printer.printing = (piStatus == 1 || piStatus == 2 || piStatus == 3 || piStatus == 4 ||
                      piStatus == 5 || piStatus == 6 || piStatus == 13 || piStatus == 16 ||
                      printStatus == 1 || printStatus == 2);

  switch(piStatus) {
    case 0:  printer.stateText = "Idle";        break;
    case 1:  printer.stateText = "Printing";    break;
    case 2:  printer.stateText = "Printing";    break;  // Elegoo CC FDM movement phases
    case 3:  printer.stateText = "Printing";    break;
    case 4:  printer.stateText = "Printing";    break;
    case 5:  printer.stateText = "Pausing...";  break;
    case 6:  printer.stateText = "Paused";      break;
    case 7:  printer.stateText = "Stopping..."; break;
    case 8:  printer.stateText = "Stopped";     break;
    case 9:  printer.stateText = "Complete";    break;
    case 10: printer.stateText = "Checking..."; break;
    case 13: printer.stateText = "Printing";    break;  // observed in Elegoo CC logs
    case 16: printer.stateText = "Heating";     break;  // pre-print heating
    default:
      switch(printStatus) {
        case 0:  printer.stateText = "Idle";     break;
        case 1:  printer.stateText = "Printing"; break;
        case 2:  printer.stateText = "Paused";   break;
        case 3:  printer.stateText = "Stopping"; break;
        case 4:  printer.stateText = "Complete"; break;
        default: printer.stateText = "Idle";     break;
      }
  }

  // On Complete, keep the previous filename if the new one is empty
  String newFilename = status["PrintInfo"]["Filename"] | "";
  if (newFilename.length() > 0 || printer.stateText != "Complete") {
    printer.filename = newFilename;
  }
  float rawProgress = status["PrintInfo"]["Progress"] | 0.0f;
  float currentTicks = status["PrintInfo"]["CurrentTicks"] | 0.0f;
  float totalTicks   = status["PrintInfo"]["TotalTicks"]   | 0.0f;
  printer.currentLayer = status["PrintInfo"]["CurrentLayer"] | 0;
  printer.totalLayer   = status["PrintInfo"]["TotalLayer"]   | 0;

  if (printer.printing) {
    if (currentTicks < 10.0f) {
      // Prep phase — force progress to zero regardless of the reported value
      printer.progress = 0.0f;
      bool nozzleReady = (printer.tempNozzleTarget <= 0.0f ||
                          printer.tempNozzle >= printer.tempNozzleTarget - 5.0f);
      bool bedReady    = (printer.tempBedTarget <= 0.0f ||
                          printer.tempBed >= printer.tempBedTarget - 3.0f);
      if (!nozzleReady || !bedReady) printer.stateText = "Heating";
    } else {
      // Ticks > 10s — actual printing, scale progress
      printer.progress = (rawProgress > 0.0f && rawProgress <= 1.0f)
                         ? rawProgress * 100.0f : rawProgress;
    }
    printer.elapsedSec   = (int)currentTicks;
    printer.remainingSec = (int)(totalTicks - currentTicks);
    if (printer.remainingSec < 0) printer.remainingSec = 0;
  } else if (printer.stateText == "Complete") {
    // Complete — keep the data (filename, elapsed) until it transitions to Idle
    // Only refresh elapsed from ticks if they're sane
    if (currentTicks > 10.0f) printer.elapsedSec = (int)currentTicks;
    printer.remainingSec = 0;
  } else {
    // Idle/Stopped — clear the data
    printer.elapsedSec   = 0;
    printer.remainingSec = 0;
    printer.progress     = 0.0f;
    printer.filename     = "";
    printer.currentLayer = 0;
    printer.totalLayer   = 0;
  }

  printer.tempNozzle       = status["TempOfNozzle"]     | 0.0f;
  printer.tempNozzleTarget = status["TempTargetNozzle"] | 0.0f;
  printer.tempBed          = status["TempOfHotbed"]     | 0.0f;
  printer.tempBedTarget    = status["TempTargetHotbed"] | 0.0f;
  printer.tempChamber      = status["TempOfBox"]        | 0.0f;

  printer.lastUpdate = millis();
  Serial.printf("[SDCP] OK — %s  %.1f%%  E:%.1f/%.0f  B:%.1f/%.0f  Ch:%.1f\n",
    printer.stateText.c_str(), printer.progress,
    printer.tempNozzle, printer.tempNozzleTarget,
    printer.tempBed, printer.tempBedTarget,
    printer.tempChamber);
}

bool fetchPrinterStatus() {
  if (WiFi.status() != WL_CONNECTED) return false;
  if (strlen(cfg.printerIP) == 0) return false;

  WiFiClient client;
  client.setTimeout(2);

  if (!client.connect(cfg.printerIP, 3030)) {
    Serial.printf("[SDCP] Cannot connect to %s:3030\n", cfg.printerIP);
    printer.connected = false;
    printer.stateText = "Offline";
    return false;
  }

  // WebSocket handshake
  client.print(
    String("GET /websocket HTTP/1.1\r\n") +
    "Host: " + cfg.printerIP + ":3030\r\n" +
    "Upgrade: websocket\r\n" +
    "Connection: Upgrade\r\n" +
    "Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n" +
    "Sec-WebSocket-Version: 13\r\n\r\n"
  );

  // Wait for the HTTP upgrade response ourselves instead of client.readString() —
  // readString() blocks internally for the full setTimeout() window with no way
  // to refresh the OLED in between, which was the real cause of the freezes.
  String resp;
  unsigned long t = millis();
  while (client.connected() && resp.indexOf("\r\n\r\n") < 0 && millis() - t < 2000) {
    tickOledAnimation();
    while (client.available()) resp += (char)client.read();
    if (resp.indexOf("\r\n\r\n") < 0) delay(5);
  }
  if (resp.indexOf("101") < 0) {
    Serial.println("[SDCP] No HTTP 101 upgrade");
    client.stop();
    printer.connected = false;
    return false;
  }

  // Send Cmd:0 status request
  String req = "{\"Id\":\"esp32\",\"Data\":{\"Cmd\":0,\"Data\":{},\"RequestID\":\"1\",\"MainboardID\":\"\",\"TimeStamp\":0,\"From\":0},\"Topic\":\"sdcp/request/\"}";
  uint8_t hdr[2] = {0x81, (uint8_t)req.length()};
  client.write(hdr, 2);
  client.print(req);

  // Read frame helper lambda
  auto readFrame = [&](int timeoutMs) -> String {
    String out = "";
    unsigned long ft = millis();
    while (client.connected() && client.available() < 2 && millis() - ft < (unsigned long)timeoutMs) {
      // Keep refreshing the OLED while waiting — animation doesn't freeze
      tickOledAnimation();
      delay(10);
    }
    if (!client.available()) return out;
    uint8_t b0 = client.read();
    uint8_t b1 = client.read();
    bool masked = (b1 & 0x80);
    int len = b1 & 0x7F;
    if (len == 126) len = (client.read() << 8) | client.read();
    if (masked) { for(int i=0;i<4;i++) client.read(); }
    if (len > 0) out.reserve(len);
    unsigned long rt = millis();
    while ((int)out.length() < len && millis() - rt < 2000) {
      // Same OLED refresh here — payload can arrive in several TCP chunks
      tickOledAnimation();
      if (client.available()) out += (char)client.read();
      else delay(1);
    }
    return out;
  };

  // Frame 1: expect ACK
  String frame1 = readFrame(2000);
  if (frame1.indexOf("\"Ack\"") < 0) {
    Serial.printf("[SDCP] No Ack frame (got %d bytes)\n", frame1.length());
    client.stop();
    return false;
  }

  // Frame 2: actual status
  String frame2 = readFrame(4000);
  Serial.printf("[SDCP] frame2: %d bytes\n", frame2.length());

  if (frame2.length() > 0) {
    parseSdcpStatus(frame2);
    // Check for an extra frame (e.g. unfinished print notification) that may
    // already be sitting in the buffer — must be checked BEFORE client.stop(),
    // otherwise the socket is already closed. Use a 0ms timeout: only grab
    // what's already arrived, don't stall the loop waiting for one that won't come.
    String frame3 = readFrame(0);
    if (frame3.length() > 0) {
      Serial.printf("[SDCP] frame3: %d bytes\n", frame3.length());
      for (int i = 0; i < (int)frame3.length(); i += 256)
        Serial.print(frame3.substring(i, min(i+256, (int)frame3.length())));
      Serial.println();
    }
    client.stop();
    return printer.connected;
  }
  client.stop();
  return false;
}

// ════════════════════════════════════════════════════════════════════════════
// SERIAL DEBUG
// ════════════════════════════════════════════════════════════════════════════
void serialPrintStatus() {
  Serial.println("----------------------------------------");
  Serial.printf("  Uptime    : %lu s\n", millis() / 1000);
  Serial.printf("  WiFi      : %s\n", WiFi.status() == WL_CONNECTED ? WiFi.localIP().toString().c_str() : "disconnected");
  Serial.printf("  Printer   : %s\n", strlen(cfg.printerIP) ? cfg.printerIP : "(no IP set)");
  Serial.printf("  Connected : %s\n", printer.connected ? "YES" : "NO");
  Serial.printf("  State     : %s\n", printer.stateText.c_str());
  if (printer.printing || printer.progress > 0.0f) {
    Serial.printf("  File      : %s\n", printer.filename.length() ? printer.filename.c_str() : "(none)");
    Serial.printf("  Progress  : %.1f%%\n", printer.progress);
    Serial.printf("  Elapsed   : %s\n", formatTime(printer.elapsedSec).c_str());
    Serial.printf("  Remaining : %s\n", formatTime(printer.remainingSec).c_str());
  }
  Serial.printf("  Nozzle    : %.1f / %.0f C\n", printer.tempNozzle, printer.tempNozzleTarget);
  Serial.printf("  Bed       : %.1f / %.0f C\n", printer.tempBed, printer.tempBedTarget);
  if (printer.tempChamber > 0.5f)
    Serial.printf("  Chamber   : %.1f C\n", printer.tempChamber);
  else
    Serial.println("  Chamber   : no sensor");
  Serial.println("----------------------------------------");
}

// ════════════════════════════════════════════════════════════════════════════
// OLED SCREENS
// ════════════════════════════════════════════════════════════════════════════
String formatTime(int s) {
  if (s <= 0) return "--:--";
  int h = s / 3600, m = (s % 3600) / 60, sec = s % 60;
  char buf[12];
  if (h > 0) snprintf(buf, sizeof(buf), "%d:%02d:%02d", h, m, sec);
  else        snprintf(buf, sizeof(buf), "%02d:%02d", m, sec);
  return String(buf);
}

// Like formatTime, but without a leading zero on minutes (shorter for the OLED)
String formatTimeCompact(int s) {
  if (s <= 0) return "--:--";
  int h = s / 3600, m = (s % 3600) / 60, sec = s % 60;
  char buf[10];
  if (h > 0) snprintf(buf, sizeof(buf), "%d:%02d:%02d", h, m, sec);
  else       snprintf(buf, sizeof(buf), "%d:%02d", m, sec);
  return String(buf);
}

// Nozzle icon (7x8): tip shape
static const uint8_t ICON_NOZZLE[] = {
  0b00111100,
  0b00111100,
  0b00111100,
  0b00011000,
  0b00011000,
  0b00001000,
  0b00001000,
  0b00000000
};

// Bed icon (7x8): flat surface with legs
static const uint8_t ICON_BED[] = {
  0b00000000,
  0b01111110,
  0b01111110,
  0b00000000,
  0b01000010,
  0b01000010,
  0b01000010,
  0b00000000
};

// Chamber icon (7x8): box outline
static const uint8_t ICON_CHAMBER[] = {
  0b01111110,
  0b01000010,
  0b01000010,
  0b01000010,
  0b01000010,
  0b01000010,
  0b01111110,
  0b00000000
};

void drawIcon(int x, int y, const uint8_t* icon) {
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      if (icon[row] & (0x80 >> col)) {
        display.drawPixel(x + col, y + row, SSD1306_WHITE);
      }
    }
  }
}

// Scrolling filename state (shared between drawPausedScreen and drawPrintingScreen)
static int scrollOffset = 0;
static unsigned long lastScroll = 0;
static String lastFilename = "";

void drawCompleteScreen() {
  int f = animFrame;

  // Blinking "DONE!" — textSize(3) = 18px/char, "DONE!" = 5*18 = 90px
  if ((f / 12) % 2 == 0) {
    display.setTextSize(3);
    display.setCursor((128 - 5*18) / 2, 6);
    display.print("DONE!");
  }

  // Print time
  display.setTextSize(1);
  String tStr = formatTime(printer.elapsedSec);
  display.setCursor((128 - (int)(tStr.length()*6)) / 2, 36);
  display.print(tStr);

  // Scrolling filename
  String fname = printer.filename;
  int fnameW = fname.length() * 6;
  int fieldW = 118;
  unsigned long now = millis();
  if (fnameW > fieldW && now - lastScroll > 130) {
    scrollOffset++;
    if (scrollOffset > fnameW + 8) scrollOffset = -fieldW;
    lastScroll = now;
  }
  for (int ci = 0; ci < (int)fname.length(); ci++) {
    int cx = 5 + ci*6 - (fnameW <= fieldW ? 0 : scrollOffset);
    if (cx >= 5 && cx <= 122)
      display.drawChar(cx, 49, fname[ci], SSD1306_WHITE, SSD1306_BLACK, 1);
  }
}

void drawPausedScreen() {
  int f = animFrame;

  // Blinking "PAUSED" textSize(3), centered — y=5
  if ((f / 12) % 2 == 0) {
    display.setTextSize(3);
    display.setCursor((128 - 6*18) / 2, 5);
    display.print("PAUSED");
  }

  // Rem Time / Layers alternating every 5s — y=36
  char buf[28];
  bool showLayers = (millis() / 5000) % 2 == 1;
  if (showLayers)
    snprintf(buf, sizeof(buf), "Layers: %d/%d", printer.currentLayer, printer.totalLayer);
  else
    snprintf(buf, sizeof(buf), "Rem Time: %s", formatTimeCompact(printer.remainingSec).c_str());

  display.setTextSize(1);
  int bw = strlen(buf) * 6;
  display.setCursor((128 - bw) / 2, 36);
  display.print(buf);

  // Scrolling filename — y=49 (same as the Done screen)
  String fname = printer.filename;
  int fnameW = fname.length() * 6;
  int fieldW = 118;
  unsigned long now = millis();
  if (fnameW > fieldW && now - lastScroll > 130) {
    scrollOffset++;
    if (scrollOffset > fnameW + 8) scrollOffset = -fieldW;
    lastScroll = now;
  }
  for (int ci = 0; ci < (int)fname.length(); ci++) {
    int cx = 5 + ci*6 - (fnameW <= fieldW ? 0 : scrollOffset);
    if (cx >= 5 && cx <= 122)
      display.drawChar(cx, 49, fname[ci], SSD1306_WHITE, SSD1306_BLACK, 1);
  }
}

// Scrolling filename state
void drawPrintingScreen() {
  char buf[32];
  int progress = (int)printer.progress;

  // ── Progress bar track ─────────────────────────────────────────
  display.drawRect(0, 0, 128, 64, SSD1306_WHITE);
  display.drawRect(4, 4, 120, 56, SSD1306_WHITE);

  int filled = (int)(progress / 100.0f * 384);
  int rem = filled;
  if (rem > 0) { int l=min(rem, 64);  display.fillRect(64,    0, l,  4, SSD1306_WHITE); rem-=l; }
  if (rem > 0) { int l=min(rem, 64);  display.fillRect(124,   0, 4,  l, SSD1306_WHITE); rem-=l; }
  if (rem > 0) { int l=min(rem,128);  display.fillRect(128-l,60, l,  4, SSD1306_WHITE); rem-=l; }
  if (rem > 0) { int l=min(rem, 64);  display.fillRect(0,  64-l, 4,  l, SSD1306_WHITE); rem-=l; }
  if (rem > 0) { int l=min(rem, 64);  display.fillRect(0,    0, l,  4, SSD1306_WHITE); }

  // ── Row 1: filename (textSize1) | gap | percent (textSize2) ─
  // Percent textSize(2): "100%" = 4*12=48px, + 1px gap from border = x from 122-48=74
  // Always 1px clear of the inner border (x=5..122, y=5..58)
  snprintf(buf, sizeof(buf), "%d%%", progress);
  int pctW = strlen(buf) * 12;
  int pctX = 121 - pctW;  // 1px from the right inner border (x=123)

  display.setTextSize(2);
  display.setCursor(pctX, 6);  // 1px from the top inner border (y=5)
  display.print(buf);

  // Filename: field x=6..(pctX-5), 5px gap before the percent
  String fname = printer.filename;
  if (fname != lastFilename) { scrollOffset=0; lastFilename=fname; lastScroll=millis(); }
  unsigned long now = millis();
  int nameFieldW = pctX - 11;  // 6 start + 5px gap before the percent
  int fnameW = fname.length() * 6;
  if (fnameW > nameFieldW && now-lastScroll > 130) {
    scrollOffset++;
    if (scrollOffset > fnameW + 8) scrollOffset = -nameFieldW;
    lastScroll = now;
  }
  display.setTextSize(1);
  // filename vertically centered in textSize(2) row (y=6..21)
  int nameStartX = (fnameW <= nameFieldW) ? 6 + (nameFieldW-fnameW)/2 : 6;
  for (int ci=0; ci<(int)fname.length(); ci++) {
    int cx = nameStartX + ci*6 - (fnameW <= nameFieldW ? 0 : scrollOffset);
    if (cx >= 6 && cx <= pctX-6)
      display.drawChar(cx, 11, fname[ci], SSD1306_WHITE, SSD1306_BLACK, 1);
  }

  // ── Separator ─────────────────────────────────────────────────
  display.drawFastHLine(5, 22, 118, SSD1306_WHITE);

  // ── Row 2: alternating Rem Time / Layers every 5s ──────────
  display.setTextSize(1);
  String infoStr;
  if (printer.stateText == "Heating") {
    const char* prep[] = {"Preparing.", "Preparing..", "Preparing..."};
    infoStr = prep[(animFrame / 10) % 3];
  } else {
    bool showLayers = (millis() / 5000) % 2 == 1;
    if (showLayers) {
      char lb[24];
      snprintf(lb, sizeof(lb), "Layers: %d/%d", printer.currentLayer, printer.totalLayer);
      infoStr = String(lb);
    } else {
      infoStr = String("Rem Time: ") + formatTimeCompact(printer.remainingSec);
    }
  }
  int infoW = infoStr.length() * 6;
  display.setCursor(6 + (116 - infoW) / 2, 28);
  display.print(infoStr);

  // ── Temp separator ────────────────────────────────────────────
  display.drawFastHLine(5, 41, 118, SSD1306_WHITE);

  // ── Temperatures y=42..58 — 2 rows of 8px + 1px gap ────────
  // Row 1: current temp (y=43), Row 2: target (y=52)
  // Vertical separators
  display.drawFastVLine(43, 41, 18, SSD1306_WHITE);
  display.drawFastVLine(82, 41, 18, SSD1306_WHITE);
  display.setTextSize(1);

  auto drawTempCol = [&](int colX, int colW, const uint8_t* icon, float val, float tgt) {
    snprintf(buf, sizeof(buf), "%.0f", val);
    int vw = strlen(buf)*6, tot = 8+1+vw;
    int ox = colX + (colW-tot)/2;
    if (ox < colX+1) ox = colX+1;
    drawIcon(ox, 43, icon);
    display.setCursor(ox+9, 43); display.print(buf);
    if (tgt > 0) snprintf(buf, sizeof(buf), "/%d", (int)tgt);
    else         snprintf(buf, sizeof(buf), "/--");
    int tw = strlen(buf)*6;
    display.setCursor(colX + (colW-tw)/2, 51); display.print(buf);
  };

  drawTempCol(5,  38, ICON_NOZZLE,  printer.tempNozzle,  printer.tempNozzleTarget);
  drawTempCol(44, 38, ICON_BED,     printer.tempBed,     printer.tempBedTarget);
  drawTempCol(83, 40, ICON_CHAMBER, printer.tempChamber, 0);
}

void drawIdleScreen() {
  int f = animFrame;

  // ── Draw the separator first so the animation doesn't overwrite it ────
  display.drawLine(0, 27, 127, 27, SSD1306_WHITE);

  // ── Infinity logo animation ────────────────────────────
  // r=7, cx1=8, cx2=22 — max x=29, max y=cy+7
  // cy=14, r=7 → y:7..21, separator at y=27
  int r = 7;
  int cx1 = 8, cy = 14;
  int cx2 = cx1 + r * 2;
  display.drawCircle(cx1, cy, r, SSD1306_WHITE);
  display.drawCircle(cx2, cy, r, SSD1306_WHITE);
  int rr1 = (f)      % (r + 2);
  int rr2 = (f + 15) % (r + 2);
  if (rr1 <= r) display.drawCircle(cx1, cy, rr1, SSD1306_WHITE);
  if (rr2 <= r) display.drawCircle(cx2, cy, rr2, SSD1306_WHITE);

  // ── Title centered in the right column (x=30..127 = 98px) ──
  // "ELEGOO MONITOR" = 14*6 = 84px → x = 30 + (98-84)/2 = 37
  display.setTextSize(1);
  display.setCursor(37, 3); display.print("ELEGOO MONITOR");

  // ── Status centered in the right column ──
  String stateStr = printer.connected ? printer.stateText :
    String(((const char*[]){"Offline","Offline.","Offline..","Offline..."}[(f/12)%4]));
  int stateW = stateStr.length() * 6;
  int stateX = 30 + (98 - stateW) / 2;
  display.setCursor(stateX, 14); display.print(stateStr);

  // ── 3 temperature columns (y=27..53, height=26px) ──
  // Col 1: x=0..41
  // Col 2: x=43..84
  // Col 3: x=86..127
  display.drawLine(42, 26, 42, 53, SSD1306_WHITE);
  display.drawLine(85, 26, 85, 53, SSD1306_WHITE);

  // Cell content: icon(8px) + 2px gap + text(n*6px), height max(8,8)=8px
  // Center vertically: y = 26 + (27-8)/2 = 35
  int cellY = 35;
  char buf[8];

  // Column 1 — Nozzle
  snprintf(buf, sizeof(buf), "%.0fC", printer.tempNozzle);
  { int tw=strlen(buf)*6, tot=8+2+tw, cx=(42-tot)/2;
    drawIcon(cx, cellY, ICON_NOZZLE);
    display.setCursor(cx+10, cellY); display.print(buf); }

  // Column 2 — Bed
  snprintf(buf, sizeof(buf), "%.0fC", printer.tempBed);
  { int tw=strlen(buf)*6, tot=8+2+tw, cx=43+(42-tot)/2;
    drawIcon(cx, cellY, ICON_BED);
    display.setCursor(cx+10, cellY); display.print(buf); }

  // Column 3 — Chamber
  snprintf(buf, sizeof(buf), "%.0fC", printer.tempChamber > 0.5f ? printer.tempChamber : 0.0f);
  { int tw=strlen(buf)*6, tot=8+2+tw, cx=86+(42-tot)/2;
    drawIcon(cx, cellY, ICON_CHAMBER);
    display.setCursor(cx+10, cellY); display.print(buf); }

  // ── Bottom separator ──
  display.drawLine(0, 53, 127, 53, SSD1306_WHITE);

  // ── IP centered, alternating ──
  char ipLine[32];
  if ((f / 30) % 2 == 0)
    snprintf(ipLine, sizeof(ipLine), "ESP: %s",
      WiFi.status()==WL_CONNECTED ? WiFi.localIP().toString().c_str() : "--");
  else
    snprintf(ipLine, sizeof(ipLine), "PRT: %s",
      strlen(cfg.printerIP) ? cfg.printerIP : "--");
  int ipX = (128 - (int)(strlen(ipLine)*6)) / 2;
  display.setCursor(ipX, 57); display.print(ipLine);
}

void drawConnectingScreen() {
  display.setTextSize(1);
  display.setCursor(0, 0);  display.print("Connecting WiFi");
  const char* frames[] = {".  ", ".. ", "..."};
  display.print(frames[(animFrame / 10) % 3]);
  display.setCursor(0, 16); display.print("SSID: "); display.print(cfg.ssid);
  const char spinner[] = {'|', '/', '-', '\\'};
  display.setTextSize(2);
  display.setCursor(56, 36); display.write(spinner[(animFrame / 5) % 4]);
}

void drawAPScreen() {
  display.setTextSize(1);

  // Pulsing border — 2px from the edge so it doesn't overlap the text
  if ((animFrame / 15) % 2 == 0)
    display.drawRect(2, 2, 124, 60, SSD1306_WHITE);

  // Centered title
  const char* title = "-- SETUP MODE --";
  display.setCursor((128 - strlen(title)*6) / 2, 6);
  display.print(title);
  display.drawFastHLine(4, 15, 120, SSD1306_WHITE);

  // Centered info lines
  const char* l1 = "WiFi: ElegooMonitor";
  const char* l2 = "Pass: elegoo123";
  const char* l3 = "IP: 192.168.4.1";
  display.setCursor((128 - strlen(l1)*6) / 2, 19); display.print(l1);
  display.setCursor((128 - strlen(l2)*6) / 2, 31); display.print(l2);
  display.setCursor((128 - strlen(l3)*6) / 2, 43); display.print(l3);
}
