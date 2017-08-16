#include "AX.h"

void Motor::mover_motor(unsigned angulo) {
	if (this->set_parametro(Motor::POSICION_OBJETIVO, angulo))
		while (dxl_get_result() == AX::COMM_SUCCESS && dxl_read_byte(this->id, Motor::MOVIENDOSE));
	else
		std::cout << "ERROR (" << error() << "): Moviendo motor id:" << this->id << " a posicion: " << angulo << std::endl;
}
unsigned Motor::get_posicion()
{
	dxl_ping(this->id);
	if (dxl_get_result() == AX::COMM_SUCCESS)
	{
		unsigned posicion = dxl_read_word(id, POSICION_ACTUAL);
		if (dxl_get_result() == AX::COMM_SUCCESS)
			return posicion;
		else
			std::cout << "ERROR (" << error() << ") al obtener la posicion del motor id:" << this->id << std::endl;
	}
	return 0;
}

unsigned Motor::get_error()
{

	return 7;
}

bool AX::set_parametro(unsigned parametro, unsigned valor, bool byte)
{
	dxl_ping(this->id);
	if (dxl_get_result() == AX::COMM_SUCCESS)
	{
		if (byte)
			dxl_write_byte(id, parametro, valor);
		else
			dxl_write_word(id, parametro, valor);
		if (dxl_get_result() == AX::COMM_SUCCESS)
			return true;
	}
	else
		std::cout << error() << ". AX: " << this->id << std::endl;
	return false;
}

int AX::get_parametro(unsigned parametro, bool byte)
{
	dxl_ping(this->id);
	if (dxl_get_result() == AX::COMM_SUCCESS)
	{
		unsigned param;
		if (byte)
			param = dxl_read_byte(id, parametro);
		else
			param = dxl_read_byte(id, parametro);
		if (dxl_get_result() == AX::COMM_SUCCESS)
			return param;
	}
	return -1;
}

std::string AX::error()
{
	dxl_ping(id);
	switch (dxl_get_result())
	{
	case COMM_TXFAIL:
		return "COMM_TXFAIL";

	case COMM_TXERROR:
		return ("COMM_TXERROR");

	case COMM_RXFAIL:
		return ("COMM_RXFAIL");

	case COMM_RXWAITING:
		return("COMM_RXWAITING");

	case COMM_RXTIMEOUT:
		return("COMM_RXTIMEOUT");

	case COMM_RXCORRUPT:
		return("COMM_RXCORRUPT");
	default:
		return("COMM_SUCCESFUL");
	}
}

void Sensor::melodia(unsigned id_melodia)
{
	set_parametro(Sensor::TEMPORIZADOR_TIMBRE, 255);
	if (id_melodia > Sensor::MAXIMO_MELODIAS) //Maximo valor melodia
		id_melodia = Sensor::MAXIMO_MELODIAS;
	set_parametro(Sensor::TIMBRE, id_melodia);
	Sleep(10);
}

void Sensor::test_melodia()
{
	for (unsigned i = 0; i < Sensor::MAXIMO_MELODIAS; i++)
		melodia(i);
}

int Sensor::get_num_palmadas()
{
	return get_parametro(Sensor::CONTADOR_DETECION_DE_SONIDO);
}

bool Sensor::detectar_obstaculo(unsigned distancia_minima)
{
	std::cout	<< "Centro: " << get_parametro(Sensor::IR_DISPARO_CNTR) << std::endl
				<< "Derecha: " << get_parametro(Sensor::IR_DISPARO_DER) << std::endl
				<< "Izquierda: " << get_parametro(Sensor::IR_DISPARO_IZQ) << std::endl;
	Proximidad proximidad = detectar_obstaculo();
	if (proximidad.derecha > distancia_minima || proximidad.frente > distancia_minima || proximidad.izquierda > distancia_minima)
		return true;
	return false;
}
Proximidad Sensor::detectar_obstaculo()
{
	return Proximidad(get_parametro(Sensor::IR_DISPARO_CNTR), get_parametro(Sensor::IR_DISPARO_DER), get_parametro(Sensor::IR_DISPARO_IZQ));
}