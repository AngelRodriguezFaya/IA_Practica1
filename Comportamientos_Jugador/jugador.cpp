#include "../Comportamientos_Jugador/jugador.hpp"
#include <iostream>
#include <vector>

using namespace std;

void reiniciar();

Action ComportamientoJugador::think(Sensores sensores)
{	

	Action accion = actIDLE;
	int a, i, ir_a_casilla, filaObjetivo, columnaObjetivo;

	//------------------------------------------------------------------------------//
	//-------------------Actualización de las variables de estado-------------------//
	//------------------------------------------------------------------------------//

	switch (last_action){
	
	// AVANZAR
	case actWALK:		
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
	
	// CORRER
	case actRUN:		
		switch (current_state.brujula)
		{
		case norte: current_state.fil -= 2; break;
		case noreste: current_state.fil -= 2; current_state.col +=2; break;
		case este: current_state.col += 2; break;
		case sureste: current_state.fil += 2; current_state.col += 2; break;
		case sur: current_state.fil += 2; break;
		case suroeste: current_state.fil += 2; current_state.col -= 2; break;
		case oeste: current_state.col -= 2; break;
		case noroeste: current_state.fil -=2; current_state.col -= 2; break;
		}
		break;
	
	// GIRAR 45º A LA DERECHA
	case actTURN_SR:	
		a = current_state.brujula;
		a = (a + 1) % 8;
		current_state.brujula = static_cast<Orientacion>(a);
		break;
	
	// GIRAR 90º A LA IZQUIERDA
	case actTURN_L:	   
		a = current_state.brujula;
		a = (a + 8 - 2) % 8;
		current_state.brujula = static_cast<Orientacion>(a);
		break;
	}

	// CASILLA POSICIONAMIENTO
	if(sensores.terreno[0] == 'G' and !bien_situado){
		current_state.fil = sensores.posF;
		current_state.col = sensores.posC;
		current_state.brujula = sensores.sentido;
		bien_situado = true;
	}

	// CASILLA BIKINI
	if(sensores.terreno[0] == 'K' and !bikini_puesto){
		bikini_puesto = true;
	}

	// CASILLA ZAPATILLAS
	if(sensores.terreno[0] == 'D' and !zapatillas_puestas){
		zapatillas_puestas = true;
	}

	// CASILLA DE PRECIPIO 
	if(sensores.terreno[0] == 'P'){
		// reiniciar();
		cout << "Reiniciando...\n";
		current_state.fil = 99;
    	current_state.col = 99;
    	current_state.brujula = norte;
    	girar_derecha = false;
    	bien_situado = false;
    	bikini_puesto = false;
    	zapatillas_puestas = false;
	}

	// CASILLA DE RECARGA
	if(sensores.terreno[0] == 'X'){
		RecargarPilas(sensores);
	}

	if( bien_situado && (sensores.nivel == 1 or sensores.nivel == 2 or sensores.nivel == 3) ){ 
		PonerTerrenoEnMatriz(sensores.terreno, current_state, mapaResultado, sensores);
	}else if (sensores.nivel == 0){
		// Revisar esto...................................................
		current_state.fil = sensores.posF;
		current_state.col = sensores.posC;
		current_state.brujula = sensores.sentido;
		bien_situado = true;
		PonerTerrenoEnMatriz(sensores.terreno, current_state, mapaResultado, sensores);
	}

	// SI NECESITA RECARGAR BATERIA
	if(sensores.bateria < BATERIA_MIN){
		necesita_recargar = true;
	}else{
		necesita_recargar = false;
	}

	// SI LE QUEDA POCA VIDA
	if(sensores.vida < 100){
		queda_poca_vida = true;
	}else{
		queda_poca_vida = false;
	}

	//------------------------------------------------------------------------------//
	//--------------------------Decidir la nueva accion-----------------------------//
	//------------------------------------------------------------------------------//

	// if(SiguienteCasillaLibre(sensores)){	
	// 		accion = actWALK;
	// } else if (sensores.terreno[0] == 'X' and recargando_pilas){	// Si está en la casilla de recarga, no se mueve.
	// 	accion = actIDLE;
	// } else if(!girar_derecha){	
	// 	accion = actTURN_L;
	// 	girar_derecha = (rand()%2 == 0);
	// } else {
	// 	accion = actTURN_SR;
	// 	girar_derecha = (rand()%2 == 0);
	// }

	if(!recargando_pilas){
		// Recorremos el sensor de terreno
		i = 0;
    	while(i < sensores.terreno.size()) {
        	// Si el agente necesita posicionarse y encuentra la casilla 'G'
        	if (!bien_situado && sensores.terreno[i] == 'G') {
            	// Definimos la acción para ir a la casilla 'G'
            	accion = SiguienteAccion(i);
				ir_a_casilla = i;
				break;
        	}
        	// Si el agente necesita recargar y encuentra la casilla 'X'
        	else if (necesita_recargar && sensores.terreno[i] == 'X') {
            	// Definimos la acción para ir a la casilla 'X'
            	accion = SiguienteAccion(i);
				RecargarPilas(sensores);
				ir_a_casilla = i;
				break;
        	}
        	// Si el agente no tiene el bikini y encuentra la casilla 'K'
        	else if (!bikini_puesto && sensores.terreno[i] == 'K') {
            	// Definimos la acción para ir a la casilla 'K'
            	accion = SiguienteAccion(i);
				ir_a_casilla = i;
				break;
        	}
        	// Si el agente no tiene las zapatillas y encuentra la casilla 'D'
        	else if (!zapatillas_puestas && sensores.terreno[i] == 'D') {
            	// Definimos la acción para ir a la casilla 'D'
            	accion = SiguienteAccion(i);
				ir_a_casilla = i;
				break;
        	}

			// Si el agente ve una casilla sin pintar, va a ella.
        	else if (bien_situado && CasillaSinPintar(i, sensores)) {
            	// Definimos la acción para ir a la casilla sin pintar
            	accion = SiguienteAccion(i);
            	ir_a_casilla = i;
            	break;
        	}
			i++;
    	}

    	// Si no se ha definido ninguna acción específica, el agente avanza si puede
    	if (accion == actIDLE){
			if(CasillaLibre(sensores, 2)) {
        		accion = actWALK;  
			}else{
				if(!girar_derecha){	
					accion = actTURN_L;
					girar_derecha = (rand()%2 == 0);
				}else{
					accion = actTURN_SR;
					girar_derecha = (rand()%2 == 0);
				}
    		}
		}
	}else
		accion = actIDLE;

	//------------------------------------------------------------------------------//
	//----------------------Mostrar el valor de los sensores------------------------//
	//------------------------------------------------------------------------------//

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
	cout << "  Vida: " << sensores.vida << endl<< endl; 		

	//------------------------------------------------------------------------------//
	//----------------Mostrar el valor de las variables de estado-------------------//
	//------------------------------------------------------------------------------//

	cout << "\n Bien situado: " << bien_situado;
	cout << " current_state.fil: " << current_state.fil << endl;
	cout << " current_state.col: " << current_state.col << endl;
	cout << " current_state.brujula: " << current_state.brujula << endl;
	cout << " Bikini puesto: " << bikini_puesto << endl;
	cout << " Zapatillas puestas: " << zapatillas_puestas << endl;
	cout << " Recargando pilas: " << recargando_pilas << endl;
	cout << " Necesita recargar: " << necesita_recargar << endl;
	cout << " Queda poca vida: " << queda_poca_vida << endl;
	cout << " Accion pendiente: " << accion_pendiente << endl;
	cout << " ir_a_casilla: " << ir_a_casilla << endl;
	cout << "--------------------------------------------\n\n";
	// Recordar la ultima accion
	last_action = accion;

	return accion;

}

	//------------------------------------------------------------------------------//
	//------------------Implementación de métodos auxiliares------------------------//
	//------------------------------------------------------------------------------//

int ComportamientoJugador::interact(Action accion, int valor){
	return false;
}

void ComportamientoJugador::PonerTerrenoEnMatriz(const vector<unsigned char> &terreno, 
			const state &st, vector< vector< unsigned char> > &matriz, Sensores& sensores){
	
	// Según la orinetación del agente, se coloca el terreno en la matriz.
	switch (st.brujula)
	{
	case norte:
		cout << ">>>>>>>Pinta terreno al norte\n";
		matriz[st.fil][st.col] = terreno[0];
		matriz[st.fil - 1][st.col - 1] = terreno[1];
		matriz[st.fil - 1][st.col] = terreno[2];
		matriz[st.fil - 1][st.col + 1] = terreno[3];
		matriz[st.fil - 2][st.col - 2] = terreno[4];
		matriz[st.fil - 2][st.col - 1] = terreno[5];
		matriz[st.fil - 2][st.col + 1] = terreno[7];
		matriz[st.fil - 2][st.col + 2] = terreno[8];
		matriz[st.fil - 3][st.col - 3] = terreno[9];
		matriz[st.fil - 3][st.col - 2] = terreno[10];
		matriz[st.fil - 3][st.col + 2] = terreno[14];
		matriz[st.fil - 3][st.col + 3] = terreno[15];
		if(sensores.nivel != 3){
			matriz[st.fil - 2][st.col] = terreno[6];
			matriz[st.fil - 3][st.col - 1] = terreno[11];
			matriz[st.fil - 3][st.col] = terreno[12];
			matriz[st.fil - 3][st.col + 1] = terreno[13];
		}
		break;
	case noreste:
		cout << "Pinta terreno al noreste\n";
		matriz[st.fil][st.col] = terreno[0];
		matriz[st.fil - 1][st.col] = terreno[1];
		matriz[st.fil - 1][st.col + 1] = terreno[2];
		matriz[st.fil][st.col + 1] = terreno[3];
		matriz[st.fil - 2][st.col] = terreno[4];
		matriz[st.fil - 2][st.col + 1] = terreno[5];
		matriz[st.fil - 1][st.col + 2] = terreno[7];
		matriz[st.fil][st.col + 2] = terreno[8];
		matriz[st.fil - 3][st.col] = terreno[9];
		matriz[st.fil - 3][st.col + 1] = terreno[10];
		matriz[st.fil - 1][st.col + 3] = terreno[14];
		matriz[st.fil][st.col + 3] = terreno[15];
		if(sensores.nivel != 3){
			matriz[st.fil - 2][st.col + 2] = terreno[6];
			matriz[st.fil - 3][st.col + 2] = terreno[11];
			matriz[st.fil - 3][st.col + 3] = terreno[12];
			matriz[st.fil - 2][st.col + 3] = terreno[13];
		}
		break;
	case este:
		cout << "Pinta terreno al este\n";
		matriz[st.fil][st.col] = terreno[0];
		matriz[st.fil - 1][st.col + 1] = terreno[1];
		matriz[st.fil][st.col + 1] = terreno[2];
		matriz[st.fil + 1][st.col + 1] = terreno[3];
		matriz[st.fil - 2][st.col + 2] = terreno[4];
		matriz[st.fil - 1][st.col + 2] = terreno[5];
		matriz[st.fil + 1][st.col + 2] = terreno[7];
		matriz[st.fil + 2][st.col + 2] = terreno[8];
		matriz[st.fil - 3][st.col + 3] = terreno[9];
		matriz[st.fil - 2][st.col + 3] = terreno[10];
		matriz[st.fil + 2][st.col + 3] = terreno[14];
		matriz[st.fil + 3][st.col + 3] = terreno[15];
		if(sensores.nivel != 3){
			matriz[st.fil][st.col + 2] = terreno[6];
			matriz[st.fil - 1][st.col + 3] = terreno[11];
			matriz[st.fil][st.col + 3] = terreno[12];
			matriz[st.fil + 1][st.col + 3] = terreno[13];
		}
		break;
	case sureste:
		cout << "Pinta terreno al sureste\n";
		matriz[st.fil][st.col] = terreno[0];
		matriz[st.fil][st.col + 1] = terreno[1];
		matriz[st.fil + 1][st.col + 1] = terreno[2];
		matriz[st.fil + 1][st.col] = terreno[3];
		matriz[st.fil][st.col + 2] = terreno[4];
		matriz[st.fil + 1][st.col + 2] = terreno[5];
		matriz[st.fil + 2][st.col + 1] = terreno[7];
		matriz[st.fil + 2][st.col] = terreno[8];
		matriz[st.fil][st.col + 3] = terreno[9];
		matriz[st.fil + 1][st.col + 3] = terreno[10];
		matriz[st.fil + 3][st.col + 1] = terreno[14];
		matriz[st.fil + 3][st.col] = terreno[15];
		if(sensores.nivel != 3){
			matriz[st.fil + 2][st.col + 2] = terreno[6];
			matriz[st.fil + 2][st.col + 3] = terreno[11];
			matriz[st.fil + 3][st.col + 3] = terreno[12];
			matriz[st.fil + 3][st.col + 2] = terreno[13];
		}
		break;
	case sur:
		cout << "Pinta terreno al sur\n";
		matriz[st.fil][st.col] = terreno[0];
		matriz[st.fil + 1][st.col + 1] = terreno[1];
		matriz[st.fil + 1][st.col] = terreno[2];
		matriz[st.fil + 1][st.col - 1] = terreno[3];
		matriz[st.fil + 2][st.col + 2] = terreno[4];
		matriz[st.fil + 2][st.col + 1] = terreno[5];
		matriz[st.fil + 2][st.col - 1] = terreno[7];
		matriz[st.fil + 2][st.col - 2] = terreno[8];
		matriz[st.fil + 3][st.col + 3] = terreno[9];
		matriz[st.fil + 3][st.col + 2] = terreno[10];
		matriz[st.fil + 3][st.col - 2] = terreno[14];
		matriz[st.fil + 3][st.col - 3] = terreno[15];
		if(sensores.nivel != 3){
			matriz[st.fil + 2][st.col] = terreno[6];
			matriz[st.fil + 3][st.col + 1] = terreno[11];
			matriz[st.fil + 3][st.col] = terreno[12];
			matriz[st.fil + 3][st.col - 1] = terreno[13];
		}
		break;

	case suroeste:
		cout << "Pinta terreno al suroeste\n";
		matriz[st.fil][st.col] = terreno[0];
		matriz[st.fil + 1][st.col] = terreno[1];
		matriz[st.fil + 1][st.col - 1] = terreno[2];
		matriz[st.fil][st.col - 1] = terreno[3];
		matriz[st.fil + 2][st.col] = terreno[4];
		matriz[st.fil + 2][st.col - 1] = terreno[5];
		matriz[st.fil + 1][st.col - 2] = terreno[7];
		matriz[st.fil][st.col - 2] = terreno[8];
		matriz[st.fil + 3][st.col] = terreno[9];
		matriz[st.fil + 3][st.col - 1] = terreno[10];	
		matriz[st.fil + 1][st.col - 3] = terreno[14];
		matriz[st.fil][st.col - 3] = terreno[15];
		if(sensores.nivel != 3){
			matriz[st.fil + 2][st.col - 2] = terreno[6];
			matriz[st.fil + 3][st.col - 2] = terreno[11];
			matriz[st.fil + 3][st.col - 3] = terreno[12];
			matriz[st.fil + 2][st.col - 3] = terreno[13];
		}
		break;
	
	case oeste:
		cout << "Pinta terreno al oeste\n";
		matriz[st.fil][st.col] = terreno[0];
		matriz[st.fil + 1][st.col - 1] = terreno[1];
		matriz[st.fil][st.col - 1] = terreno[2];
		matriz[st.fil - 1][st.col - 1] = terreno[3];
		matriz[st.fil + 2][st.col - 2] = terreno[4];
		matriz[st.fil + 1][st.col - 2] = terreno[5];
		matriz[st.fil - 1][st.col - 2] = terreno[7];
		matriz[st.fil - 2][st.col - 2] = terreno[8];
		matriz[st.fil + 3][st.col - 3] = terreno[9];
		matriz[st.fil + 2][st.col - 3] = terreno[10];
		matriz[st.fil - 2][st.col - 3] = terreno[14];
		matriz[st.fil - 3][st.col - 3] = terreno[15];
		if(sensores.nivel != 3){
			matriz[st.fil][st.col - 2] = terreno[6];
			matriz[st.fil + 1][st.col - 3] = terreno[11];
			matriz[st.fil][st.col - 3] = terreno[12];
			matriz[st.fil - 1][st.col - 3] = terreno[13];
		}
		break;
	
	case noroeste:
		cout << "Pinta terreno al noroeste\n";
		matriz[st.fil][st.col] = terreno[0];
		matriz[st.fil][st.col - 1] = terreno[1];
		matriz[st.fil - 1][st.col - 1] = terreno[2];
		matriz[st.fil - 1][st.col] = terreno[3];
		matriz[st.fil][st.col - 2] = terreno[4];
		matriz[st.fil - 1][st.col - 2] = terreno[5];
		matriz[st.fil - 2][st.col - 1] = terreno[7];
		matriz[st.fil - 2][st.col] = terreno[8];
		matriz[st.fil][st.col - 3] = terreno[9];
		matriz[st.fil - 1][st.col - 3] = terreno[10];	
		matriz[st.fil - 3][st.col - 1] = terreno[14];
		matriz[st.fil - 3][st.col] = terreno[15];
		if(sensores.nivel != 3){
			matriz[st.fil - 2][st.col - 2] = terreno[6];
			matriz[st.fil - 2][st.col - 3] = terreno[11];
			matriz[st.fil - 3][st.col - 3] = terreno[12];
			matriz[st.fil - 3][st.col - 2] = terreno[13];
		}
		break;
	}

}

void ComportamientoJugador::PonerTerrenoEnAuxiliar(const vector<unsigned char> &terreno, 
			const state &st, vector< vector< unsigned char> > &matriz, Sensores& sensores){
	
	// ESTE ES PARA LA MATRIZ AUXILIAR
	// Según la orinetación del agente, se coloca el terreno en la matriz.
	switch (st.brujula)
	{
	case norte:
		cout << ">>>>>>>Pinta terreno al norte en AUXILIAR\n";
		matriz[st.fil][st.col] = terreno[0];
		matriz[st.fil - 1][st.col - 1] = terreno[1];
		matriz[st.fil - 1][st.col] = terreno[2];
		matriz[st.fil - 1][st.col + 1] = terreno[3];
		matriz[st.fil - 2][st.col - 2] = terreno[4];
		matriz[st.fil - 2][st.col - 1] = terreno[5];
		matriz[st.fil - 2][st.col + 1] = terreno[7];
		matriz[st.fil - 2][st.col + 2] = terreno[8];
		matriz[st.fil - 3][st.col - 3] = terreno[9];
		matriz[st.fil - 3][st.col - 2] = terreno[10];
		matriz[st.fil - 3][st.col + 2] = terreno[14];
		matriz[st.fil - 3][st.col + 3] = terreno[15];
		if(sensores.nivel != 3){
			matriz[st.fil - 2][st.col] = terreno[6];
			matriz[st.fil - 3][st.col - 1] = terreno[11];
			matriz[st.fil - 3][st.col] = terreno[12];
			matriz[st.fil - 3][st.col + 1] = terreno[13];
		}
		break;
	case noreste:
		cout << "Pinta terreno al noreste en AUXILIAR\n";
		matriz[st.fil][st.col] = terreno[0];
		matriz[st.fil - 1][st.col] = terreno[1];
		matriz[st.fil - 1][st.col + 1] = terreno[2];
		matriz[st.fil][st.col + 1] = terreno[3];
		matriz[st.fil - 2][st.col] = terreno[4];
		matriz[st.fil - 2][st.col + 1] = terreno[5];
		matriz[st.fil - 1][st.col + 2] = terreno[7];
		matriz[st.fil][st.col + 2] = terreno[8];
		matriz[st.fil - 3][st.col] = terreno[9];
		matriz[st.fil - 3][st.col + 1] = terreno[10];
		matriz[st.fil - 1][st.col + 3] = terreno[14];
		matriz[st.fil][st.col + 3] = terreno[15];
		if(sensores.nivel != 3){
			matriz[st.fil - 2][st.col + 2] = terreno[6];
			matriz[st.fil - 3][st.col + 2] = terreno[11];
			matriz[st.fil - 3][st.col + 3] = terreno[12];
			matriz[st.fil - 2][st.col + 3] = terreno[13];
		}
		break;
	case este:
		cout << "Pinta terreno al este en AUXILIAR\n";
		matriz[st.fil][st.col] = terreno[0];
		matriz[st.fil - 1][st.col + 1] = terreno[1];
		matriz[st.fil][st.col + 1] = terreno[2];
		matriz[st.fil + 1][st.col + 1] = terreno[3];
		matriz[st.fil - 2][st.col + 2] = terreno[4];
		matriz[st.fil - 1][st.col + 2] = terreno[5];
		matriz[st.fil + 1][st.col + 2] = terreno[7];
		matriz[st.fil + 2][st.col + 2] = terreno[8];
		matriz[st.fil - 3][st.col + 3] = terreno[9];
		matriz[st.fil - 2][st.col + 3] = terreno[10];
		matriz[st.fil + 2][st.col + 3] = terreno[14];
		matriz[st.fil + 3][st.col + 3] = terreno[15];
		if(sensores.nivel != 3){
			matriz[st.fil][st.col + 2] = terreno[6];
			matriz[st.fil - 1][st.col + 3] = terreno[11];
			matriz[st.fil][st.col + 3] = terreno[12];
			matriz[st.fil + 1][st.col + 3] = terreno[13];
		}
		break;
	case sureste:
		cout << "Pinta terreno al sureste en AUXILIAR\n";
		matriz[st.fil][st.col] = terreno[0];
		matriz[st.fil][st.col + 1] = terreno[1];
		matriz[st.fil + 1][st.col + 1] = terreno[2];
		matriz[st.fil + 1][st.col] = terreno[3];
		matriz[st.fil][st.col + 2] = terreno[4];
		matriz[st.fil + 1][st.col + 2] = terreno[5];
		matriz[st.fil + 2][st.col + 1] = terreno[7];
		matriz[st.fil + 2][st.col] = terreno[8];
		matriz[st.fil][st.col + 3] = terreno[9];
		matriz[st.fil + 1][st.col + 3] = terreno[10];
		matriz[st.fil + 3][st.col + 1] = terreno[14];
		matriz[st.fil + 3][st.col] = terreno[15];
		if(sensores.nivel != 3){
			matriz[st.fil + 2][st.col + 2] = terreno[6];
			matriz[st.fil + 2][st.col + 3] = terreno[11];
			matriz[st.fil + 3][st.col + 3] = terreno[12];
			matriz[st.fil + 3][st.col + 2] = terreno[13];
		}
		break;
	case sur:
		cout << "Pinta terreno al sur en AUXILIAR\n";
		matriz[st.fil][st.col] = terreno[0];
		matriz[st.fil + 1][st.col + 1] = terreno[1];
		matriz[st.fil + 1][st.col] = terreno[2];
		matriz[st.fil + 1][st.col - 1] = terreno[3];
		matriz[st.fil + 2][st.col + 2] = terreno[4];
		matriz[st.fil + 2][st.col + 1] = terreno[5];
		matriz[st.fil + 2][st.col - 1] = terreno[7];
		matriz[st.fil + 2][st.col - 2] = terreno[8];
		matriz[st.fil + 3][st.col + 3] = terreno[9];
		matriz[st.fil + 3][st.col + 2] = terreno[10];
		matriz[st.fil + 3][st.col - 2] = terreno[14];
		matriz[st.fil + 3][st.col - 3] = terreno[15];
		if(sensores.nivel != 3){
			matriz[st.fil + 2][st.col] = terreno[6];
			matriz[st.fil + 3][st.col + 1] = terreno[11];
			matriz[st.fil + 3][st.col] = terreno[12];
			matriz[st.fil + 3][st.col - 1] = terreno[13];
		}
		break;

	case suroeste:
		cout << "Pinta terreno al suroeste en AUXILIAR\n";
		matriz[st.fil][st.col] = terreno[0];
		matriz[st.fil + 1][st.col] = terreno[1];
		matriz[st.fil + 1][st.col - 1] = terreno[2];
		matriz[st.fil][st.col - 1] = terreno[3];
		matriz[st.fil + 2][st.col] = terreno[4];
		matriz[st.fil + 2][st.col - 1] = terreno[5];
		matriz[st.fil + 1][st.col - 2] = terreno[7];
		matriz[st.fil][st.col - 2] = terreno[8];
		matriz[st.fil + 3][st.col] = terreno[9];
		matriz[st.fil + 3][st.col - 1] = terreno[10];	
		matriz[st.fil + 1][st.col - 3] = terreno[14];
		matriz[st.fil][st.col - 3] = terreno[15];
		if(sensores.nivel != 3){
			matriz[st.fil + 2][st.col - 2] = terreno[6];
			matriz[st.fil + 3][st.col - 2] = terreno[11];
			matriz[st.fil + 3][st.col - 3] = terreno[12];
			matriz[st.fil + 2][st.col - 3] = terreno[13];
		}
		break;
	
	case oeste:
		cout << "Pinta terreno al oeste en AUXILIAR\n";
		matriz[st.fil][st.col] = terreno[0];
		matriz[st.fil + 1][st.col - 1] = terreno[1];
		matriz[st.fil][st.col - 1] = terreno[2];
		matriz[st.fil - 1][st.col - 1] = terreno[3];
		matriz[st.fil + 2][st.col - 2] = terreno[4];
		matriz[st.fil + 1][st.col - 2] = terreno[5];
		matriz[st.fil - 1][st.col - 2] = terreno[7];
		matriz[st.fil - 2][st.col - 2] = terreno[8];
		matriz[st.fil + 3][st.col - 3] = terreno[9];
		matriz[st.fil + 2][st.col - 3] = terreno[10];
		matriz[st.fil - 2][st.col - 3] = terreno[14];
		matriz[st.fil - 3][st.col - 3] = terreno[15];
		if(sensores.nivel != 3){
			matriz[st.fil][st.col - 2] = terreno[6];
			matriz[st.fil + 1][st.col - 3] = terreno[11];
			matriz[st.fil][st.col - 3] = terreno[12];
			matriz[st.fil - 1][st.col - 3] = terreno[13];
		}
		break;
	
	case noroeste:
		cout << "Pinta terreno al noroeste en AUXILIAR\n";
		matriz[st.fil][st.col] = terreno[0];
		matriz[st.fil][st.col - 1] = terreno[1];
		matriz[st.fil - 1][st.col - 1] = terreno[2];
		matriz[st.fil - 1][st.col] = terreno[3];
		matriz[st.fil][st.col - 2] = terreno[4];
		matriz[st.fil - 1][st.col - 2] = terreno[5];
		matriz[st.fil - 2][st.col - 1] = terreno[7];
		matriz[st.fil - 2][st.col] = terreno[8];
		matriz[st.fil][st.col - 3] = terreno[9];
		matriz[st.fil - 1][st.col - 3] = terreno[10];	
		matriz[st.fil - 3][st.col - 1] = terreno[14];
		matriz[st.fil - 3][st.col] = terreno[15];
		if(sensores.nivel != 3){
			matriz[st.fil - 2][st.col - 2] = terreno[6];
			matriz[st.fil - 2][st.col - 3] = terreno[11];
			matriz[st.fil - 3][st.col - 3] = terreno[12];
			matriz[st.fil - 3][st.col - 2] = terreno[13];
		}
		break;
	}

}

/* void ComportamientoJugador::PonerInstanteEnMatriz(const int& instante_actual, const state &st, 
              vector< vector<int> > &matriz){
	
	// Según la orientación del agente, se coloca el instante en la matriz.
	switch (st.brujula)
	{
		case norte:
		cout << ">>>>>>>Pone instantes al norte\n";
		matriz[st.fil][st.col] = instante_actual;
		matriz[st.fil - 1][st.col - 1] = instante_actual;
		matriz[st.fil - 1][st.col] = instante_actual;
		matriz[st.fil - 1][st.col + 1] = instante_actual;
		matriz[st.fil - 2][st.col - 2] = instante_actual;
		matriz[st.fil - 2][st.col - 1] = instante_actual;
		matriz[st.fil - 2][st.col] = instante_actual;
		matriz[st.fil - 2][st.col + 1] = instante_actual;
		matriz[st.fil - 2][st.col + 2] = instante_actual;
		matriz[st.fil - 3][st.col - 3] = instante_actual;
		matriz[st.fil - 3][st.col - 2] = instante_actual;
		matriz[st.fil - 3][st.col - 1] = instante_actual;
		matriz[st.fil - 3][st.col] = instante_actual;
		matriz[st.fil - 3][st.col + 1] = instante_actual;
		matriz[st.fil - 3][st.col + 2] = instante_actual;
		matriz[st.fil - 3][st.col + 3] = instante_actual;
		break;

	case noreste:
		cout << ">>>>>>>Pone instantes al noroeste\n";
		matriz[st.fil][st.col] = instante_actual;
		matriz[st.fil - 1][st.col] = instante_actual;
		matriz[st.fil - 1][st.col + 1] = instante_actual;
		matriz[st.fil][st.col + 1] = instante_actual;
		matriz[st.fil - 2][st.col] = instante_actual;
		matriz[st.fil - 2][st.col + 1] = instante_actual;
		matriz[st.fil - 2][st.col + 2] = instante_actual;
		matriz[st.fil - 1][st.col + 2] = instante_actual;
		matriz[st.fil][st.col + 2] = instante_actual;
		matriz[st.fil - 3][st.col] = instante_actual;
		matriz[st.fil - 3][st.col + 1] = instante_actual;
		matriz[st.fil - 3][st.col + 2] = instante_actual;
		matriz[st.fil - 3][st.col + 3] = instante_actual;
		matriz[st.fil - 2][st.col + 3] = instante_actual;
		matriz[st.fil - 1][st.col + 3] = instante_actual;
		matriz[st.fil][st.col + 3] = instante_actual;
		break;

	case este:
		cout << ">>>>>>>Pone instantes al este\n";
		matriz[st.fil][st.col] = instante_actual;
		matriz[st.fil - 1][st.col + 1] = instante_actual;
		matriz[st.fil][st.col + 1] = instante_actual;
		matriz[st.fil + 1][st.col + 1] = instante_actual;
		matriz[st.fil - 2][st.col + 2] = instante_actual;
		matriz[st.fil - 1][st.col + 2] = instante_actual;
		matriz[st.fil][st.col + 2] = instante_actual;
		matriz[st.fil + 1][st.col + 2] = instante_actual;
		matriz[st.fil + 2][st.col + 2] = instante_actual;
		matriz[st.fil - 3][st.col + 3] = instante_actual;
		matriz[st.fil - 2][st.col + 3] = instante_actual;
		matriz[st.fil - 1][st.col + 3] = instante_actual;
		matriz[st.fil][st.col + 3] = instante_actual;
		matriz[st.fil + 1][st.col + 3] = instante_actual;
		matriz[st.fil + 2][st.col + 3] = instante_actual;
		matriz[st.fil + 3][st.col + 3] = instante_actual;
		break;

	case sureste:
		cout << ">>>>>>>Pone instantes al sureste\n";
		matriz[st.fil][st.col] = instante_actual;
		matriz[st.fil][st.col + 1] = instante_actual;
		matriz[st.fil + 1][st.col + 1] = instante_actual;
		matriz[st.fil + 1][st.col] = instante_actual;
		matriz[st.fil][st.col + 2] = instante_actual;
		matriz[st.fil + 1][st.col + 2] = instante_actual;
		matriz[st.fil + 2][st.col + 2] = instante_actual;
		matriz[st.fil + 2][st.col + 1] = instante_actual;
		matriz[st.fil + 2][st.col] = instante_actual;
		matriz[st.fil][st.col + 3] = instante_actual;
		matriz[st.fil + 1][st.col + 3] = instante_actual;
		matriz[st.fil + 2][st.col + 3] = instante_actual;
		matriz[st.fil + 3][st.col + 3] = instante_actual;
		matriz[st.fil + 3][st.col + 2] = instante_actual;
		matriz[st.fil + 3][st.col + 1] = instante_actual;
		matriz[st.fil + 3][st.col] = instante_actual;
		break;

	case sur:
		cout << ">>>>>>>Pone instantes al sur\n";
		matriz[st.fil][st.col] = instante_actual;
		matriz[st.fil + 1][st.col + 1] = instante_actual;
		matriz[st.fil + 1][st.col] = instante_actual;
		matriz[st.fil + 1][st.col - 1] = instante_actual;
		matriz[st.fil + 2][st.col + 2] = instante_actual;
		matriz[st.fil + 2][st.col + 1] = instante_actual;
		matriz[st.fil + 2][st.col] = instante_actual;
		matriz[st.fil + 2][st.col - 1] = instante_actual;
		matriz[st.fil + 2][st.col - 2] = instante_actual;
		matriz[st.fil + 3][st.col + 3] = instante_actual;
		matriz[st.fil + 3][st.col + 2] = instante_actual;
		matriz[st.fil + 3][st.col + 1] = instante_actual;
		matriz[st.fil + 3][st.col] = instante_actual;
		matriz[st.fil + 3][st.col - 1] = instante_actual;
		matriz[st.fil + 3][st.col - 2] = instante_actual;
		matriz[st.fil + 3][st.col - 3] = instante_actual;
		break;

	case suroeste:
		cout << ">>>>>>>Pone instantes al suroeste\n";
		matriz[st.fil][st.col] = instante_actual;
		matriz[st.fil + 1][st.col] = instante_actual;
		matriz[st.fil + 1][st.col - 1] = instante_actual;
		matriz[st.fil][st.col - 1] = instante_actual;
		matriz[st.fil + 2][st.col] = instante_actual;
		matriz[st.fil + 2][st.col - 1] = instante_actual;
		matriz[st.fil + 2][st.col - 2] = instante_actual;
		matriz[st.fil + 1][st.col - 2] = instante_actual;
		matriz[st.fil][st.col - 2] = instante_actual;
		matriz[st.fil + 3][st.col] = instante_actual;
		matriz[st.fil + 3][st.col - 1] = instante_actual;
		matriz[st.fil + 3][st.col - 2] = instante_actual;
		matriz[st.fil + 3][st.col - 3] = instante_actual;
		matriz[st.fil + 2][st.col - 3] = instante_actual;
		matriz[st.fil + 1][st.col - 3] = instante_actual;
		matriz[st.fil][st.col - 3] = instante_actual;
		break;
	
	case oeste:
		cout << ">>>>>>>Pone instantes al oeste\n";
		matriz[st.fil][st.col] = instante_actual;
		matriz[st.fil + 1][st.col - 1] = instante_actual;
		matriz[st.fil][st.col - 1] = instante_actual;
		matriz[st.fil - 1][st.col - 1] = instante_actual;
		matriz[st.fil + 2][st.col - 2] = instante_actual;
		matriz[st.fil + 1][st.col - 2] = instante_actual;
		matriz[st.fil][st.col - 2] = instante_actual;
		matriz[st.fil - 1][st.col - 2] = instante_actual;
		matriz[st.fil - 2][st.col - 2] = instante_actual;
		matriz[st.fil + 3][st.col - 3] = instante_actual;
		matriz[st.fil + 2][st.col - 3] = instante_actual;
		matriz[st.fil + 1][st.col - 3] = instante_actual;
		matriz[st.fil][st.col - 3] = instante_actual;
		matriz[st.fil - 1][st.col - 3] = instante_actual;
		matriz[st.fil - 2][st.col - 3] = instante_actual;
		matriz[st.fil - 3][st.col - 3] = instante_actual;
		break;
	
	case noroeste:
		cout << ">>>>>>>Pone instantes al noroeste\n";
		matriz[st.fil][st.col] = instante_actual;
		matriz[st.fil][st.col - 1] = instante_actual;
		matriz[st.fil - 1][st.col - 1] = instante_actual;
		matriz[st.fil - 1][st.col] = instante_actual;
		matriz[st.fil][st.col - 2] = instante_actual;
		matriz[st.fil - 1][st.col - 2] = instante_actual;
		matriz[st.fil - 2][st.col - 2] = instante_actual;
		matriz[st.fil - 2][st.col - 1] = instante_actual;
		matriz[st.fil - 2][st.col] = instante_actual;
		matriz[st.fil][st.col - 3] = instante_actual;
		matriz[st.fil - 1][st.col - 3] = instante_actual;
		matriz[st.fil - 2][st.col - 3] = instante_actual;
		matriz[st.fil - 3][st.col - 3] = instante_actual;
		matriz[st.fil - 3][st.col - 2] = instante_actual;
		matriz[st.fil - 3][st.col - 1] = instante_actual;
		matriz[st.fil - 3][st.col] = instante_actual;
		break;
	}
}
	 */

void ComportamientoJugador::RecargarPilas(Sensores& sensores){
	if(sensores.terreno[0] == 'X' and necesita_recargar and !queda_poca_vida){
		recargando_pilas = true;
		cout << "Recargando mi bateria... Dame un descanso\n";
		sensores.bateria += 10;
	}else
		recargando_pilas = false;
}

// La casilla debe de estar en el cono de visión del agente
bool ComportamientoJugador::CasillaLibre(const Sensores &sensores, const int& num_casilla){
	bool casilla_libre = false;
	if( (sensores.terreno[num_casilla] == 'T' 						// Suelo Arenoso
		or sensores.terreno[num_casilla] == 'S' 					// Suelo Pedregoso
		or sensores.terreno[num_casilla] == 'G' 					// Posicinamiento
		or (sensores.terreno[num_casilla] == 'B' and zapatillas_puestas)	// Bosque
		or (sensores.terreno[num_casilla] == 'A' and bikini_puesto) // Agua
		or sensores.terreno[num_casilla] == 'K'						// Bikini
		or sensores.terreno[num_casilla] == 'D'						// Zapatillas
		or sensores.terreno[num_casilla] == 'X'						// Recarga
		) and sensores.agentes[num_casilla] == '_'					// No hay agente
		  and sensores.terreno[num_casilla] != 'P'					// No es un precipicio
		  and sensores.terreno[num_casilla] != 'M'					// No es un muro
		//   and !sensores.colision
		  and !recargando_pilas){	// Si no esta recargando
			casilla_libre = true;
	}

	return casilla_libre;
}

int ComportamientoJugador::CasillaAntigua(const Sensores& sensores, const vector< vector<int> >& matriz_instantes){
	int casilla_antigua = -1;

	for(int i = 0; i < TAM_SENSORES_TERR_AGEN; i++){
		if(sensores.terreno[i] < casilla_antigua){
			casilla_antigua = i;
		}
	}

	return casilla_antigua;
}

Action ComportamientoJugador::GiraRandomDerecha(const Sensores& sensores){
	Action accion;
	if(!girar_derecha){	
		accion = actTURN_L;
		girar_derecha = (rand()%2 == 0);
	}else{
		accion = actTURN_SR;
		girar_derecha = (rand()%2 == 0);
	}

	return accion;
}

Action ComportamientoJugador::SiguienteAccion(const int& i){
	// Definimos la acción en función de la posición en el sensor de terreno
    if (i == 2 || i == 6 || i == 12) {
        return actWALK;  // Corrección aquí
    } else if (i == 1 || i == 4 || i == 5 || i == 9 || i == 10 || i == 11) {
        return actTURN_L;
    } else if (i == 3 || i == 7 || i == 8 || i == 13 || i == 14 || i == 15) {
        return actTURN_SR;
    } else {
        return actIDLE;
    }
}

int ComportamientoJugador::CasillaSinPintar(const int& i, const Sensores& sensores) {
    // Obtener la posición del agente
    int posF = sensores.posF;
    int posC = sensores.posC;

    // Obtener la dirección a la que se quiere ir
    int dirF = posF;
    int dirC = posC;
    switch (sensores.sentido) {
        case norte:
            dirF--;
            break;
        case este:
            dirC++;
            break;
        case sur:
            dirF++;
            break;
        case oeste:
            dirC--;
            break;
        case noroeste:
            dirF--;
            dirC--;
            break;
        case noreste:
            dirF--;
            dirC++;
            break;
        case suroeste:
            dirF++;
            dirC--;
            break;
        case sureste:
            dirF++;
            dirC++;
            break;
    }

    // Buscar la casilla sin pintar en la matrizAuxiliar
    for (int fila = 0; fila < TAM_SENSORES_TERR_AGEN; fila++) {
        for (int columna = 0; columna < TAM_SENSORES_TERR_AGEN; columna++) {
            if (sensores.terreno[fila * TAM_SENSORES_TERR_AGEN + columna] == '?') {
                int casillaF = posF + fila - 1;
                int casillaC = posC + columna - 1;
                if (casillaF == dirF && casillaC == dirC) {
                    return fila * TAM_SENSORES_TERR_AGEN + columna;
                }
            }
        }
    }

    return -1;  // No se encontró ninguna casilla sin pintar en el cono de visión del agente
	
}


/* void reiniciar(){
	cout << "Reiniciando...\n";
	current_state.fil = 99;
    current_state.col = 99;
    current_state.brujula = norte;
    ComportamientoJugador. = false;
    bien_situado = false;
    bikini_puesto = false;
    zapatillas_puestas = false;
} */