Control  de temperatura camara frigorifica
 trabajo practico NRO 2 la misma corresponde a la sección de la primera entrega "Finit State Machines" se entrega el mismo con el objetivo de recibir una devolucion.
 para poder mejorar el estado del mismo trabajo practico , para luego comenzar con la parte de codigo del mismo

<img width="3570" height="3221" alt="Image" src="https://github.com/user-attachments/assets/ab188216-98e2-4cfe-9420-c44dd93c41d1" />
<img width="4425" height="1731" alt="Image" src="https://github.com/user-attachments/assets/3e33d900-e501-4406-80d2-5f6eff3e3671" />
<img width="4425" height="1731" alt="Image" src="https://github.com/user-attachments/assets/3d5b7cdf-6120-4281-b3ba-75b3f3915a6c" />

MEMORIA DESCRIPTIVA DEL SISTEMA DE CONTROL AMBIENTAL
1. Introducción

El presente informe tiene como objetivo describir el funcionamiento de un sistema de control ambiental desarrollado mediante una Máquina de Estados Finitos (FSM).
El sistema está orientado a la regulación automática de tres variables ambientales —temperatura, humedad y calidad de aire— y a la supervisión del estado de la puerta del recinto.
Asimismo, incorpora un mecanismo de seguridad que interrumpe el funcionamiento general ante la apertura prolongada de la puerta.

2. Objetivos del Sistema
   
2.1 Objetivo General

Regular y mantener las condiciones ambientales de una cámara frigorífica dentro de valores previamente configurados, garantizando además la seguridad mediante la supervisión de la apertura de la puerta.

2.2 Objetivos Específicos

Mantener la temperatura, humedad y calidad del aire dentro de los rangos deseados.

Activar los actuadores correspondientes (humidificador, compresor y ventilador) cuando las variables se desvían de los valores de referencia.

Detectar la apertura de la puerta y controlar que no permanezca abierta más tiempo del permitido.

Accionar un estado de emergencia que detenga el sistema ante condiciones inseguras.

Requerir un reset manual para reanudar operaciones tras una emergencia.

3. Descripción General del Funcionamiento

El sistema opera mediante una secuencia de estados mutuamente excluyentes. Cada estado representa una condición operativa definida y permite ejecutar acciones específicas sobre los actuadores.

La FSM comienza en el estado INICIO, pasa a ESPERA/MONITOREO y desde allí evalúa continuamente los sensores para decidir si es necesario activar los mecanismos de control o supervisión.

4. Definición de Estados
   
4.1 Estado INICIO

Corresponde al estado inicial del sistema.
El sistema permanece inactivo hasta recibir la orden de START, tras lo cual se transfiere al estado principal:

ESPERA / MONITOREO

4.2 Estado ESPERA / MONITOREO

Es el estado base y el núcleo del funcionamiento del sistema.
En este estado se realiza:

Lectura continua de temperatura (T)

Lectura de humedad (H)

Lectura de calidad de aire (CA)

Supervisión del estado de la puerta

El sistema evalúa si alguna de las variables está fuera del rango permitido. Para ello utiliza parámetros configurables:

SetT, SetH, SetCA

deltaT, deltaH, deltaCA

Transiciones desde este estado:

a) Control de Humedad

Si H < SetH − deltaH → HUMIDIFICANDO

b) Control de Temperatura

Si T > SetT + deltaT → ENFRIANDO

c) Control de Calidad de Aire

Si CA > SetCA + deltaCA → VENTILANDO

d) Supervisión de Puerta

Si la puerta se abre en cualquier momento → PUERTA ABIERTA

4.3 Estado HUMIDIFICANDO (Humidificador ON)

En este estado se activa el humidificador para incrementar la humedad del ambiente.

Transiciones:

Si H ≥ SetH + deltaH, la humedad vuelve al rango → ESPERA / MONITOREO

Si la puerta se abre → PUERTA ABIERTA

4.4 Estado ENFRIANDO (Compresor ON)

Se activa el compresor con el fin de disminuir la temperatura.

Transiciones:

Si T ≤ SetT − deltaT → ESPERA / MONITOREO

Si la puerta se abre → PUERTA ABIERTA

4.5 Estado VENTILANDO (Ventilador ON)

Se activa el ventilador para mejorar la calidad del aire en caso de que esté fuera de los valores aceptables.

Transiciones:

Si CA ≤ SetCA − deltaCA → ESPERA / MONITOREO

Si la puerta se abre → PUERTA ABIERTA

5. Estado de Supervisión de Puerta
   
5.1 Estado PUERTA ABIERTA (Timer ON)

Este estado se activa inmediatamente ante cualquier apertura de la puerta, independientemente del estado previo.
En este estado se realiza:

Activación de un temporizador que mide el tiempo de apertura.

Interrupción temporal del control ambiental.

Transiciones:

Si la puerta se cierra antes del tiempo máximo permitido → ESPERA / MONITOREO

Si el tiempo supera T_EXCESO → EMERGENCIA_APAGADO

6. Estado de Seguridad
   
6.1 Estado EMERGENCIA_APAGADO (Reset Manual)

Este estado se activa cuando la puerta permanece abierta más tiempo del permitido.

Funciones principales:

Desactivar todos los actuadores del sistema.

Bloquear la operación normal hasta que un operador realice el reset manual.

Transición:

Con RESET_MANUAL → ESPERA / MONITOREO

7. Consideraciones de Diseño

El uso de histéresis (deltaT, deltaH, deltaCA) evita cambios frecuentes de estado por pequeñas fluctuaciones en las variables.

El estado de supervisión de puerta tiene prioridad sobre los demás estados para garantizar la seguridad del recinto.

El estado de emergencia es irreversible sin intervención humana, lo cual previene fallos operativos peligrosos.
