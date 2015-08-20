# 用 LinkIt One 做一個烘衣機控制裝置的雛型
(**這篇的完整進度可以在這裡看到:  [https://www.gitbook.com/book/kuochunchang/-make-a-dryer-controller/details](https://www.gitbook.com/book/kuochunchang/-make-a-dryer-controller/details)**)


前一陣子用了多年的烘衣機壞了，原本想修理一下繼續使用，不過考量安全問題，還是買了台新的。但是新機器在使用上有一點不便利之處，尤其是依據設定的烘乾時間結束之後，有時衣服還是溼的；不然就是設定的時間過長以至於浪費了許多電力。這次打算使用[LinkIt One](http://labs.mediatek.com/site/global/developer_tools/mediatek_linkit/whatis_linkit/index.gsp)來做一個裝置，可以在烘衣機上先設定一定足夠烘乾衣服的時間，但當衣服已經烘好時，提前結束烘乾的行程以節省電力。

![timer](https://cloud.githubusercontent.com/assets/12403337/9374429/8a13a69c-4728-11e5-9f46-1829d025056e.jpg)

這次要運用LinkIt One現成的範例來修改，以迭代（iterate）的方式進行，並儘量減少程式碼（雖然我喜歡寫程式），逐步建構出一個「外掛」的烘衣機控制裝置。雖然直接取代原本的定時開關感覺比較厲害，但仍以不拆開機器為原則，先完成這個版本。



在學習各種開發板的使用時，溫溼度計、繼電器...是常看到的範例，有趣的是這些範例可以用在這裡做出一個有用的裝置的！
以下分享一下使用LinkIt One搭配 [MediaTek Cloud Sandbox (MCS)](https://mcs.mediatek.com/zh-TW) 以及我邊做邊學的過程！在這篇會使用到：
* 在 LinkIt One 使用溫溼度感應器
* 透過 WiFi 連上網路
* 運用 MediaTek Cloud Sandbox
* 透過 RESTFul API 傳送、取得資料

## 想法
在衣服烘乾的過程中，烘乾機所排出的空氣溼度應該會逐漸降低，直到持續在一個溼度不再降低時，應該就是已經烘好了（這是一個假設），此時就可以切斷烘乾機的電源。
先用 LinkIt One上實作一個雛型並測試，試看看這個概念是否可行，然後做出一個實用的裝置。步驟如下：
* 步驟 1：使用 [LinkIt One + Grove](http://www.seeedstudio.com/depot/Grove-Starter-Kit-for-LinkIt-ONE-p-2028.html) - Temperature & Humidity Sensor Pro 偵測溫、溼度。
* 步驟 2：讓LinkIt One連接 WiFi，將溫溼度數據傳送到MCS，觀察自開始烘乾到結束時溫溼度的變化，以確認烘衣機排氣的溼度可以用來決定是否提前關閉烘衣機。MCS在這個階段的主要任務是紀錄資料，然後運用這些資料驗證可行性。
* 步驟 3：若驗證上述方式是可行的，開始規劃程式，並加入繼電器用來控制電源。
* 步驟 4：藉著這個裝置可以上網的能力，增加更多功能：例如即時通知、耗電量統計...


## 動手做
## 1. 準備開發環境：
* 設定方式可以參考葉難老師的[說明](http://yehnan.blogspot.tw/2015/03/linkit-one_17.html)。
* 我的環境是Windows 8.1安裝在VertualBox上面，USB Driver安裝一直有問題。後來又下載了mediatek_linkit_windows-com-port-driver.zip 。在安裝之後，仍然未能解決。只好在裝置管理員中，手動將driver指向C:\Program Files (x86)\MediaTek\MediaTek inbox_COM_Driver\driver\v1.1032.1 之後才正常了。
* 本文實作的環境是 Arduino 1.5.7 + MediaTek LinkIt ONE SDK 1.1.05 (for Arduino)

##2. 使用溫溼度感測器 - 直接使用範例程式
使用 [Grove Starter Kit](http://www.seeedstudio.com/depot/Grove-Starter-Kit-for-LinkIt-ONE-p-2028.html)，連麵包板都省了！  
首先，找來
[Grove Starter Kit for LinkIt ONE 的程式範例](https://github.com/Seeed-Studio/Grove_Starter_Kit_For_LinkIt)，可以將這整包下載，解壓縮後複製到Arduino IDE安裝路徑下的example裏面，接著就可以方便地運用了。
![2015-08-03 11 23 01](https://cloud.githubusercontent.com/assets/12403337/9030220/47977ad6-39d2-11e5-9416-98e888ec1d23.png )
 <br><br>

直接用Grove_Temp_Humi_Sensor.ino這個範例程式。執行後，如圖：可以看到可以得知目前的溫溼度。
（如果執行時出現「找不到 DHT.h ...」這樣的錯誤訊息，可以將這個[資料夾](https://github.com/Seeed-Studio/Grove_Starter_Kit_For_LinkIt/tree/master/libraries/Humidity_Temperature_Sensor)，複製到Arduino IDE安裝路徑下的library裏面）


接著開始嘗試把這些數據透過WiFi連上網路傳送出去。

## 3. 使用 Wifi - 直接使用範例程式，加上非常小的修改

還是一樣，從範例開始。當安裝LinkIt One SDK  時，便已經安裝了一些範例。
如下所示使用WiFiWebClient這個範例程式。
![2015-08-03 13 16 01](https://cloud.githubusercontent.com/assets/12403337/9031202/cfa146a0-39e1-11e5-85fa-dab9b52b008e.png)
然後依據個人實際環境去修改21~24行，尤其是WIFI_AUTH 要看清楚不要設定錯了啊！

````c
#define WIFI_AP "your_ap_ssid"
#define WIFI_PASSWORD "your_password"
#define WIFI_AUTH LWIFI_WEP  // choose from LWIFI_OPEN, LWIFI_WPA, or LWIFI_WEP.
#define SITE_URL "arduino.cc"
````

另外修改了第51行
```c
  //c.println("GET / HTTP/1.1"); <--原來的
  c.println("GET /asciilogo.txt  HTTP/1.1");
```

執行時如果沒問題就可以看到以下的結果：

![2015-08-03 13 14 03](https://cloud.githubusercontent.com/assets/12403337/9031190/8e69eed0-39e1-11e5-9d8c-789920859a99.png)


## 4. 將溫溼度感測器的數值送到 MCS -　設定以及程式碼
###設定
先在[MediaTek Cloud Sandbox (MCS)](https://mcs.mediatek.com/zh-TW)註冊一個帳號[（操作說明）](https://mcs.mediatek.com/resources/zh-TW/latest/tutorial)。

接著，為將要使用的溫、溼度資料做一些設定，流程如後：


### a. 來到[產品原型列表](https://mcs.mediatek.com/v2console/zh-TW/development)創建一個產品原型然後儲存。
![2015-08-03 13 47 46](https://cloud.githubusercontent.com/assets/12403337/9031490/476757a2-39e6-11e5-973e-bd8b84bdba85.png)



![2015-08-03 13 50 06](https://cloud.githubusercontent.com/assets/12403337/9031513/99b31410-39e6-11e5-92ad-e98b63b8cc31.png)

###b.  設定資料通道與測試裝置
接下來新增/設定溫度與溼度兩個資料通道。
![2015-08-03 13 53 14](https://cloud.githubusercontent.com/assets/12403337/9031543/047c6652-39e7-11e5-9fcb-cef70a1119af.png)
按下「詳情」  ，然後「新增」 ，接著再新增「顯示器」類型的資料通道。
![2015-08-03 13 56 49](https://cloud.githubusercontent.com/assets/12403337/9031586/8b72c750-39e7-11e5-8a00-679512081347.png)

![2015-08-03 15 37 48](https://cloud.githubusercontent.com/assets/12403337/9053522/6d02966c-3aa8-11e5-830f-307ca842f4b3.png)
![2015-08-03 15 37 16](https://cloud.githubusercontent.com/assets/12403337/9053523/6ddc37fa-3aa8-11e5-9478-e2facc6c820c.png)
此時，產品原型（資料的模型）設置好了！可以這麼說：「有一個原型 -- "*DryerController_LinkItOne" *，它包含了*Temparature* 以及*Humidity*  這兩個資料通道」。

###c. 設定測試裝置
資料的原型設置完成之後，可以建立一或多個「測試裝置」使用此一資料模型。
按右上角的創建測試裝置，然後輸入裝置名稱。
![2015-08-03 14 16 19](https://cloud.githubusercontent.com/assets/12403337/9031863/104e908c-39eb-11e5-8aef-6831fe46a4c2.png)

完成！
![2015-08-03 14 28 43](https://cloud.githubusercontent.com/assets/12403337/9031932/011a9f10-39ec-11e5-8a45-f56c3663a2e8.png)

## 測試
###a. 試著將資料送到MCS
剛才已經會在LinkIt One運用WiFi連上網路，接下來試著將資料經由LinkIt One送往MCS。
 MCS本身可以依據我們的設定產生範例的程式碼提供參考。例如：在溫度這個資料通道的設定中打開API提示，然後選擇 Arduino，就可以看到範例。
 ![2015-08-04 13 02 26](https://cloud.githubusercontent.com/assets/12403337/9053566/18995862-3aa9-11e5-97f6-bac4467c0f0d.png)

 然後將這個範例　
 　　

` content.println("POST /mcs/v2/devices/D5mzZo0s/datapoints.csv HTTP/1.1"); `


這一行以後的程式複製貼到剛才測試WiFi的程式裏面，略做修改...如下：

```c
#include <LTask.h>
#include <LWiFi.h>
#include <LWiFiClient.h>

#define WIFI_AP "your_ap_ssid"
#define WIFI_PASSWORD "your_password"
#define WIFI_AUTH LWIFI_WPA  // choose from LWIFI_OPEN, LWIFI_WPA, or LWIFI_WEP.
#define SITE_URL "api.mediatek.com"

LWiFiClient content;

void setup()
{
  LTask.begin();
  LWiFi.begin();
  Serial.begin(9600);
  Serial.println("Connecting to AP");
  while (0 == LWiFi.connect(WIFI_AP, LWiFiLoginInfo(WIFI_AUTH, WIFI_PASSWORD)))
  {
    Serial.println("Connecting to AP...");
    delay(1000);
  }

  // keep retrying until connected to website
  Serial.println("Connecting to WebSite");
  while (0 == content.connect(SITE_URL, 80))
  {
    Serial.println("Re-Connecting to WebSite");
    delay(1000);
  }

  // send HTTP request, ends with 2 CR/LF
  Serial.println("send HTTP GET request");

  // ---- copied form MCS Arduino code hint ----
  content.println("POST /mcs/v2/devices/D5mzZo0s/datapoints.csv HTTP/1.1");
  String data = "Temperature,,28";
  int dataLength = data.length();
  content.println("Host: api.mediatek.com");
  content.println("deviceKey: viNDQqm5aP6pp0uD");
  content.print("Content-Length: ");
  content.println(dataLength);
  content.println("Content-Type: text/csv");
  content.println("Connection: close");
  content.println();
  content.println(data);
// ---- ----

  // waiting for server response
  Serial.println("waiting HTTP response:");
  while (!content.available())
  {
    delay(100);
  }
}

boolean disconnectedMsg = false;

void loop()
{
  // Make sure we are connected, and dump the response content to Serial
  while (content)
  {
    int v = content.read();
    if (v != -1)
    {
      Serial.print((char)v);
    }
    else
    {
      Serial.println("no more content, disconnect");
      content.stop();
      while (1)
      {
        delay(1);
      }
    }

  }

  if (!disconnectedMsg)
  {
    Serial.println("disconnected by server");
    disconnectedMsg = true;
  }
  delay(500);
}

```
執行之後，就可以看到MCS上面剛剛設定的 Temparature 這個資料通道的數值變成了程式指定的數值，這裡是28度。
![2015-08-03 15 01 02](https://cloud.githubusercontent.com/assets/12403337/9032364/8075b6a6-39f0-11e5-8b22-4acdffd85079.png)

###b. 將溫溼度感測器的數據定時送往MCS
剛才，已經試過如何取得溼度感測器的數據，以及如何將資料傳送到MCS。先將這些程式碼先拼湊起來，如以下程式：
```c
#include <LTask.h>
#include <LWiFi.h>
#include <LWiFiClient.h>

#include "DHT.h"
#define DHTPIN 2     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)

#define WIFI_AP "zaku"
#define WIFI_PASSWORD "12345678"
#define WIFI_AUTH LWIFI_WPA  // choose from LWIFI_OPEN, LWIFI_WPA, or LWIFI_WEP.
#define SITE_URL "api.mediatek.com"

#define DEVICE_KEY "viNDQqwtghr6pp0uD"
#define DEVICE_ID "D5mzZo0s"

LWiFiClient content;
DHT dht(DHTPIN, DHTTYPE);
void setup()
{
  LTask.begin();
  LWiFi.begin();
  dht.begin();
  Serial.begin(9600);
  Serial.println("Connecting to AP");
  while (0 == LWiFi.connect(WIFI_AP, LWiFiLoginInfo(WIFI_AUTH, WIFI_PASSWORD)))
  {
    Serial.println("Waiting for connected to AP...");
    delay(1000);
  }
}

boolean disconnectedMsg = false;

void loop()
{
  if (LWiFi.status() == LWIFI_STATUS_DISCONNECTED) {
    Serial.println("Reconnected to AP...");
    while (0 == LWiFi.connect(WIFI_AP, LWiFiLoginInfo(WIFI_AUTH, WIFI_PASSWORD)))
    {
      Serial.println("Waiting for reconnected to AP...");
      delay(1000);
    }
  }
  float t = 0.0;
  float h = 0.0;

  if (dht.readHT(&t, &h))
  {
    Serial.println("------------------------------");
    Serial.print("temperature = ");
    Serial.println(t);
    sendValueToMCS(t, "Temperature");

    Serial.print("humidity = ");
    Serial.println(h);
    sendValueToMCS(h, "Humidity");

  }

  delay(10000);
}

void sendValueToMCS(float value, String channelId) {

  Serial.println("Connecting to WebSite");
  while (0 == content.connect(SITE_URL, 80))
  {
    Serial.println("Re-Connecting to WebSite");
    delay(1000);
  }

  Serial.println("send HTTP GET request");
  String action = "POST /mcs/v2/devices/";
  action += DEVICE_ID;
  action += "datapoints.csv HTTP/1.1";
  content.println(action);

  String data = channelId + ",,";
  data += value;
  Serial.println("send Data"  + data);
  int dataLength = data.length();

  content.println("Host: api.mediatek.com");
  content.print("deviceKey: ");
  content.println(DEVICE_KEY);
  content.print("Content-Length: ");
  content.println(dataLength);
  content.println("Content-Type: text/csv");
  content.println("Connection: close");
  content.println();
  content.println(data);

  // waiting for server response
  Serial.println("waiting HTTP response:");
  while (!content.available())
  {
    delay(100);
  }

  // Make sure we are connected, and dump the response content to Serial
  while (content)
  {
    int v = content.read();
    if (v != -1)
    {
      Serial.print((char)v);
    }
    else
    {
      Serial.println("no more content, disconnect");
      content.stop();
      while (1)
      {
        delay(1);
      }
    }

  }

  if (!disconnectedMsg)
  {
    Serial.println("disconnected by server");
    disconnectedMsg = true;
  }
  delay(500);
}
```

我真的洗了一堆衣服，然後開始烘乾，並且把LinkIt One拿去測試。程式每10秒鐘開始將溫溼度傳送到MCS。此時，也可以看到MCS的Console上面數據不斷的變化。一個多小時之後，衣服乾了。   接著需要觀察這段時間的數據。雖然MCS Console上面可以察看歷史資料，也可以只顯示一個區間的數據。

![testing](https://cloud.githubusercontent.com/assets/12403337/9374432/8dd701f2-4728-11e5-8dd1-5af07ae0299b.jpg)

![2015-08-04 18 31 08](https://cloud.githubusercontent.com/assets/12403337/9058838/05cdfc6e-3ad7-11e5-8c81-83792868b28b.png)

不過就是沒辦法順利顯示所期望的區間。不過，還是可以透過RESTful的API來取得（[說明文件](https://mcs.mediatek.com/resources/zh-TW/latest/api_references/)）：
我用以下這兩個URL來取得資料：
* https://api.mediatek.com/mcs/v2/devices/D5mzZo0s/datachannels/Humidity/datapoints.csv?start=1438589400000&end=1438596600000&limit=1000
* https://api.mediatek.com/mcs/v2/devices/D5mzZo0s/datachannels/Temperature/datapoints.csv?start=1438589400000&end=1438596600000&limit=1000

有幾個需要注意的地方：
* 直接在瀏覽器上打開這兩個網址會得到 HTTP 401 的錯誤，需要另外在HTTP GET 的 header 中加入 DeviceKey。
可以用[Postman](https://chrome.google.com/webstore/detail/postman/fhbjgbiflinjbdggehcddcbncdddomop)之類的工具，或是用[https://www.hurl.it](https://www.hurl.it)輔助。
![2015-08-04 17 25 47](https://cloud.githubusercontent.com/assets/12403337/9058100/43f85184-3ad1-11e5-954c-d6892c15dab6.png)
* 在 API的說明文件中指出，若有指定區間則limit這個參數是沒有作用的。但實際上若不用limit指定數量，就只會顯示一筆資料。

資料取得之後，用熟悉的試算表工具把這些數據以圖表方式顯現。
可以發現溫度會不斷上升，而溼度會不斷下降，而在衣服烘乾時趨於穩定（不過似乎太早結束實驗了）。

![2015-08-04 17 38 33](https://cloud.githubusercontent.com/assets/12403337/9058099/42fdc1c4-3ad1-11e5-894c-c116fdba1ec9.png)

至此，可以做到運用溫溼度感測器的數據來判斷衣服是否已經烘乾。不過有兩個問題：
* 在測試過程中LinkIt One到WiFi AP 經常有斷線的情況，只好手動重置，也造成了資料不連續。
* 資料的抖動（jitter）過大，有可能會造成誤判。


下階段要做以下的改善：
* 當程式發現網路斷線時，必須能夠自動恢復連線。
* 用程式實現低通濾波器消除資料的抖動，讓溫、溼度數據的曲線變得平滑，易於判斷。


（待續）
