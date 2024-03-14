#include "../Comportamientos_Jugador/jugador.hpp"
#include <iostream>
using namespace std;

Action ComportamientoJugador::think(Sensores sensores)
{

	Action accion = actRUN;
	int a;

	// Actualización de las variables de esatdo
	switch (last_action)
	{
	case actWALK:		// Actualización en caso de avanzar
		switch (current_state.brujula)
		{
		case norte: current_state.fil--; break;			
		case noreste: current_state.fil--; current_state.col++; break;
		case este: current_state.col++; break;
		case sureste: current_state.fil++; current_state.col++; break;
		case sur: current_state.fil++; break;
		case suroeste: current_state.fil++; current_state.col--; break;
		case oeste: current_state.col--; break;
		case noroeste: current_state.fil--; current_state.col--; break;
		}
		break;
	
	case actRUN:		// Actualización en caso de correr
		break;
	
	case actTURN_SR:	// Actualización en caso de girar 45º a la derecha
		a = current_state.brujula;
		a = (a + 1) % 8;
		current_state.brujula = static_cast<Orientacion>(a);
		break;
	
	case actTURN_L:	   // Actualización en caso de girar 90º a la izquierda
		a = current_state.brujula;
		a = (a - 6) % 8;	// Es equivalente a (a-2+8) % 8.
		current_state.brujula = static_cast<Orientacion>(a);
		break;
	}

	// Decidir la nueva accion
	if( (sensores.terreno[2] == 'T' or sensores.terreno[2] == 'S') // Si puedo avanzar a la siguiente casilla
		and sensores.agentes[2] == '_' ){
			accion = actWALK;
	} else if(!girar_derecha){	
		accion = actTURN_L;
		girar_derecha = (rand()%2 == 0);
	} else {
		accion = actTURN_SR;
		girar_derecha = (rand()%2 == 0);
	}

/* 	// Mostrar el valor de los sensores
	cout << "Posicion: fila " << sensores.posF << " columna " << sensores.posC;
	switch (sensores.sentido)
	{
		case norte:	  cout << " Norte\n";	break;
		case noreste: cout << " Noreste\n";	break;
		case este:    cout << " Este\n";	break;
		case sureste: cout << " Sureste\n";	break;
		case sur:     cout << " Sur\n";	break;
		case suroeste:cout << " Suroeste\n";	break;
		case oeste:   cout << " Oeste\n";	break;
		case noroeste:cout << " Noroeste\n";	break;
	}
	cout << "Terreno: ";
	for (int i=0; i<sensores.terreno.size(); i++)
		cout << sensores.terreno[i];

	cout << "  Agentes: ";
	for (int i=0; i<sensores.agentes.size(); i++)
		cout << sensores.agentes[i];

	cout << "\nColision: " << sensores.colision;
	cout << "  Reset: " << sensores.reset;
	cout << "  Vida: " << sensores.vida << endl<< endl; */

	// Recordar la ultima accion
	last_action = accion;

	return accion;
	
}

int ComportamientoJugador::interact(Action accion, int valor)
{
	return false;
}