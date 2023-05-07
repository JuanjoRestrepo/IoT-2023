
# Pines seleccionados (GPIO):

# Entradas (Sensores): 23 , 24

# Salidas (Reles): 17 , 27


# ------------------------------------------------------------------------------

#Librerias

import RPi.GPIO as GPIO
import time
import ubidots


# Configuración de los pines de los reles
RELE1_PIN = 17
RELE2_PIN = 27

# Configuración de los umbrales de humedad

HUMEDAD_MINIMA_RELE1 = 30  #MODIFICAR ESTOS UMBRALES
HUMEDAD_MINIMA_RELE2 = 40  #MODIFICAR ESTOS UMBRALES

# Configuración de los pines de los sensores de humedad
SENSOR_HUMEDAD_1_PIN = 23
SENSOR_HUMEDAD_2_PIN = 24

# Configuración de la conexión a Ubidots
TOKEN_UBIDOTS = "BBFF-i7TvCWTZwMkxRfdT98YbI5Xq8guFsW"
SENSOR_HUMEDAD_1_ID_UBIDOTS = "6456666dcfead64871d875be"
SENSOR_HUMEDAD_2_ID_UBIDOTS = "64566673f3af0f0b8b8a3099"
RELE1_SWITCH_ID_UBIDOTS = "645660b9fc27ea0cd32026f3"
RELE2_SWITCH_ID_UBIDOTS = "64566264bdba180ca7a45405"

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
        time.sleep(30)
        apagarRele1()
    else:
        apagarRele1()

    # Si el switch del rele 2 está activado, enciende el rele
    if rele2_estado == 1:
        encenderRele2()
        time.sleep(30)
        apagarRele2()
    else:
        apagarRele2()

    # Lee la señal de los sensores de humedad
    humedad_sensor1 = GPIO.input(SENSOR_HUMEDAD_1_PIN)
    humedad_sensor2 = GPIO.input(SENSOR_HUMEDAD_2_PIN)

    # Evalúa la humedad y enciende el switch correspondiente
    if humedad_sensor1 < HUMEDAD_MINIMA_RELE1:
        rele1_switch.save_value({'value': 1})
    else:
        rele1_switch.save_value({'value': 0})

    if humedad_sensor2 < HUMEDAD_MINIMA_RELE2:
        rele2_switch.save_value({'value': 1})
    else:
        rele2_switch.save_value({'value': 0})

    # Envía las mediciones de humedad a Ubidots
    sensor_humedad_1.save_value({'value': humedad_sensor1})
    sensor_humedad_2.save_value({'value': humedad_sensor2})

    # Espera 5 segundos antes de repetir el bucle
    time.sleep(1)
