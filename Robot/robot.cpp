#include "robot.h"

void Robot::mostrar_motores() {
	if (motores.empty())
	{
		cout << "No hay motores conectados" << endl;
		return;
	}
	for (Motor motor : motores)
	{
		cout << "\tMotor[" << motor.id << "]" << endl;
		cout << "\t\t" << "Version: " << motor.version << endl;
		cout << "\t\t" << "Modelo: " << motor.modelo << endl;
	}
}
bool Robot::mover_motor(unsigned id, unsigned angulo) {
	for (Motor motor : motores) {
		if (motor.id == id) {
			motor.mover_motor(angulo);
			return true;
		}
	}
	return false;
}
bool Robot::mover_motor(vector<unsigned> posiciones)
{
	vector<pair<unsigned, unsigned>> aux;
	int i = 0;
	for (auto x : posiciones)
		aux.push_back({ ++i,x });

	return mover_motor(aux);
}
bool Robot::mover_motor(vector<pair<unsigned, unsigned>> instrucciones) {

	dxl_set_txpacket_id(BROADCAST_ID);
	dxl_set_txpacket_instruction(INST_SYNC_WRITE);
	dxl_set_txpacket_parameter(0, Motor::POSICION_OBJETIVO);
	dxl_set_txpacket_parameter(1, 2);
	for (unsigned i = 0; i < instrucciones.size(); i++)
	{
		dxl_set_txpacket_parameter(2 + 3 * i, instrucciones[i].first);
		dxl_set_txpacket_parameter(2 + 3 * i + 1, dxl_get_lowbyte(instrucciones[i].second));
		dxl_set_txpacket_parameter(2 + 3 * i + 2, dxl_get_highbyte(instrucciones[i].second));
	}
	dxl_set_txpacket_length((2 + 1)*instrucciones.size() + 4);
	dxl_txrx_packet();
	dxl_get_result();

	informar_error_motores();
	return true;
}
pair<vector<Motor>, vector<Sensor>> Robot::busqueda_AX(unsigned tipo) {
	int n = 0;
	pair<vector<Motor>, vector<Sensor>> motores_sensores;
	cout << "Buscando motores y sensores" << endl;
	for (int i = 1; i < 255; i++)
	{
		dxl_set_baud(i);
		std::cout << "==== Baudrate number: " << i << " =====\n" << std::endl;

		for (int id = 0; id < BROADCAST_ID; id++)
		{
			dxl_ping(id);
			if (dxl_get_result() == COMM_RXSUCCESS)
			{
				unsigned modelo = dxl_read_word(id, AX::NUMERO_MODELO);
				if (dxl_get_result() == COMM_RXSUCCESS)
					if (modelo == 12)
						motores_sensores.first.push_back(Motor(id));
					else
						motores_sensores.second.push_back(Sensor(id));
			}
		}
		cout << "Numero de motores: " << motores_sensores.first.size() << endl;
		if (tipo == BUSQUEDA_SUAVE)
			return motores_sensores;
	}
	return motores_sensores;
}
bool Robot::posicion_home() {
	return this->mover_motor(POSICION_HOME);
}

vector<pair<unsigned, unsigned>> Robot::get_posicion()
{
	vector<pair<unsigned, unsigned>> posicion;
	for (auto motor : motores)
		posicion.push_back({ motor.id,motor.get_posicion() });
	return posicion;
}

bool Robot::set_posicion(vector<pair<unsigned, unsigned>> posicion)
{
	for (auto pos : posicion)
		for (auto motor : motores)
			if (pos.first == motor.id)
				motor.mover_motor(pos.second);
	return true;
}
bool Robot::set_posicion(vector <unsigned> posicion) {
	mover_motor(posicion);
	return true;
}

bool Robot::set_parametro(unsigned parametro, unsigned valor)
{
	unsigned estado;
	dxl_set_txpacket_id(BROADCAST_ID);
	dxl_set_txpacket_instruction(INST_SYNC_WRITE);
	dxl_set_txpacket_parameter(0, parametro);
	dxl_set_txpacket_parameter(1, 2);
	for (unsigned i = 0; i < motores.size(); i++)
	{
		dxl_set_txpacket_parameter(2 + 3 * i, motores[i].id);
		dxl_set_txpacket_parameter(2 + 3 * i + 1, dxl_get_lowbyte(valor));
		dxl_set_txpacket_parameter(2 + 3 * i + 2, dxl_get_highbyte(valor));
	}
	dxl_set_txpacket_length((2 + 1)*motores.size() + 4);
	dxl_txrx_packet();
	estado = dxl_get_result();
	std::cout << estado << std::endl;
	return estado;
}
bool Robot::set_parametro(unsigned parametro, std::vector<unsigned> valor)
{
	if (motores.size() != valor.size())
	{
		std::cout << "ERROR: Numero de parametros (" << valor.size() << ") no concuerda con el numero de motores(" << motores.size() << ")" << std::endl;
		return false;
	}
	dxl_set_txpacket_id(BROADCAST_ID);
	dxl_set_txpacket_instruction(INST_SYNC_WRITE);
	dxl_set_txpacket_parameter(0, parametro);
	dxl_set_txpacket_parameter(1, 2);
	for (unsigned i = 0; i < motores.size(); i++)
	{
		dxl_set_txpacket_parameter(2 + 3 * i, motores[i].id);
		dxl_set_txpacket_parameter(2 + 3 * i + 1, dxl_get_lowbyte(valor[i]));
		dxl_set_txpacket_parameter(2 + 3 * i + 2, dxl_get_highbyte(valor[i]));
	}
	dxl_set_txpacket_length((2 + 1)*motores.size() + 4);
	dxl_txrx_packet();
	std::cout << dxl_get_result() << std::endl;
}

void Robot::ejecutar_trayectoria(Trayectorias::Trayectoria trayectoria, unsigned pasos, std::function<bool()> sync = []()->bool {return false; })
{
	unsigned iteraciones = 0;
	for (auto posicion : trayectoria.inicio)
	{
		mover_motor(posicion.second);
		Sleep(posicion.first * 1000 / 1.6);
	}
	while (iteraciones++ < pasos || sync()) {

		if (sensores.back().get_num_palmadas() > 2)
			break;

		for (auto posicion : trayectoria.bucle)
		{
			mover_motor(posicion.second);
			Sleep(posicion.first * 1000 / 1.6);
		}
	}
	for (auto posicion : trayectoria.fin)
	{
		mover_motor(posicion.second);
		Sleep(posicion.first * 1000 / 1.6);
	}
}
void Robot::ejecutar_trayectoria(Trayectorias::Trayectoria trayectoria, std::function<bool()> sync)
{
	if (sync())
		ejecutar_trayectoria(trayectoria, 0, sync);
}

void Robot::informar_error_motores()
{
	dxl_rx_packet();
	unsigned char id = gbStatusPacket[ID]; // Verify id
	unsigned error = gbStatusPacket[ERRBIT]; // Verify error bit
	if (error && ERRBIT_VOLTAGE)
	{
		cout << "ERROR: Voltaje de entrada fuera de rango" << endl;
	}
	if (error && ERRBIT_ANGLE)
	{
		cout << "ERROR: Limite de angulo fuera de rango" << endl;
	}
	if (error && ERRBIT_OVERHEAT)
	{
		cout << "ERROR: Sobrecalentamiento" << endl;
	}
	if (error && ERRBIT_RANGE)
	{
		cout << "ERROR: Parametro fuera de rango" << endl;
	}
	if (error && ERRBIT_CHECKSUM)
	{
		cout << "ERROR: Fallo de comunicacion" << endl;
	}
	if (error && ERRBIT_OVERLOAD)
	{
		cout << "ERROR: Sobrecarga de corriente" << endl;
	}
	if (error && ERRBIT_INSTRUCTION)
	{
		cout << "ERROR: Intruccion invalidad" << endl;
	}
}


bool Robot::agacharse()
{
	double h = 1;
	double alpha, beta;

	for (alpha = 90; alpha > 25; alpha = alpha - h)
	{
		vector <pair<unsigned, unsigned>> mover;

		beta = 2 * alpha;
		cout << "\rAlpha: " << alpha << " Beta: " << beta;


		mover.push_back({ 11, 512 - (90 - alpha + 15) * 1024 / 300 });
		mover.push_back({ 12, 512 + (90 - alpha + 15) * 1024 / 300 });
		mover.push_back({ 15, 512 + (90 - alpha) * 1024 / 300 });
		mover.push_back({ 16, 512 - (90 - alpha) * 1024 / 300 });

		mover.push_back({ 13, 512 - (180 - beta) * 1024 / 300 });
		mover.push_back({ 14, 512 + (180 - beta) * 1024 / 300 });

		this->mover_motor(mover);
		Sleep(30);
	}
	return true;
}

bool Robot::incorporarse()
{
	double h = 1;
	double alpha, beta;

	for (alpha = 25; alpha < 90; alpha = alpha + h)
	{
		vector <pair<unsigned, unsigned>> mover;

		beta = 2 * alpha;
		cout << "\rAlpha: " << alpha << " Beta: " << beta;


		mover.push_back({ 11, 512 - (90 - alpha + 15) * 1024 / 300 });
		mover.push_back({ 12, 512 + (90 - alpha + 15) * 1024 / 300 });
		mover.push_back({ 15, 512 + (90 - alpha) * 1024 / 300 });
		mover.push_back({ 16, 512 - (90 - alpha) * 1024 / 300 });

		mover.push_back({ 13, 512 - (180 - beta) * 1024 / 300 });
		mover.push_back({ 14, 512 + (180 - beta) * 1024 / 300 });

		this->mover_motor(mover);
		Sleep(30);
	}
	return true;
}


void Robot::marcha_adelante(unsigned pasos)
{
	ejecutar_trayectoria(Trayectorias::caminar, pasos);
}
void Robot::marcha_atras(unsigned pasos)
{
	ejecutar_trayectoria(Trayectorias::caminar_atras, pasos);
}

void Robot::marcha_autonoma()
{
	sensores.back().set_parametro(Sensor::param::CONTADOR_DETECION_DE_SONIDO, 0, true); //Reinicio contador de palmadas
	unsigned distancia_minima = 20;

	while (sensores.back().get_num_palmadas() < 3)
	{
		std::cout << "num_palmadas " << sensores.back().get_num_palmadas() << std::endl;
		ejecutar_trayectoria(Trayectorias::caminar, [this, distancia_minima]()->bool {return sensores.back().detectar_obstaculo().frente < distancia_minima; });

		if (sensores.back().get_num_palmadas() > 2)
			break;

		if (sensores.back().detectar_obstaculo().derecha < sensores.back().detectar_obstaculo().izquierda)
			ejecutar_trayectoria(Trayectorias::caminar_derecha, 2);
		else
			ejecutar_trayectoria(Trayectorias::caminar_izquierda, 3);
	}
	posicion_home();
	Sleep(500);
	posicion_home();
}


void Robot::set_limite_angulos()
{
	set_parametro(Motor::LIMITE_ANGULO_CCW, LIMITE_ANGULOS_CCW);
	set_parametro(Motor::LIMITE_ANGULO_CW, LIMITE_ANGULOS_CW);
}
void Robot::reset_limite_angulos()
{
	set_parametro(Motor::LIMITE_ANGULO_CCW, 1023);
	set_parametro(Motor::LIMITE_ANGULO_CW, 0);
}

void Robot::pruebas()
{
	auto posicion = get_posicion();
	for (auto pos : posicion)
		std::cout << pos.first << " " << pos.second << std::endl;
	exit(EXIT_SUCCESS);
}

void Robot::test_articualciones()
{
	cout << "Levante el robot del suelo y mantengalo hasta que se realice la comprobacion de todos los motores" << endl;
	Sleep(5000);
	for (auto motor : motores)
	{
		auto pos = motor.get_posicion();
		motor.mover_motor(0);
		motor.mover_motor(1023);
		motor.mover_motor(pos);
	}
}

void Robot::interpretar_cmd(string cmd)
{
	unsigned num_pasos;
	if (cmd == "adelante" || cmd == "atras" || cmd == "derecha" || cmd == "izquierda")
	{
		std::cout << "Numero de pasos?";
		std::cin >> num_pasos;
		if (cmd == "adelante")
			ejecutar_trayectoria(Trayectorias::caminar, num_pasos);
		else if (cmd == "atras")
			ejecutar_trayectoria(Trayectorias::caminar_atras, num_pasos);
		else if (cmd == "derecha")
			ejecutar_trayectoria(Trayectorias::caminar_derecha, num_pasos);
		else if (cmd == "izquierda")
			ejecutar_trayectoria(Trayectorias::caminar_izquierda, num_pasos);
	}
	if (cmd == "home")
		posicion_home();
	if (cmd == "agacharse")
		agacharse();
	if (cmd == "levantarse")
		incorporarse();
	if (cmd == "coger")
		coger_objeto();
	if (cmd == "dejar")
		dejar_objeto();
	if (cmd == "mod_autonomo")
		marcha_autonoma();
}

void Robot::abrir_brazos()
{
	mover_motor(ABRIR_BRAZOS);
}

void Robot::cerrar_brazos()
{
	mover_motor(CERRAR_BRAZOS);
}

void Robot::relax_brazos()
{
	mover_motor(RELAX_BRAZOS);
}

void Robot::coger_objeto()
{
	agacharse();
	abrir_brazos();
	Sleep(1000);
	cerrar_brazos();
	Sleep(1000);
	incorporarse();
}
void Robot::dejar_objeto()
{
	agacharse();
	abrir_brazos();
	Sleep(1000);
	incorporarse();
	relax_brazos();
}

bool es_parecido(std::vector<std::pair<unsigned, unsigned>> a, std::vector<std::pair<unsigned, unsigned>> b, double error)
{
	bool parecido = false;
	for (auto val_1 : a)
	{
		for (auto val_2 : b)
		{
			if (val_1.first == val_2.second)
				if (val_1.second < val_2.second + error && val_1.second > val_2.second - error)
					parecido = true;
				else
					return false;
		}
	}
	return parecido;
}