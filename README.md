# 用 LinkIt 自己做一個烘衣機自動停止開關
前一陣子用了 18 年的烘衣機壞了，原本想修理一下繼續使用，不過考量安全問題，還是買了台新的機器。不過新機器在使用上有一點不太習慣與不便利之處。尤其是設定烘乾的時間，不是結束後衣服還是溼的，就是設定的時間過長以至於浪費了許多電力。
這次打算使用 LinkItOne來做一個裝置，當衣服已經烘好時，提前結束烘乾的行程以節省電力。在學習這些開發板的使用時，最常看到的範例就是，參溼度計、繼電器，但是這些範例用在這裡就成了一個有用的裝置了！


以不拆開機器為原則！


在 LinkIt One上實作一個雛型並測試，試看看這個概念是否可行。
步驟 1：使用 LinkIt One + Grove - Temperature & Humidity Sensor Pro 偵測溫、溼度。
步驟 2：讓LinkIt One連接 WiFi，將溫溼度數據傳送到 MCS，觀察自開始烘乾到結束時溫溼度的變化，以確認烘衣機排氣的溼度可以用來決定是否提前關閉烘衣機。
步驟 3：若驗證上述方式是可行的，再開始加入繼電器用來控制電源，以控制烘乾的時間。


##1. 準備開發環境：
* 設定方式可以參考葉難老師的[說明](http://yehnan.blogspot.tw/2015/03/linkit-one_17.html)。
* 可是我的環境是Windows8.1安裝在VertualBox上面，USB Driver安裝一直有問題。後來又下載了mediatek_linkit_windows-com-port-driver.zip 。在安裝之後，仍然未能解決。只好在裝置管理員中，手動將driver指向C:\Program Files (x86)\MediaTek\MediaTek inbox_COM_Driver\driver\v1.1032.1 之後才正常了。


* 我的環境是 Arduino 1.5.7 + MediaTek LinkIt ONE SDK 1.1.05 (for Arduino)

##2. 使用溫溼度感測器
首先，找來
[Grove Starter Kit for LinkIt ONE 的程式範例](https://github.com/Seeed-Studio/Grove_Starter_Kit_For_LinkIt)，我們可以將這整包下載，解壓縮後複製到Arduino IDE安裝路徑下的example裏面，接著就可以方便地運用了。
![2015-08-03 11 23 01](https://cloud.githubusercontent.com/assets/12403337/9030220/47977ad6-39d2-11e5-9416-98e888ec1d23.png)     
 <br><br>    
            
直接用Grove_Temp_Humi_Sensor.ino這個範例程式，如圖我們可以看到可以得知目前的溫溼度。   
（如果執行時出現「找不到 DHT.h ...」這樣的錯誤訊息，可以將這個[資料夾](https://github.com/Seeed-Studio/Grove_Starter_Kit_For_LinkIt/tree/master/libraries/Humidity_Temperature_Sensor)，複製到Arduino IDE安裝路徑下的library裏面）   

![2015-08-03 11 01 40](https://cloud.githubusercontent.com/assets/12403337/9030096/63807b64-39d0-11e5-913e-e92a92c5da1d.jpg)

接著我想要把這些數據透過WiFi傳出去。

## 3. 使用 Wifi

還是一樣，從範例開始。當我們安裝LinkIt One SDK  時，便已經安裝了一些範例。   
如下所示使用WiFiWebClient這個範例程式。   
![2015-08-03 13 16 01](https://cloud.githubusercontent.com/assets/12403337/9031202/cfa146a0-39e1-11e5-85fa-dab9b52b008e.png)   
然後依據個人實際環境去修改21~24行，尤其是WIFI_AUTH 要看清楚不要設定錯了啊！
```c
#define WIFI_AP "your_ap_ssid"
#define WIFI_PASSWORD "your_password"
#define WIFI_AUTH LWIFI_WEP  // choose from LWIFI_OPEN, LWIFI_WPA, or LWIFI_WEP.
#define SITE_URL "arduino.cc"
```
我自己另外修改了第51行
```c
  //c.println("GET / HTTP/1.1"); <--原來的
  c.println("GET /asciilogo.txt  HTTP/1.1");
```

如果沒問題就會出現以下的結果：

![2015-08-03 13 14 03](https://cloud.githubusercontent.com/assets/12403337/9031190/8e69eed0-39e1-11e5-9d8c-789920859a99.png)


## 4. 將溫溼度感測器的數值送到 MCS
先在[MediaTek Cloud Sandbox (MCS)](https://mcs.mediatek.com/zh-TW)註冊一個帳號，[用法](https://mcs.mediatek.com/resources/zh-TW/latest/tutorial)還蠻簡單的。
接著，為我們將要使用的溫、溼度資料做一些設定。   

##設定
### a. 來到[產品原型列表](https://mcs.mediatek.com/v2console/zh-TW/development)創建一個產品原型然後儲存。     
![2015-08-03 13 47 46](https://cloud.githubusercontent.com/assets/12403337/9031490/476757a2-39e6-11e5-973e-bd8b84bdba85.png)   
   
   
 
![2015-08-03 13 50 06](https://cloud.githubusercontent.com/assets/12403337/9031513/99b31410-39e6-11e5-92ad-e98b63b8cc31.png)   

###b.  設定資料通道與測試裝置   
接下來新增/設定溫度與溼度兩個資料通道。   
![2015-08-03 13 53 14](https://cloud.githubusercontent.com/assets/12403337/9031543/047c6652-39e7-11e5-9fcb-cef70a1119af.png)   
按下「詳情」  ，然後「新增」 ，接著再新增「顯示器」類型的資料通道。   
![2015-08-03 13 56 49](https://cloud.githubusercontent.com/assets/12403337/9031586/8b72c750-39e7-11e5-8a00-679512081347.png)
   
![2015-08-03 14 03 58](https://cloud.githubusercontent.com/assets/12403337/9031691/f14c64f4-39e8-11e5-91f7-08ae3e1a9371.png)   
![2015-08-03 14 04 57](https://cloud.githubusercontent.com/assets/12403337/9031692/f22e6f2a-39e8-11e5-969d-c8b0299a7567.png)   
此時，我們將產品原型（資料的模型）設置好了，也就是也一個裝置的雛型*「DryerController_LinkItOne」*，它包含了*Temparature* 以及*Humidity*  這兩個資料通道。

###c. 設定測試裝置   
資料的模型設置完成之後，我們可以建立一或多個測試裝置使用此一資料模型。     
按右上角的創建測試裝置，然後輸入裝置名稱。 
![2015-08-03 14 16 19](https://cloud.githubusercontent.com/assets/12403337/9031863/104e908c-39eb-11e5-8aef-6831fe46a4c2.png)
   
完成！   
![2015-08-03 14 28 43](https://cloud.githubusercontent.com/assets/12403337/9031932/011a9f10-39ec-11e5-8a45-f56c3663a2e8.png)

##d 程式
我們剛剛測試了WiFi的使用，接下來試著將資料經由LinkIt One送往MCS。   
 MCS本身也會依據我們的設定產生範例的程式碼提供參考。例如我們在溫度這個資料通道的設定中打開API提示，然後選擇 Arduino，就可以看到範例。然後我們將這個範例
```content.println("POST /mcs/v2/devices/D5mzZo0s/datapoints.csv HTTP/1.1"); ```
這一行以後的程式複製貼到剛才測試WiFi的程式裏面，略做些改之後。
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
執行之後，就可以看到MCS上面剛剛設定的裝置，Temparature這個資料通道的數值變成了程式指定的數值，這裡是28度。   
![2015-08-03 15 01 02](https://cloud.githubusercontent.com/assets/12403337/9032364/8075b6a6-39f0-11e5-8b22-4acdffd85079.png)
   
接著比照這個方式，將Sensor上得到的溫溼度的資料定時送往MCS。


15:40開始測試
#### 結合繼電器


 用Linked Connect 7681 實際製作
這是一個實用的裝置，所以不可能將

## 二、 用LinkIt Connect 7681 實際製作

