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

### 2.4.1. Diagrama estructural

El diagrama estructural muestra los componentes principales del sistema: el sensor ultrasónico HC-SR04 encargado de medir la distancia, el microcontrolador Arduino responsable del procesamiento de los datos y la clasificación del rango de distancia, y el sistema de LEDs que indica visualmente el resultado de la medición

<img width="1440" height="1140" alt="image" src="https://github.com/user-attachments/assets/fa109679-8eec-4fca-90c1-2326ebb89600" />

### 2.4.2. Diagrama de comportamiento

Diagrama de comportamiento del algoritmo de medición y clasificación de distancia, conexion con el servidor y conexion wifi.

<img width="1440" height="1740" alt="image" src="https://github.com/user-attachments/assets/9639e8fe-78b4-46b5-bfbf-b3b08ae5a934" />


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

# **4. Pruebas y Validaciones**

## 4.1. Objetivo de las pruebas

El objetivo de las pruebas realizadas es verificar el correcto funcionamiento del sistema IoT desarrollado, evaluando de manera integral la adquisición de datos, la comunicación mediante MQTT, la interacción con aplicaciones externas y la ejecución de acciones en los actuadores.

En particular, se busca validar que:

- El objeto inteligente mide y transmite correctamente la distancia.
- La comunicación mediante el protocolo MQTT es funcional y estable.
- Los comandos enviados desde diferentes interfaces (aplicación móvil e IA) son correctamente interpretados y ejecutados.
- El sistema mantiene su operatividad ante condiciones normales y escenarios de reconexión.

## 4.2. Metodología de pruebas

Las pruebas se realizaron utilizando el sistema completo en funcionamiento, compuesto por el objeto inteligente basado en ESP32, el broker MQTT en la nube, la aplicación móvil IoT MQTT Panel y el servidor intermedio en Python con integración de herramientas de IA.

El procedimiento general consistió en:

1. Colocar objetos a distintas distancias frente al sensor ultrasónico.
2. Observar los valores medidos y publicados en el tópico MQTT correspondiente.
3. Verificar la visualización de los datos en la aplicación móvil.
4. Enviar comandos desde la aplicación móvil y desde la herramienta de IA.
5. Comprobar la recepción de los comandos en el ESP32 y la respuesta de los LEDs.
6. Simular eventos de desconexión de red para evaluar la capacidad de reconexión del sistema.
7. Analizar los registros del monitor serie para validar tiempos de respuesta y manejo de eventos.

A diferencia de un enfoque basado únicamente en mediciones numéricas, las pruebas se orientaron a la verificación funcional del sistema como un todo, considerando la interacción entre sus componentes.

## 4.3 Validación de la comunicación

Durante la ejecución de las pruebas se pudo observar que el sistema presenta un comportamiento coherente con la arquitectura propuesta basada en MQTT.

El objeto inteligente logra establecer conexión con el broker en la nube y mantener comunicación bidireccional, publicando datos del sensor y recibiendo comandos de control. La publicación de datos se realiza de forma eficiente, ya que únicamente se envían mensajes cuando se detecta un cambio en el rango de distancia, evitando tráfico innecesario.

La aplicación móvil permite visualizar la información en tiempo real mediante componentes gráficos, lo que facilita la interpretación de los datos por parte del usuario. Asimismo, los comandos enviados desde esta interfaz son correctamente transmitidos a través del broker y ejecutados por el dispositivo.

En cuanto a la integración con la herramienta de IA, se verificó que las instrucciones en lenguaje natural pueden ser interpretadas y transformadas en comandos MQTT válidos, los cuales son enviados al sistema y ejecutados de manera adecuada. De igual forma, la consulta de datos permite recuperar el último valor de distancia registrado, evidenciando la correcta integración entre el servidor intermedio y el flujo de datos MQTT.

Adicionalmente, se comprobó que el sistema es capaz de recuperar la conexión con el broker tras una interrupción de red, reanudando su funcionamiento sin intervención manual, lo que demuestra un nivel adecuado de robustez.

## 4.4 Registro de pruebas

Durante las pruebas se obtuvieron los siguientes resultados.

### 4.4.1 Precisión del sensor ultrasónico

<img width="572" height="667" alt="image (1)" src="https://github.com/user-attachments/assets/681f87d4-d681-4572-a666-7415a35786e5" />

### 4.4.2 Medición y publicación de datos

| **Caso** | **Distancia medida** | **Comportamiento esperado** | **Resultado observado** |
| --- | --- | --- | --- |
| 1 | < 2 cm | Medición inválida ignorada | No se publica dato |
| 2 | 25 cm | Publicación de distancia | Dato publicado |
| 3 | 80 cm | Publicación de distancia | Dato publicado |
| 4 | 120 cm | Publicación de distancia | Dato publicado |
| 5 | 110 cm | Sin cambio de rango | No se publica nuevo dato |
| 6 | 90 cm | Publicación de distancia | Dato publicado |

### 4.4.3 Validación de comunicación y control de actuadores

| **Caso** | **Origen del comando** | **Comando enviado** | **Resultado esperado** | **Resultado observado** |
| --- | --- | --- | --- | --- |
| 1 | App móvil | LED_RED_ON | LED rojo encendido | Correcto |
| 2 | App móvil | ALL_OFF | LEDs apagados | Correcto |
| 3 | Claude Desktop | LED_GREEN_ON | LED verde encendido | Correcto |
| 4 | Claude Desktop | LED_YELLOW_ON | LED amarillo encendido | Correcto |
| 5 | Claude Desktop | LED_RED_ON | LED rojo encendido | Correcto |
| 6 | Claude Desktop | ALL_OFF | LEDs apagados | Correcto |

### 4.4.4 Validación de interacción y consulta de datos con IA

| **Caso** | **Entrada en lenguaje natural** | **Acción esperada** | **Resultado observado** |
| --- | --- | --- | --- |
| 1 | “Turn on red led” | Envío de comando LED_RED_ON | LED rojo encendido |
| 2 | “Can you turn on green led?” | Envío de comando LED_GREEN_ON | LED verde encendido |
| 3 | “Turn off all leds” | Envío de comando ALL_OFF | LEDs apagados |
| 4 | “What is the distance” | Consulta del último valor | Valor mostrado correctamente |
| 5 | “Turn on red led, yellow led and green led, later turn off all leds” | Secuencia de LEDs y apagado | Secuencia correcta encendiendo y apagando los LEDs |

### 4.4.5 Validación de reconexión

| **Caso** | **Evento** | **Acción esperada** | **Resultado observado** |
| --- | --- | --- | --- |
| 1 | Pérdida de conexión | Intento de reconexión automática | Reconexión exitosa |
| 2 | Reconexión al broker | Suscripción a tópicos | Recepción de comandos restaurada |
| 3 | Conexión inicial | Client ID único | Identificador con sufijo aleatorio |

# **5. Resultados**

En esta sección se analizan los datos obtenidos durante las pruebas realizadas al sistema IoT, considerando tanto el comportamiento del sensor ultrasónico como la integración completa mediante comunicación MQTT.

A partir de las mediciones registradas en el registro de precisión del sensor ultrasónico, se evaluó la exactitud del sensor comparando la distancia real con la distancia medida por el dispositivo. Los resultados muestran que el sistema presenta un error absoluto promedio de aproximadamente 1.46 cm y un error porcentual promedio cercano al 2.23 %, valores que se encuentran dentro de un rango aceptable para sensores ultrasónicos de bajo costo.

Se observó que el error tiende a incrementarse ligeramente en distancias mayores, lo cual es consistente con el comportamiento esperado de este tipo de sensores, debido a factores como la dispersión de la señal ultrasónica, la calidad de la superficie del objeto y posibles interferencias en el entorno.

En cuanto al comportamiento del sistema basado en MQTT, se verificó que la publicación de datos se realiza de manera eficiente, ya que el objeto inteligente únicamente envía información cuando se detecta un cambio en el rango de distancia. Este enfoque reduce significativamente el tráfico de red sin afectar la disponibilidad de la información para el usuario.

La visualización de los datos en la aplicación móvil permitió confirmar que la información es recibida correctamente y presentada de forma gráfica en tiempo real. Asimismo, los comandos enviados desde la aplicación fueron ejecutados sin errores por el objeto inteligente, evidenciando la correcta implementación del mecanismo de suscripción y procesamiento de mensajes MQTT.

Respecto a la integración con la herramienta de inteligencia artificial, se comprobó que las instrucciones en lenguaje natural pueden ser interpretadas y convertidas en comandos válidos, los cuales son enviados al sistema y ejecutados correctamente. De igual forma, la consulta de datos permitió obtener el último valor registrado del sensor, validando la coherencia del flujo de información.

Finalmente, se observó que el sistema mantiene su funcionamiento tras interrupciones de red, logrando reconectarse automáticamente al broker MQTT y restablecer la comunicación, lo cual confirma el cumplimiento de los requisitos de robustez definidos.

# 6. Conclusiones

A partir del desarrollo e implementación del sistema IoT basado en MQTT, se concluye que el sistema cumple satisfactoriamente con los requerimientos funcionales y no funcionales planteados, logrando integrar de manera efectiva un objeto inteligente, una aplicación móvil y una herramienta de inteligencia artificial.

Las pruebas realizadas permitieron validar que el sensor ultrasónico ofrece un nivel de precisión adecuado, con un error promedio de 1.46 cm, lo cual es consistente con las limitaciones inherentes a este tipo de dispositivos. Este nivel de precisión resulta suficiente para aplicaciones de monitoreo y control en entornos no críticos.

Desde el punto de vista de la arquitectura, el uso del protocolo MQTT permitió implementar un sistema desacoplado, en el cual la adquisición de datos, la toma de decisiones y la ejecución de acciones se encuentran distribuidas entre distintos componentes. Este enfoque facilita la escalabilidad del sistema y permite su integración con múltiples interfaces, como aplicaciones móviles y herramientas de inteligencia artificial.

Asimismo, se verificó que el sistema responde correctamente a los comandos enviados desde diferentes fuentes, manteniendo coherencia en la ejecución de acciones sobre los actuadores. La integración con la herramienta de IA representa un valor añadido importante, ya que permite interactuar con el sistema mediante lenguaje natural, ampliando las posibilidades de uso.

Por otro lado, la implementación de mecanismos como la publicación basada en cambios de rango y la reconexión automática contribuye a mejorar la eficiencia y robustez del sistema, aspectos fundamentales en aplicaciones IoT.

En términos generales, el sistema desarrollado demuestra un funcionamiento estable, eficiente y correctamente estructurado, cumpliendo con los objetivos planteados en la actividad y evidenciando la correcta aplicación de conceptos de sistemas embebidos, comunicación en red y arquitectura distribuida.

# 7. Recomendaciones

A partir de los resultados obtenidos y del análisis del sistema, se proponen las siguientes recomendaciones para mejorar su desempeño y ampliar sus capacidades en futuras implementaciones:

- Incorporar la publicación del rango de distancia (RED, YELLOW, GREEN) como dato adicional en MQTT, facilitando su uso por aplicaciones externas o sistemas automatizados.
- Mejorar la seguridad de la comunicación MQTT mediante la validación de certificados TLS en el dispositivo, evitando configuraciones inseguras.
- Implementar mecanismos de confirmación de recepción de mensajes (ACK) para aumentar la confiabilidad en escenarios con mayor pérdida de paquetes.
- Optimizar la gestión del Client ID para reflejar correctamente el identificador dinámico en los registros del sistema.
- Considerar el uso de niveles de calidad de servicio (QoS) en MQTT para garantizar la entrega de mensajes en aplicaciones más críticas.
- Evaluar la incorporación de nuevos actuadores o sensores que amplíen la funcionalidad del sistema, permitiendo su adaptación a otros contextos de aplicación.

Estas recomendaciones permitirían mejorar la precisión, seguridad, confiabilidad y escalabilidad del sistema, convirtiéndolo en una solución más robusta para entornos IoT más exigentes.
