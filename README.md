# SmartHome
IoT

## Run
Fast and easy way:

```bash
docker-compose up
```
Then check running containers:
```bash
docker container ls -a
```

Or
```bash
docker-compose up -d --build homeassistant
```

Or follow instructions below to run docker containers individually

### MQTT

#### Server
```bash
docker run -it --name="mosquitto" --net=host -p 1883:1883 --restart=always eclipse-mosquitto
```

#### Client
Useful for testing MQTT server and configuration.
This simple script will create a subscriber and a publisher, that post messages to the MQTT server in loop.
Subscriber then prints out the received messages.

It is written in Python, create a virtual environment to run it:
```bash
source venv/bin/activate
cd mqtt
pip install -r requirements.txt
python client.py
```

### Home Assistant
```bash
docker run --init -d --name="home-assistant" -e "TZ=Europe/Paris" -v ./hassconfig:/config --net=host -p 8123:8123 --restart=always homeassistant/home-assistant:stable
```
