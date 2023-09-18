# SFCM
Smart Factory Control and Measurement

Terdiri dari:
1. Data collector yang mengumpulkan data dari sensor listrik INA219, INA226, INA260, atau INA3221 menggunakan ESP32
   - mengontrol motor DC dari segi kecepatan, arah putaran (searah atau berlawanan arah jaum jam)
   - memonitor voltase dan arus yang digunakan motor DC
   - memonitor suhu dan getaran motor DC
   - mengirim data ke RabbitMQ
2. Data worker yang men-subscribe data dari RabbitMQ dan menyimpannya ke MongoDB
   Program ini dibuat dengan Python3, membutuhkan module pika dan pymongo. Agar dapat berjalan dengan baik, perlu dilakukan instalasi modul tersebut dengan cara:
   
   `pip install pika`
   
   `pip install pymongo`
   
3. Program PHP yang membaca data dari database MongoDB dan menyediakan data dalam format JSON
4. Halaman HTML dengan Chart.js yang memformat JSON diatas menjadi Line chart


## References
- EP32 Pinout from https://lastminuteengineers.com/esp32-pinout-reference/
- Arduino BTS7960 https://www.instructables.com/Motor-Driver-BTS7960-43A/
- https://wolles-elektronikkiste.de/en/ina226-current-and-power-sensor
