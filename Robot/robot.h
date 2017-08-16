#pragma once
#include <windows.h>
#include <iostream>
#include <vector>
#include <conio.h>
#include <thread>
#include <csignal>
#include <assert.h> 
#include "dynamixel.h"
#include "AX.h"
//#include "vista.h"
#include "trayectorias.h"
#pragma comment(lib, "dynamixel.lib")


#define POSICION_HOME {206,818,252,772,480,544,512,512,512,512,464,560,436,588,554,470,512,512}
#define LIMITE_ANGULOS_CCW { 0, 0,40,40,50,50,60,60,90,90,30,120,55,150,125,60,100,70 }
#define LIMITE_ANGULOS_CW  { 300,300,230,230,250,250,220,220,200,200,175,300,150,245,240,175,220,200 }
#define POSICION_ZERO { 512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512 }
#define ABRIR_BRAZOS {542, 478, 378, 600, 328, 725 }
#define CERRAR_BRAZOS { 514, 502, 163, 859, 424, 590}
#define RELAX_BRAZOS {206,818,252,772,480,544}

enum busqueda {
	BUSQUEDA_SUAVE,		//Solo busca en 1M Baudios
	BUSQUEDA_COMPLETA	//Busca en toda la banda de Baudios. Mas lento
};



using namespace std;
class Robot {
public:
	Robot(unsigned num_motores = 0, unsigned num_sensores = 0, bool auto_busqueda = true)
	{

		signal(SIGINT, Robot::apagado);
		//vista::Vista interfaz_grafico;
		//hilo_vista = thread(&vista::Vista::render, &interfaz_grafico);

		if (dxl_initialize() == false) {
			cout << "ERROR: No se pudo realizar la conexion USB2Dynamixel." << endl;
			exit(EXIT_FAILURE);
		}
		cout << "Conexion USB2Dynamixel establecida." << endl;
		if (auto_busqueda) {
			pair<vector<Motor>, vector<Sensor>> motores_sensores = this->busqueda_AX();
			motores = motores_sensores.first;
			sensores = motores_sensores.second;
			this->set_parametro(Motor::PAR_MAXIMO, 1023);
			this->set_parametro(Motor::LIMITE_PAR, 1023);
			if (num_motores != 0 || num_sensores != 0) {
				if (motores.size() != num_motores || sensores.size() != num_sensores) {
					cout << "ERROR: El numero de motores es " << motores.size() << " y se esperaban " << num_motores << endl;
					cout << "ERROR: El numero de sensores es " << sensores.size() << " y se esperaban " << num_sensores << endl;
					exit(EXIT_FAILURE);
				}
			}
		}
		sensores.back().test_melodia();
		//posicion_home();
	}
	~Robot() {
		cout << "Conexion USB2Dynamixel cerrada" << endl;
		dxl_terminate();
		//out << "Cerrando ventana de graficos...\r";
		//hilo.join();
		//cout << "Ventana de graficos cerrada.\t" << endl;
	}

	bool posicion_home();
	bool agacharse();
	bool incorporarse();
	void marcha_adelante(unsigned pasos);
	void marcha_atras(unsigned pasos);
	void marcha_autonoma();
	void abrir_brazos();
	void cerrar_brazos();
	void coger_objeto();
	void dejar_objeto();
	void relax_brazos();
	void set_limite_angulos();
	void reset_limite_angulos();
	void pruebas();
	void test_articualciones();
	void interpretar_cmd(string);

private:
	std::vector<Motor> motores;
	std::vector<Sensor> sensores;
	thread hilo_vista;

	pair<vector<Motor>, vector<Sensor>> busqueda_AX(unsigned tipo_busqueda = BUSQUEDA_SUAVE);
	void mostrar_motores();
	bool mover_motor(unsigned id, unsigned angulo);
	bool mover_motor(vector<pair<unsigned, unsigned>>);
	bool mover_motor(vector<unsigned>);
	vector < pair<unsigned, unsigned>> get_posicion();
	bool set_posicion(vector < pair<unsigned, unsigned>>);
	bool set_posicion(vector < unsigned>);
	unsigned numero_motores() { return motores.size(); };
	//bool set_parametro(unsigned id, unsigned parametro, unsigned valor);
	bool set_parametro(unsigned parametro, unsigned valor);
	bool set_parametro(unsigned parametro, vector<unsigned> valor);
	void ejecutar_trayectoria(Trayectorias::Trayectoria, unsigned, std::function<bool()>);
	void ejecutar_trayectoria(Trayectorias::Trayectoria, std::function<bool()>);
	void informar_error_motores();
	static void apagado(int signum)
	{
		cout << "Desconectando...";
		exit(signum);
	}
};
bool es_parecido(vector<pair<unsigned, unsigned>>, vector<pair<unsigned, unsigned>>, double error = 50);