# AquariumMonitor
ESP8266で水槽の状態をモニタする話
## 仕様

* 水温、室温、湿度のモニタ
* センサデータのmunin-nodeでの出力機能
* LED、CO2、ブクブクのON/OFF
* 水温閾値設定による冷却ファン、エアコンON/OFF
* Web設定インターフェース

## デバイス
||型番|参考|
|:-:|:-:|:-:|
|マイコン|ESP8266||
|温湿度センサ|HDC1000|秋月電子のもの、終売品…|
|水温センサ|DS18B20+|砲弾型のものを使用|
|LCD|0.91インチOLED||
|A/D変換器|ADS1115|https://www.adafruit.com/product/1085 のコピー品|
|12Vスイッチ|MP4401|MOSFETアレイ|
|CO2リレー||Aliexで購入、12VでON/OFF可能なもの|
