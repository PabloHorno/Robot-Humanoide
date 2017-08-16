#include <iostream>
#include "robot.h"

void menu();

int main()
{
	Robot maximo(18, 1);
	//maximo.pruebas();
	maximo.posicion_home();
	string input;
	while (input != "salir")
	{
		if(!input.empty())
			std::cout << "Cmd recibido: " << input << std::endl;
		std::cout << "=================================================" << std::endl;
		std::cout << "\t\t\tMAXIMO" << std::endl;
		std::cout << "=================================================" << std::endl;

		std::cout << "Maximo esta a su disposicion." << std::endl << std::endl;

		menu();

		std::cout << std::endl << "Que desea que haga Maximo? ";
			std::cin >> input;

		maximo.interpretar_cmd(input);
		system("cls");
	}

	return 1;
}


void menu()
{
	std::cout << "adelante(n) :\tMaximo camina hacia adelante n pasos." << std::endl;
	std::cout << "atras(n) : \tMaximo camina hacia atras n pasos." << std::endl;
	std::cout << "izquierda(n) : \tMaximo camina hacia la izquierda n pasos." << std::endl;
	std::cout << "derecha(n) : \tMaximo camina hacia la derecha n pasos." << std::endl;
	std::cout << "home : \t\tMaximo vuelve a la posición home." << std::endl;
	std::cout << "agacharse : \tMaximo se agacha." << std::endl;
	std::cout << "levantarse : \tMaximo se incorpora." << std::endl;
	std::cout << "coger : \tMaximo coge un objeto con los brazos." << std::endl;
	std::cout << "dejar : \tDejar el objeto que tiene cogido." << std::endl;
	std::cout << "mod_palmadas : \tIdentifica acctividades por palmadas" << std::endl;
	std::cout << "mod_autonomo : \tMarcha autonoma" << std::endl;
}