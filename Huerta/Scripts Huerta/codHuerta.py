
# Pines seleccionados (GPIO):

# Entradas (Sensores): 23 , 24

# Salidas (Reles): 17 , 27


# ------------------------------------------------------------------------------

#Librerias

import RPi.GPIO as GPIO
import time
import ubidots



# Configuración de los pines de los reles |
RELE1_PIN = 17
RELE2_PIN = 27

# Configuración de los umbrales de humedad

HUMEDAD_MINIMA_RELE1 = 500  #Si esta por encima de este valor = Resequedad
HUMEDAD_MINIMA_RELE2 = 500  #Si esta por encima de este valor = Resequedad

# Configuración de los pines de los sensores de humedad
SENSOR_HUMEDAD_1_PIN = 23
SENSOR_HUMEDAD_2_PIN = 24

# Configuración de la conexión a Ubidots
TOKEN_UBIDOTS = "BBFF-x2TtFtcJIOqCkKcfy3tZZgFW5W80Hu"
SENSOR_HUMEDAD_1_ID_UBIDOTS = "64396fc807ab95000e3e5f39"
SENSOR_HUMEDAD_2_ID_UBIDOTS = "645f8274d0a262000d038057"
RELE1_SWITCH_ID_UBIDOTS = "643977c27c8183000e09253f"
RELE2_SWITCH_ID_UBIDOTS = "645f8251ce3cf2000c5e306c"

api = ubidots.ApiClient(token=TOKEN_UBIDOTS)
sensor_humedad_1 = api.get_variable(SENSOR_HUMEDAD_1_ID_UBIDOTS)
sensor_humedad_2 = api.get_variable(SENSOR_HUMEDAD_2_ID_UBIDOTS)
rele1_switch = api.get_variable(RELE1_SWITCH_ID_UBIDOTS)
rele2_switch = api.get_variable(RELE2_SWITCH_ID_UBIDOTS)

# Configuración de los pines GPIO
GPIO.setmode(GPIO.BCM)
GPIO.setup(RELE1_PIN, GPIO.OUT)
GPIO.setup(RELE2_PIN, GPIO.OUT) 
GPIO.setup(SENSOR_HUMEDAD_1_PIN, GPIO.IN)
GPIO.setup(SENSOR_HUMEDAD_2_PIN, GPIO.IN)

# Funciones para encender y apagar los reles
def encenderRele1():
    GPIO.output(RELE1_PIN, GPIO.HIGH)

def apagarRele1():
    GPIO.output(RELE1_PIN, GPIO.LOW)

def encenderRele2():
    GPIO.output(RELE2_PIN, GPIO.HIGH)

def apagarRele2():
    GPIO.output(RELE2_PIN, GPIO.LOW)

# Bucle principal del programa
while True:
    # Lee el estado actual de los switches en Ubidots
    rele1_estado = rele1_switch.get_values(1)[0]['value']
    rele2_estado = rele2_switch.get_values(1)[0]['value']

    # Si el switch del rele 1 está activado, enciende el rele
    if rele1_estado == 1:
        encenderRele1()
        time.sleep(3)
        apagarRele1()
    else:
        apagarRele1()

    # Si el switch del rele 2 está activado, enciende el rele
    if rele2_estado == 1:
        encenderRele2()
        time.sleep(3)
        apagarRele2()
    else:
        apagarRele2()

    # Lee la señal de los sensores de humedad
    
    humedad_sensor1 = GPIO.input(SENSOR_HUMEDAD_1_PIN)
    humedad_sensor2 = GPIO.input(SENSOR_HUMEDAD_2_PIN)
    
    print("Sensor1:  ",humedad_sensor1)
    print("Sensor2:  ",humedad_sensor2)
    
    # Evalúa la humedad y enciende el switch correspondiente
    """if humedad_sensor1 > HUMEDAD_MINIMA_RELE1:
        rele1_switch.save_value({'value': 1})
    else:
        rele1_switch.save_value({'value': 0})

    if humedad_sensor2 > HUMEDAD_MINIMA_RELE2:
        rele2_switch.save_value({'value': 1})
    else:
        rele2_switch.save_value({'value': 0})"""
    
    if humedad_sensor1 == 1:
        rele1_switch.save_value({'value': 1})
    else:
        rele1_switch.save_value({'value': 0})

    if humedad_sensor2 == 1:
        rele2_switch.save_value({'value': 1})
    else:
        rele2_switch.save_value({'value': 0})
        
    # Envía las mediciones de humedad a Ubidots
    sensor_humedad_1.save_value({'value': humedad_sensor1})
    sensor_humedad_2.save_value({'value': humedad_sensor2})

    # Espera 1 segundos antes de repetir el bucle
    time.sleep(1)
