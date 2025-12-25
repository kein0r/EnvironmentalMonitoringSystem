# Preparation
1. curl -sSL https://get.docker.com | sh
2. Create compose.yaml based on below snipped
3. sudo docker compose up -d

# Clean-up
1. Change device for zigbee stick. Home assistant will automatically move devices over
2. Re-register signa through http://homeassistant.local:8080/v1/qrcodelink?device_name=signal-api

Docker compose for HA from https://www.home-assistant.io/installation/raspberrypi-other
Docker compose for whisper and piper from https://community.home-assistant.io/t/using-add-ons-with-home-assistant-core/567369/8
Docker compose for matter etc. from https://community.home-assistant.io/t/using-matter-and-thread-in-a-dockerized-ha-instance/721088/7
Note: Ports below don't seem to be necessary if "they are on the same docker network"

```
services:
  homeassistant:
    container_name: homeassistant
    image: "ghcr.io/home-assistant/home-assistant:stable"
    volumes:
      - /usr/share/hassio:/config
      - /etc/localtime:/etc/localtime:ro
      - /run/dbus:/run/dbus:ro
    restart: unless-stopped
    privileged: true
    network_mode: host
    environment:
      TZ: America/Detroit
    devices:
      - /dev/serial/by-id/usb-Nabu_Casa_Home_Assistant_Connect_ZBT-1_b82a1b2c6f39ef11889c53f454516304-if00-port0:/dev/ZBT-1 # devices can't be symlinks

  signal-cli-rest-api:
    image: bbernhard/signal-cli-rest-api:latest
    environment:
      - MODE=normal #supported modes: json-rpc, native, normal
      #- AUTO_RECEIVE_SCHEDULE=0 22 * * * #enable this parameter on demand (see description below)
    ports:
      - "8080:8080" #map docker port 8080 to host port 8080.
    volumes:
      - "/usr/share/signal-cli-config:/home/.local/share/signal-cli" #map "signal-cli-config" folder on host system into docker container. the folder contains the password and cryptographic keys when a new number is registered
```
