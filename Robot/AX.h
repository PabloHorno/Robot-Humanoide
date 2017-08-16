#include <windows.h>
#include <iostream>
#include <vector>
#include <string>
#include "dynamixel.h"
#pragma comment(lib, "dynamixel.lib")

class AX {
public:
	unsigned id;
	unsigned version;
	unsigned modelo;
	AX(unsigned id) {
		this->id = id;
		int modelo = dxl_read_word(id, AX::NUMERO_MODELO);
		if (dxl_get_result() == AX::COMM_SUCCESS)
			this->modelo = modelo;
		else
			std:: cout << error();
		int version = dxl_read_byte(id, AX::VERSION_FIRMWARE);
		if (dxl_get_result() == AX::COMM_SUCCESS)
			this->version = version;
		else
			std::cout << error();
	}
	bool set_parametro(unsigned, unsigned, bool byte = false);
	int get_parametro(unsigned, bool byte = false);
	std::string error();
	enum {
		NUMERO_MODELO,
		COMM_SUCCESS,
		VERSION_FIRMWARE,
		ID_NUM,
	};
};

class Proximidad {
public:
	Proximidad() {};
	Proximidad(double frente, double derecha, double izquierda)
	{
		this->frente = frente;
		this->derecha = derecha;
		this->izquierda = izquierda;
	}
	double frente;
	double derecha;
	double izquierda;
};
class Motor : public AX
{
public:
	Motor(unsigned id) : AX(id) {};
	void mover_motor(unsigned);
	unsigned get_posicion();
	unsigned get_error();
private:
public:
	enum 
	{
		NUMERO_MODELO,
		VERSION_FIRMWARE = 2,
		ID_NUM,
		VELOCIDAD_MEDIA_TRANSFERENCIA = 4,
		RETORNA_TIEMPO_ESPERA,
		LIMITE_ANGULO_CW = 6,
		LIMITE_ANGULO_CCW = 8,
		LIMITE_TEMPERATURA = 11,
		LIMITE_TENSION_MIN,
		LIMITE_TENSION_MAX,
		PAR_MAXIMO = 14,
		ESTO_NIVEL_RETORNO = 16,
		ALARMA_LED = 17,
		ALARMA_DESACTIVACION,
		PAR_HABILITAR = 24,
		LED,
		MARGEN_CONTROL_CW,
		MARGEN_CONTROL_CCW,
		PENDIENT_CONTROL_CW,
		PENDIENT_CONTROL_CCW,
		POSICION_OBJETIVO,
		VELOCIDAD_MOVIMIENTO = 32,
		LIMITE_PAR = 34,
		POSICION_ACTUAL = 36,
		VELOCIDAD_ACTUAL = 38,
		CARGA_ACTUAL = 40,
		TENSION_ACTUAL = 42,
		TEMPERATURA_ACTUAL,
		INSTRUCCION_REGISTRADA,
		MOVIENDOSE = 46,
		BLOQUEO,
		MARTILLEO
	};
};

class Sensor:public AX {
public:
	Sensor(unsigned id) : AX(id) {};
	void melodia(unsigned id_melodia);
	void test_melodia();
	int get_num_palmadas();

	bool detectar_obstaculo(unsigned distancia_minima);
	Proximidad detectar_obstaculo();

	enum param{
		NUMERO_MODELO,
		VERSION_FIRMWARE = 2,
		ID_NUM,
		VELOCIDAD_MEDIA_TRANSFERENCIA = 4,
		RETORNA_TIEMPO_ESPERA,
		ESTADO_NIVEL_RETORNO = 16,
		MAXIMO_MELODIAS = 25,
		IR_DISPARO_IZQ = 26,
		IR_DISPARO_CNTR,
		IR_DISPARO_DER,
		DATOS_LUZ_IZQ,
		DATOS_LUZ_CNTR,
		DATOS_LUZ_DER,
		OBSTACULO_DETECTADO_IR,
		LUZ_DETECTADA,
		DATOS_DE_SONIDO = 35,
		MAXIMO_HUMBRAL_SONIDO,
		CONTADOR_DETECION_DE_SONIDO,
		TIEMPO_DETECCION_SONIDO,
		TIMBRE = 40,
		TEMPORIZADOR_TIMBRE,
		ORDEN_REGISTRADA = 44,
		CONTROL_REMOTO_RECIBIDO = 46,
		BLOQUEO,
		DATOS_RX_CONTROL_REMOTO,
		DATOS_TX_CONTROL_REMOTO = 50,
		COMPARACION_DETECCION_OBSTACULOS = 52,
		COMPARACION_DETECCION_LUZ
	};
};