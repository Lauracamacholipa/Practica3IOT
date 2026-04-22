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

**RF01:** El sistema deberá medir la distancia utilizando el sensor ultrasónico HC-SR04 en un rango de **2 cm a 150 cm**, descartando valores inválidos.

**RF02:** El objeto inteligente deberá **publicar la distancia medida** en un tópico MQTT cada vez que detecte un **cambio de rango**, utilizando un broker en la nube.

**RF03:** El objeto inteligente deberá **suscribirse a un tópico MQTT de comandos**, desde el cual recibirá instrucciones para controlar los actuadores.

**RF04:** El sistema deberá interpretar los siguientes comandos recibidos vía MQTT:

| Comando | Acción |
| --- | --- |
| `LED_RED_ON` | Encender LED rojo |
| `LED_YELLOW_ON` | Encender LED amarillo |
| `LED_GREEN_ON` | Encender LED verde |
| `ALL_OFF` | Apagar todos los LEDs |

**RF05:** El sistema deberá permitir que una **aplicación móvil (IoT MQTT Panel)** visualice la distancia en forma gráfica y envíe comandos para controlar los LEDs.

**RF06:** El sistema deberá permitir que una herramienta de IA interprete lenguaje natural y convierta instrucciones en mensajes MQTT válidos.

**RF07:** La herramienta de IA deberá permitir consultar el valor de la distancia mediante lenguaje natural, obteniendo datos desde el sistema MQTT.

**RF08:** El sistema deberá actualizar el estado de los LEDs en función de los mensajes recibidos desde el broker MQTT.

---

## 1.2. Requerimientos No Funcionales

**RNF01:** El código deberá mantener una **estructura modular basada en clases**, con separación de responsabilidades entre sensor, comunicación y actuador.

**RNF02:** El sistema deberá reconectarse automáticamente al broker MQTT en caso de pérdida de conexión, con intentos cada 2000 ms.

**RNF03:** El objeto inteligente deberá ignorar mediciones fuera de rango, registrando el evento sin interrumpir la ejecución.

**RNF04:** El sistema deberá establecer conexión MQTT con un **Client ID único** que incluya el identificador del grupo, verificable en los logs de conexión del broker.

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
