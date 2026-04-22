import asyncio
import json
from mcp.server import Server
from mcp.server.stdio import stdio_server
from mcp.types import Tool, TextContent
import paho.mqtt.client as mqtt
import ssl



BROKER   = "2a343cb26d7749cba23d29071060df5a.s1.eu.hivemq.cloud"
PORT     = 8883
USERNAME = "grupo3"
PASSWORD = "Grupo3000"


TOPIC_COMMANDS = "sis234/grupo3/actuator/leds"
TOPIC_SENSOR   = "sis234/grupo3/sensor/distance"

# ─── State ────────────────────────────────────────────────────────────────────
last_distance = {"value": "No data yet"}

# ─── MQTT client setup ────────────────────────────────────────────────────────
mqtt_client = mqtt.Client(client_id="mcp-grupo3")
mqtt_client.username_pw_set(USERNAME, PASSWORD)
mqtt_client.tls_set(cert_reqs=ssl.CERT_NONE)
mqtt_client.tls_insecure_set(True)

def on_connect(client, userdata, flags, rc):
    if rc == 0:
        client.subscribe(TOPIC_SENSOR)

def on_message(client, userdata, msg):
    value = msg.payload.decode()
    print("Received:", value)
    last_distance["value"] = value  

mqtt_client.on_connect = on_connect
mqtt_client.on_message = on_message
mqtt_client.connect(BROKER, PORT)
mqtt_client.loop_start()

app = Server("mqtt-grupo3")

@app.list_tools()
async def list_tools():
    return [
        Tool(
            name="send_led_command",
            description="Send a command to control the LEDs on the ESP32. Valid commands: LED_RED_ON, LED_YELLOW_ON, LED_GREEN_ON, ALL_OFF",
            inputSchema={
                "type": "object",
                "properties": {
                    "command": {
                        "type": "string",
                        "enum": ["LED_RED_ON", "LED_YELLOW_ON", "LED_GREEN_ON", "ALL_OFF"]
                    }
                },
                "required": ["command"]
            }
        ),
        Tool(
            name="get_distance",
            description="Get the last distance value measured by the ultrasonic sensor on the ESP32",
            inputSchema={
                "type": "object",
                "properties": {}
            }
        )
    ]

@app.call_tool()
async def call_tool(name: str, arguments: dict):
    if name == "send_led_command":
        command = arguments["command"]
        mqtt_client.publish(TOPIC_COMMANDS, command)
        return [TextContent(type="text", text=f"Command '{command}' sent to ESP32")]

    if name == "get_distance":
        return [TextContent(type="text", text=f"Last distance: {last_distance['value']} cm")]

async def main():
    async with stdio_server() as (read_stream, write_stream):
        await app.run(read_stream, write_stream, app.create_initialization_options())

if __name__ == "__main__":
    asyncio.run(main())