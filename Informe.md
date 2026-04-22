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