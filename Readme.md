# Preparation
0. Create a backup through home assistant and download it 
1. Install docker on raspberry pi by running `curl -sSL https://get.docker.com | sh`
2. Create compose.yaml based on below snippeds
3. Optional: copy files from add-on backup, e.g. core_matter_server.tar.gz, to their original location
4. Run `sudo docker compose up -d`

# Clean-up
1. Change device for zigbee stick. Home assistant will automatically move devices over
2. Re-register signal through http://homeassistant.local:8080/v1/qrcodelink?device_name=signal-api
3. It might be possible to retain matter devices by stopping the matter-server and replacing the newly created /usr/share/hassio/addons/data/core_matter_server/SomeNumber.json with the one from backup
4. Ensure to add /usr/share/signal-cli-config, /usr/share/hassio/addons/data/core_matter_server /usr/share/hassio/addons/config/core_matter_server etc. to a backup

# Debugging
- run `sudo docker ps` and then `sudo docker exec -it ContainerID bash`
- Run `sudo docker logs -f matter-server`

# Docker Compose.yaml
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
    container_name: signal-cli-rest-api
    restart: unless-stopped
    environment:
      - MODE=normal #supported modes: json-rpc, native, normal
      #- AUTO_RECEIVE_SCHEDULE=0 22 * * * #enable this parameter on demand (see description below)
    ports:
      - "8080:8080" #map docker port 8080 to host port 8080.
    volumes:
      - "/usr/share/signal-cli-config:/home/.local/share/signal-cli" #map "signal-cli-config" folder on host system into docker container. the folder contains the password and cryptographic keys when a new number is registered

## Matter-server from https://github.com/home-assistant-libs
  matter-server:
    image: ghcr.io/home-assistant-libs/python-matter-server:stable
    container_name: matter-server
    restart: unless-stopped
    network_mode: host
    privileged: true
    security_opt:
      - apparmor:unconfined
    volumes:
      - /usr/share/hassio/addons/data/core_matter_server:/data
      - /usr/share/hassio/addons/config/core_matter_server:/config
      - /run/dbus:/run/dbus:ro
      - /etc/localtime:/etc/localtime:ro
      - /dev/hci0  # bluetooth device, might not be necessary, just in case
```

# References
- Docker compose for HA from https://www.home-assistant.io/installation/raspberrypi-other
- Docker compose for whisper and piper from https://community.home-assistant.io/t/using-add-ons-with-home-assistant-core/567369/8
- Docker compose for matter etc. from https://community.home-assistant.io/t/using-matter-and-thread-in-a-dockerized-ha-instance/721088/7
- Note: Ports for wyoming below don't seem to be necessary if "they are on the same docker network"
- Great write up found at https://blog.maxaller.name/engineering/2025/08/10/thread-matter-home-assistant-docker.html
