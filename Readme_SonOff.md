Most information are taken from https://sigmdel.ca/michel/ha/sonoff/sonoff_mini_en.html#curl

Test Relais
curl http://10.0.0.110:8081/zeroconf/switch -d '{"deviceid":"10009abcde","data":{"switch":"on"}}' {"seq":1,"error":0}

Unlock OTA
curl http://10.0.0.110:8081/zeroconf/ota_unlock -d '{"deviceid":"10009abcde","data":{}}'

Verify the OTA is Unlocked
curl http://10.0.0.110:8081/zeroconf/info -d '{"deviceid":"10009abcde","data":{}}'

Get SHA265 of the Binary
shasum -a 256 ESPHomeBackporchLightSwitch/.pioenvs/backporchlightswitch/firmware.bin

Start Simple Web Server
Unfortunately python3 -m http.server does not work so I used https://github.com/emikulic/darkhttpd
 ./darkhttpd ./EnvironmentalMonitoringSystem/ --port 8000

Flash New Firmware
curl http://10.0.0.110:8081/zeroconf/ota_flash -d '{"deviceid":"10009abcde","data": { "downloadUrl": "http://10.0.0.5:8000/ESPHomeBackporchLightSwitch/.pioenvs/backporchlightswitch/firmware.bin", "sha256sum": "8708b2d4097d9d4430f9049069a2d7f891865fe775dde6a769b160997540247d" }}'
Observe if the file is requested
10.0.0.110 - - [05/Jun/2021 10:43:15] "GET /ESPHomeBackporchLightSwitch/.pioenvs/backporchlightswitch/firmware.bin?deviceid=1001042697&ts=1481765933&sign=02649a8b723e4553d4613a01633d848e24119f9d73f8a1be1731663028eb1591 HTTP/1.1" 200 -

