# Práctica 3 — Integración de Sensor y Actuador en un Objeto Inteligente con MQTT

**Carrera:** Ingeniería de Sistemas

**Docente**: Eduardo Enrique Marin Garcia

**Asignatura:** SIS-234 - Internet de las Cosas

**Integrantes**: 

- Laura Camacho Lipa
- Sergio Francisco Solis Luizaga
- Cristhian Butron Perez

---

# **1. Requerimientos Funcionales y No Funcionales**

## 1.1. Requerimientos Funcionales

**RF01:** El sistema deberá medir la distancia utilizando el sensor ultrasónico HC-SR04 en un rango de 2 cm a 150 cm.

**RF02:** El objeto inteligente deberá publicar la distancia medida en un tópico MQTT cada vez que detecte un cambio de rango.

**RF03:** El objeto inteligente deberá suscribirse a un tópico MQTT de comandos, desde el cual recibirá instrucciones para controlar los actuadores.

**RF04:** El sistema deberá interpretar los siguientes comandos recibidos vía MQTT:

| Comando | Acción |
| --- | --- |
| `LED_RED_ON` | Encender LED rojo |
| `LED_YELLOW_ON` | Encender LED amarillo |
| `LED_GREEN_ON` | Encender LED verde |
| `ALL_OFF` | Apagar todos los LEDs |

**RF05:** El sistema deberá permitir que una aplicación móvil (IoT MQTT Panel) visualice la distancia en forma gráfica y envíe comandos para controlar los LEDs.

**RF06:** El sistema deberá permitir que una herramienta de IA interprete lenguaje natural y convierta instrucciones en mensajes MQTT válidos.

**RF07:** La herramienta de IA deberá permitir consultar el valor de la distancia mediante lenguaje natural, obteniendo datos desde el sistema MQTT.

**RF08:** El sistema deberá actualizar el estado de los LEDs en función de los mensajes recibidos desde el broker MQTT.

---

## 1.2. Requerimientos No Funcionales

**RNF01:** El código deberá mantener una estructura modular basada en clases, con separación de responsabilidades entre sensor, comunicación y actuador.

**RNF02:** El sistema deberá reconectarse automáticamente al broker MQTT en caso de pérdida de conexión, con intentos cada 2000 ms.

**RNF03:** El objeto inteligente deberá ignorar mediciones fuera de rango, registrando el evento sin interrumpir la ejecución.

**RNF04:** El sistema deberá establecer conexión MQTT con un Client ID único que incluya el identificador del grupo, verificable en los logs de conexión del broker.

# **2. Diseño del Sistema**

## 2.1. Diagrama de bloques

El sistema funciona de forma simple: el sensor ultrasónico detecta qué tan cerca está un objeto y lo clasifica por colores según la distancia (rojo si está muy cerca, amarillo a una distancia media, verde si está más lejos, o fuera de rango si no entra en esos valores).

Esa información se envía constantemente a la nube mediante MQTT. Allí, un programa en Python recibe los datos y los guarda para que otros puedan consultarlos.

Por otro lado, desde una aplicación o consola se pueden enviar comandos para encender o apagar los LEDs (rojo, amarillo, verde o todos apagados). Estos comandos también viajan por la nube hasta el ESP32, que los recibe y enciende las luces correspondientes al instante.

En resumen, el sistema permite ver la distancia de un objeto y controlar las luces en tiempo real desde cualquier lugar usando la nube.

<img width="2048" height="114" alt="image" src="https://github.com/user-attachments/assets/72dea0ab-3f7c-4b7b-a258-bcf9eb2e44ed" />

## 2.2. Diagrama de circuito

El sistema electrónico fue representado mediante un diagrama esquemático elaborado en el software KiCad, donde se muestran las interconexiones entre el microcontrolador, el sensor ultrasónico y los elementos de señalización visual. El núcleo del circuito está constituido por el módulo ESP32-WROOM-32, el cual actúa como unidad de procesamiento y control del sistema.

Para la medición de distancia se emplea el sensor ultrasónico HC-SR04, el cual se conecta al microcontrolador mediante cuatro terminales.Asimismo, el sistema incluye tres diodos emisores de luz (LED) utilizados como indicadores visuales del rango de distancia detectado. Cada LED se conecta a un pin de salida digital del microcontrolador y se encuentra en serie con una resistencia de 200 Ω, cuya función es limitar la corriente que circula por el diodo para evitar su deterioro.

<img width="958" height="590" alt="image" src="https://github.com/user-attachments/assets/0b799f78-b4c4-40f4-b3ae-42ea3596f4bb" />

## 2.3. Diagrama de arquitectura del sistema

El diagrama muestra un sistema dividido en dos partes:

**Nivel local (WiFi):**

Hay dos dispositivos ESP32. Uno funciona como sensor, usando el ultrasónico para medir la distancia y clasificarla en colores (rojo, amarillo, verde o fuera de rango), enviando los datos constantemente. El otro funciona como actuador, recibiendo comandos para encender o apagar los LEDs de esos mismos colores. Ambos están conectados a la misma red WiFi.

**Nivel nube (Internet):**

En la nube hay un broker MQTT (HiveMQ) que actúa como intermediario: recibe los datos del sensor y envía los comandos hacia el actuador.

También hay un servidor en Python que se conecta a ese broker, guarda la información de la distancia y permite dos cosas: consultar los datos o enviar comandos a los LEDs.

Desde cualquier lugar, un cliente (como una consola o app) puede conectarse a ese servidor para ver la distancia o controlar las luces.

<img width="862" height="722" alt="image" src="https://github.com/user-attachments/assets/81476657-a5df-4784-9bd2-9261507eec75" />


## 2.4. Diagramas estructurales y de comportamiento
### 2.4.3. Diagrama de secuencia

El sistema usa arquitectura **publish/subscribe MQTT** donde el Broker HiveMQ actúa como intermediario central. El ESP32 publica mediciones del sensor ultrasónico solo cuando cambia el rango de distancia (optimización). Tanto la App Móvil como el Servidor MCP pueden enviar comandos a los LEDs publicando en `actuator/leds`. El MCP Server usa herramientas (tools) que Claude Desktop interpreta desde lenguaje natural, almacena el último valor del sensor localmente y puede consultar/controlar el dispositivo IoT mediante MQTT con autenticación TLS.

<img width="1040" height="1762" alt="image" src="https://github.com/user-attachments/assets/e61e1ba8-781d-4cd3-b367-880012d488ec" />

# **3. Implementación**

El sistema fue implementado utilizando un microcontrolador ESP32 como objeto inteligente, encargado de la adquisición de datos y control de actuadores, junto con un servidor intermedio basado en Python que actúa como interfaz para herramientas externas, incluyendo integración con inteligencia artificial.

La comunicación entre componentes se realiza mediante el protocolo MQTT sobre TLS, utilizando un broker en la nube (HiveMQ Cloud), lo que permite una arquitectura desacoplada y escalable.

El desarrollo se realizó en:

- Arduino IDE (C++) para el ESP32
- Python para el servidor MQTT intermedio

## 3.1. Diagrama de clases

El sistema propuesto se estructura en tres clases principales. La clase central del diseño es `MQTTManager`, la cual concentra la lógica de conectividad y mensajería. Esta clase encapsula los parámetros de configuración de red (credenciales Wi-Fi mediante `ssid` y `password`, dirección del broker, puerto y `clientId`), así como las instancias de cliente seguro (`WiFiClientSecure`) y de cliente MQTT (`PubSubClient`). Sus métodos públicos contemplan la inicialización del sistema (`begin()`), la publicación y suscripción a tópicos (`publish()`, `subscribe()`), el ciclo de mantenimiento de la conexión (`loop()`) y la asignación dinámica de funciones de retorno (`setCallback()`).

La clase `UltrasonicSensor` modela almacena los pines de disparo y eco (`trigPin`, `echoPin`) junto con el último valor medido (`lastRange`).  Por su parte, la clase `LedController` abstrae el manejo de un indicador luminoso RGB compuesto por tres pines de salida (`redPin`, `yellowPin`, `greenPin`). 

<img width="661" height="402" alt="MQTT-DiagramaClases drawio" src="https://github.com/user-attachments/assets/5703cdc7-753c-46fa-9bc1-79748763e819" />

## 3.2. Arquitectura de implementación

El sistema sigue una arquitectura distribuida basada en publicación/suscripción (publish/subscribe), compuesta por tres módulos principales:

- Objeto inteligente (ESP32): Mide la distancia mediante un sensor ultrasónico, publica datos al broker MQTT y recibe comandos para controlar actuadores.
- Broker MQTT (HiveMQ Cloud): Intermedia la comunicación entre publicadores y suscriptores.
- Servidor MQTT (Python + MCP): Permite la interacción con herramientas externas e IA. Traduce comandos de alto nivel a mensajes MQTT.

Esta arquitectura permite desacoplar completamente los componentes, facilitando escalabilidad y flexibilidad.

## 3.3 Implementación del objeto inteligente (ESP32)

El objeto inteligente fue desarrollado en C++ utilizando un enfoque orientado a objetos, con separación clara de responsabilidades.

- **Clase `UltrasonicSensor`:** Genera la señal ultrasónica, calcula la distancia en cm y filtra mediciones inválidas. Clasifica la distancia en rangos (RED, YELLOW, GREEN).
- **Clase `MQTTManager`:** Gestiona la conexión WiFi, establece conexión segura con el broker MQTT, publica datos del sensor, recibe comandos mediante suscripción e implementa reconexión automática
- **Clase `LedController`:** Controla los LEDs, interpreta comandos MQTT y garantiza que solo un LED esté activo a la vez

Sigue el siguiente flujo de operación:

1. El sensor mide la distancia periódicamente
2. Se valida la medición
3. Se clasifica en un rango
4. Se publica solo si el rango cambia
5. El sistema escucha comandos MQTT
6. Los LEDs se actualizan en función de los comandos

## 3.4 Implementación del servidor MQTT (Python)

El servidor MQTT fue desarrollado en Python como un componente intermedio encargado de facilitar la interacción entre el sistema IoT y aplicaciones externas, particularmente herramientas basadas en inteligencia artificial. A diferencia del objeto inteligente, cuya función principal es la adquisición de datos y ejecución de acciones físicas, este servidor cumple un rol de abstracción y mediación, permitiendo transformar solicitudes de alto nivel en operaciones concretas dentro del ecosistema MQTT.

El servidor fue implementado en Python utilizando:

- `paho-mqtt` para comunicación MQTT
- `asyncio` para manejo asíncrono
- MCP (Model Context Protocol) para exponer herramientas a IA

A través de este mecanismo, se exponen herramientas (tools) que pueden ser invocadas por sistemas externos, como modelos de lenguaje. Estas herramientas encapsulan funcionalidades específicas del sistema, tales como el envío de comandos a los actuadores (`send_led_command`) y la consulta del último valor de distancia registrado (`get_distance`).

Cuando se invoca una herramienta de control, el servidor traduce la solicitud en un mensaje MQTT válido, el cual es publicado en el tópico de comandos. Este mensaje es posteriormente recibido por el ESP32, cerrando así el ciclo de control. Por otro lado, las consultas de datos no implican comunicación adicional con el dispositivo, sino que se resuelven utilizando el estado almacenado, lo que reduce la latencia y la carga de red.

## 3.5 Comunicación MQTT

La comunicación entre los distintos componentes del sistema se basa en el protocolo MQTT, el cual sigue un modelo de publicación/suscripción que resulta especialmente adecuado para entornos IoT debido a su bajo consumo de recursos y su flexibilidad. En este esquema, los dispositivos no se comunican directamente entre sí, sino que intercambian información a través de un broker central, lo que permite desacoplar completamente los emisores y receptores de datos.

El sistema utiliza los siguientes tópicos:

- **Sensor:**
    - `sis234/grupo3/sensor/distance`
- **Comandos:**
    - `sis234/grupo3/actuator/leds`

## **3.6 Manejo de confiabilidad**

Para mejorar la confiabilidad del sistema, se implementaron los siguientes mecanismos:

- **Reconexión automática:** Intentos cada 2000 ms
- **Re-suscripción automática:** Tras reconexión
- **Filtrado de datos inválidos:** Evita ruido en mediciones
- **Procesamiento eficiente:** Publicación solo ante cambios relevantes

## **3.7 Organización del código**

El proyecto se encuentra estructurado de la siguiente manera:

```
main/
 ├── main.ino
 ├── UltrasonicSensor.h
 ├── UltrasonicSensor.cpp
 ├── LedController.h
 ├── LedController.cpp
 ├── MQTTManager.h
 ├── MQTTManager.cpp
 ├── config.h
 
MCP-MQTT/
 ├── MQTT_server/
	 ├── MQTT_server.py
```

Esta estructura permite una clara separación de responsabilidades y facilita el mantenimiento y escalabilidad del sistema.
