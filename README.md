Control  de temperatura camara frigorifica
 trabajo practico NRO 2 la misma corresponde a la sección de la primera entrega "Finit State Machines" se entrega el mismo con el objetivo de recibir una devolucion.
 para poder mejorar el estado del mismo trabajo practico , para luego comenzar con la parte de codigo del mismo

<img width="3570" height="3221" alt="Image" src="https://github.com/user-attachments/assets/ab188216-98e2-4cfe-9420-c44dd93c41d1" />
<img width="4425" height="1731" alt="Image" src="https://github.com/user-attachments/assets/3e33d900-e501-4406-80d2-5f6eff3e3671" />
<img width="4425" height="1731" alt="Image" src="https://github.com/user-attachments/assets/3d5b7cdf-6120-4281-b3ba-75b3f3915a6c" />
[Nuevo_Microsoft_Word_Document.md](https://github.com/user-attachments/files/23617107/Nuevo_Microsoft_Word_Document.md)
PHPWord
* {font-family: Arial; font-size: 11pt;}
a.NoteRef {text-decoration: none;}
hr {height: 1px; padding: 0; margin: 1em 0; border: 0; border-top: 1px solid #CCC;}
table {border: 1px solid black; border-spacing: 0px; width : 100%;}
td {border: 1px solid black;}
.Normal {margin-bottom: 8pt;}
h1 {font-family: 'Times New Roman'; font-size: 24pt; font-weight: bold;}
h2 {font-family: 'Times New Roman'; font-size: 18pt; font-weight: bold;}
h3 {font-family: 'Times New Roman'; font-size: 13.5pt; font-weight: bold;}
h4 {font-family: 'Times New Roman'; font-size: 12pt; font-weight: bold;}
.Título 1 Car {font-family: 'Times New Roman'; font-size: 24pt; font-weight: bold;}
.Título 2 Car {font-family: 'Times New Roman'; font-size: 18pt; font-weight: bold;}
.Título 3 Car {font-family: 'Times New Roman'; font-size: 13.5pt; font-weight: bold;}
.Título 4 Car {font-family: 'Times New Roman'; font-size: 12pt; font-weight: bold;}
.Strong {font-weight: bold;}
.Normal (Web) {font-family: 'Times New Roman'; font-size: 12pt;}
.Emphasis {font-style: italic;}
MEMORIA DESCRIPTIVA DEL SISTEMA DE CONTROL AMBIENTAL

1\. Introducción

El presente informe tiene como objetivo describir el funcionamiento de un sistema de control ambiental desarrollado mediante una Máquina de Estados Finitos (FSM).  
El sistema está orientado a la regulación automática de tres variables ambientales —temperatura, humedad y calidad de aire— y a la supervisión del estado de la puerta del recinto.  
Asimismo, incorpora un mecanismo de seguridad que interrumpe el funcionamiento general ante la apertura prolongada de la puerta.

2\. Objetivos del Sistema

2.1 Objetivo General

Regular y mantener las condiciones ambientales de una cámara frigorífica dentro de valores previamente configurados, garantizando además la seguridad mediante la supervisión de la apertura de la puerta.

2.2 Objetivos Específicos

3\. Descripción General del Funcionamiento

El sistema opera mediante una secuencia de estados mutuamente excluyentes. Cada estado representa una condición operativa definida y permite ejecutar acciones específicas sobre los actuadores.

La FSM comienza en el estado INICIO, pasa a ESPERA/MONITOREO y desde allí evalúa continuamente los sensores para decidir si es necesario activar los mecanismos de control o supervisión.

4\. Definición de Estados

4.1 Estado INICIO

Corresponde al estado inicial del sistema.  
El sistema permanece inactivo hasta recibir la orden de START, tras lo cual se transfiere al estado principal:

➡️ ESPERA / MONITOREO

4.2 Estado ESPERA / MONITOREO

Es el estado base y el núcleo del funcionamiento del sistema.  
En este estado se realiza:

El sistema evalúa si alguna de las variables está fuera del rango permitido. Para ello utiliza parámetros configurables:  
SetT, SetH, SetCA, deltaT, deltaH, deltaCA.

Las transiciones posibles desde este estado son:

a) Control de Humedad

b) Control de Temperatura

c) Control de Calidad de Aire

d) Supervisión de Puerta

4.3 Estado HUMIDIFICANDO (Humidificador ON)

En este estado se activa el humidificador para incrementar la humedad del ambiente.

Transiciones:

4.4 Estado ENFRIANDO (Compresor ON)

Se activa el compresor con el fin de disminuir la temperatura.

Transiciones:

4.5 Estado VENTILANDO (Ventilador ON)

Se activa el ventilador para mejorar la calidad del aire en caso de que esté fuera de los valores aceptables.

Transiciones:

5\. Estado de Supervisión de Puerta

5.1 Estado PUERTA ABIERTA (Timer ON)

Este estado se activa inmediatamente ante cualquier apertura de la puerta, independientemente del estado previo del sistema.  
Aquí se lleva a cabo:

Transiciones:

6\. Estado de Seguridad

6.1 Estado EMERGENCIA\_APAGADO (Reset Manual)

Este estado se activa cuando la puerta permanece abierta más tiempo del permitido.  
Funciones principales:

Transición:

7\. Consideraciones de Diseño
