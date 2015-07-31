# 用 LinkIt 自己做一個烘衣機自動停止開關
前一陣子用了 18 年的烘衣機壞了，原本想修理一下繼續使用，不過考量安全問題，還是買了台新的機器。不過新機器在使用上有一點不太習慣與不便利之處。

## 一、先試看看這個概念是否可行
步驟 1：使用 LinkIt One + Grove - Temperature & Humidity Sensor Pro 偵測溫、溼度

### 在 LinkIt One上實作並測試
#### 準備開發環境：
* 設定方式可以參考葉難老師的說明 http://yehnan.blogspot.tw/2015/03/linkit-one_17.html
* 可是我的環境是Windows8.1安裝在VertualBox上面，USB Driver安裝一直有問題。後來又下載了mediatek_linkit_windows-com-port-driver.zip 。在安裝之後，仍然未能解決。只好在裝置管理員中，手動將driver指向C:\Program Files (x86)\MediaTek\MediaTek inbox_COM_Driver\driver\v1.1032.1 之後才正常了。

* 我的環境是 Arduino 1.5.7 + MediaTek LinkIt ONE SDK 1.1.05 (for Arduino)



## 二、 用LinkIt Connect 7681 實際製作
