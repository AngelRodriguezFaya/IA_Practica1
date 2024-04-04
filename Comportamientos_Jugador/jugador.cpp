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
		a = (a + 8 - 2) % 8;
		current_state.brujula = static_cast<Orientacion>(a);
		break;
	}

	// Si está en la casilla de posicionamiento, se actualizan el estado (la posición y 
	// la orientación) y le indicamos que ya está bien situado.
	if(sensores.terreno[0] == 'G' and !bien_situado){
		current_state.fil = sensores.posF;
		current_state.col = sensores.posC;
		current_state.brujula = sensores.sentido;
		bien_situado = true;
	}

	// Si está en la casilla del bikini y no lo lleva puesto, se lo pone.
	if(sensores.terreno[0] == 'K' and !bikini_puesto){
		bikini_puesto = true;
	}

	// Si está en la casilla de las zapatillas y no las lleva puestas, se las pone.
	if(sensores.terreno[0] == 'D' and !zapatillas_puestas){
		zapatillas_puestas = true;
	}

	// Si está en la casilla de precipio, sufre un reinicio.
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

	// Si está en la casilla de recargar, se recarga su bateria.
	if(sensores.terreno[0] == 'X'){
		RecargarPilas(sensores);
	}

	if(bien_situado && sensores.nivel == 1){ 
		PonerTerrenoEnMatriz(sensores.terreno, current_state, mapaResultado);
	}else if (sensores.nivel == 0){
		// Revisar esto...................................................
		current_state.fil = sensores.posF;
		current_state.col = sensores.posC;
		current_state.brujula = sensores.sentido;
		bien_situado = true;
		PonerTerrenoEnMatriz(sensores.terreno, current_state, mapaResultado);
	}

	//Si necesita recargar la bateria/pilas
	if(sensores.bateria < BATERIA_MAX){
		necesita_recargar = true;
	}else{
		necesita_recargar = false;
	}

	//Si le quedan pocos instantes de vida
	if(sensores.vida < 500){
		queda_poca_vida = true;
	}else{
		queda_poca_vida = false;
	}

	//------------------------------------------------------------------------------//
	//--------------------------Decidir la nueva accion-----------------------------//
	//------------------------------------------------------------------------------//

	if(SiguienteCasillaLibre(sensores)){	
			accion = actWALK;
	} else if (sensores.terreno[0] == 'X' && recargando_pilas){	// Si está en la casilla de recarga, no se mueve.
		accion = actIDLE;
	} else if(!girar_derecha){	
		accion = actTURN_L;
		girar_derecha = (rand()%2 == 0);
	} else {
		accion = actTURN_SR;
		girar_derecha = (rand()%2 == 0);
	}

	// // Orden: Posicionamiento(si necesita) > Recargar(si necesita) > Bikini(si no lleva) > Zapatillas(si no lleva) > Desconocida
	// // Si no hay ninguna, puede entrar en función la matriz de instantes.
	// i = 0;
	// while (i < TAM_SENSORES_TERR_AGEN){
	// 	if(sensores.terreno[i] == 'G' and !bien_situado){
	// 		ir_a_casilla = i;
	// 		break;
	// 	}else if(sensores.terreno[i] == 'X' and necesita_recargar){
	// 		ir_a_casilla = i;
	// 		break;
	// 	}else if(sensores.terreno[i] == 'K' and !bikini_puesto){
	// 		ir_a_casilla = i;
	// 		break;
	// 	}else if(sensores.terreno[i] == 'D' and !zapatillas_puestas){
	// 		ir_a_casilla = i;
	// 		break;
	// 	}else if(sensores.terreno[i] == '?'){
	// 		ir_a_casilla = i;
	// 		break;
	// 	}
	// 	i++;
	// }
	
	// // Calcular la fila y columna de la celda objetivo
	// filaObjetivo = current_state.fil;
	// columnaObjetivo = current_state.col;

	// switch (ir_a_casilla) {
	// 	case 0: // Norte
	// 		filaObjetivo--;
	// 		break;
	// 	case 1: // Noreste
	// 		filaObjetivo--;
	// 		columnaObjetivo++;
	// 		break;
	// 	case 2: // Este
	// 		columnaObjetivo++;
	// 		break;
	// 	case 3: // Sureste
	// 		filaObjetivo++;
	// 		columnaObjetivo++;
	// 		break;
	// 	case 4: // Sur
	// 		filaObjetivo++;
	// 		break;
	// 	case 5: // Suroeste
	// 		filaObjetivo++;
	// 		columnaObjetivo--;
	// 		break;
	// 	case 6: // Oeste
	// 		columnaObjetivo--;
	// 		break;
	// 	case 7: // Noroeste
	// 		filaObjetivo--;
	// 		columnaObjetivo--;
	// 		break;
	// }

	// // Moverse a la celda objetivo
	// if (filaObjetivo != current_state.fil || columnaObjetivo != current_state.col) {
	// 	if (sensores.terreno[ir_a_casilla] == 'M' || sensores.terreno[ir_a_casilla] == 'A' || sensores.terreno[ir_a_casilla] == 'L') {
	// 		// Casilla intransitable, buscar otro camino
	// 		// COMPLETAR ---------------------------<<<------------------------------------------------------------<<<
	// 	} else {
	// 		if (filaObjetivo < current_state.fil) {
	// 			accion = actWALK;
	// 		} else if (filaObjetivo > current_state.fil) {
	// 			accion = actRUN;
	// 		} else if (columnaObjetivo < current_state.col) {
	// 			accion = actTURN_L;
	// 		} else if (columnaObjetivo > current_state.col) {
	// 			accion = actTURN_SR;
	// 		}
	// 	}
	// } else {
	// 	// Ya se encuentra en la celda objetivo
	// 	accion = actIDLE;
	// }

	// // Si no hay ninguna de las anteriores, se elige una casilla según la matriz de instantes.
	// // COMPLETAR ---------------------------<<<------------------------------------------------------------<<<


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


	// Recordar la ultima accion
	last_action = accion;

	return accion;
	
}

	//------------------------------------------------------------------------------//
	//------------------Implementación de métodos auxiliares------------------------//
	//------------------------------------------------------------------------------//

int ComportamientoJugador::interact(Action accion, int valor)
{
	return false;
}

void ComportamientoJugador::PonerTerrenoEnMatriz(const vector<unsigned char> &terreno, 
			const state &st, vector< vector< unsigned char> > &matriz){
	
	// Según la orinetación del agente, se coloca el terreno en la matriz.
	switch (st.brujula)
	{
	case norte:
		cout << "Pinta terreno al norte\n";
		matriz[st.fil][st.col] = terreno[0];
		matriz[st.fil - 1][st.col - 1] = terreno[1];
		matriz[st.fil - 1][st.col] = terreno[2];
		matriz[st.fil - 1][st.col + 1] = terreno[3];
		matriz[st.fil - 2][st.col - 2] = terreno[4];
		matriz[st.fil - 2][st.col - 1] = terreno[5];
		matriz[st.fil - 2][st.col] = terreno[6];
		matriz[st.fil - 2][st.col + 1] = terreno[7];
		matriz[st.fil - 2][st.col + 2] = terreno[8];
		matriz[st.fil - 3][st.col - 3] = terreno[9];
		matriz[st.fil - 3][st.col - 2] = terreno[10];
		matriz[st.fil - 3][st.col - 1] = terreno[11];
		matriz[st.fil - 3][st.col] = terreno[12];
		matriz[st.fil - 3][st.col + 1] = terreno[13];
		matriz[st.fil - 3][st.col + 2] = terreno[14];
		matriz[st.fil - 3][st.col + 3] = terreno[15];
		break;
	case noreste:
		cout << "Pinta terreno al noreste\n";
		matriz[st.fil][st.col] = terreno[0];
		matriz[st.fil - 1][st.col] = terreno[1];
		matriz[st.fil - 1][st.col + 1] = terreno[2];
		matriz[st.fil][st.col + 1] = terreno[3];
		matriz[st.fil - 2][st.col] = terreno[4];
		matriz[st.fil - 2][st.col + 1] = terreno[5];
		matriz[st.fil - 2][st.col + 2] = terreno[6];
		matriz[st.fil - 1][st.col + 2] = terreno[7];
		matriz[st.fil][st.col + 2] = terreno[8];
		matriz[st.fil - 3][st.col] = terreno[9];
		matriz[st.fil - 3][st.col + 1] = terreno[10];
		matriz[st.fil - 3][st.col + 2] = terreno[11];
		matriz[st.fil - 3][st.col + 3] = terreno[12];
		matriz[st.fil - 2][st.col + 3] = terreno[13];
		matriz[st.fil - 1][st.col + 3] = terreno[14];
		matriz[st.fil][st.col + 3] = terreno[15];
		break;
	case este:
		cout << "Pinta terreno al este\n";
		matriz[st.fil][st.col] = terreno[0];
		matriz[st.fil - 1][st.col + 1] = terreno[1];
		matriz[st.fil][st.col + 1] = terreno[2];
		matriz[st.fil + 1][st.col + 1] = terreno[3];
		matriz[st.fil - 2][st.col + 2] = terreno[4];
		matriz[st.fil - 1][st.col + 2] = terreno[5];
		matriz[st.fil][st.col + 2] = terreno[6];
		matriz[st.fil + 1][st.col + 2] = terreno[7];
		matriz[st.fil + 2][st.col + 2] = terreno[8];
		matriz[st.fil - 3][st.col + 3] = terreno[9];
		matriz[st.fil - 2][st.col + 3] = terreno[10];
		matriz[st.fil - 1][st.col + 3] = terreno[11];
		matriz[st.fil][st.col + 3] = terreno[12];
		matriz[st.fil + 1][st.col + 3] = terreno[13];
		matriz[st.fil + 2][st.col + 3] = terreno[14];
		matriz[st.fil + 3][st.col + 3] = terreno[15];
		break;
	case sureste:
		cout << "Pinta terreno al sureste\n";
		matriz[st.fil][st.col] = terreno[0];
		matriz[st.fil][st.col + 1] = terreno[1];
		matriz[st.fil + 1][st.col + 1] = terreno[2];
		matriz[st.fil + 1][st.col] = terreno[3];
		matriz[st.fil][st.col + 2] = terreno[4];
		matriz[st.fil + 1][st.col + 2] = terreno[5];
		matriz[st.fil + 2][st.col + 2] = terreno[6];
		matriz[st.fil + 2][st.col + 1] = terreno[7];
		matriz[st.fil + 2][st.col] = terreno[8];
		matriz[st.fil][st.col + 3] = terreno[9];
		matriz[st.fil + 1][st.col + 3] = terreno[10];
		matriz[st.fil + 2][st.col + 3] = terreno[11];
		matriz[st.fil + 3][st.col + 3] = terreno[12];
		matriz[st.fil + 3][st.col + 2] = terreno[13];
		matriz[st.fil + 3][st.col + 1] = terreno[14];
		matriz[st.fil + 3][st.col] = terreno[15];
		break;
	case sur:
		cout << "Pinta terreno al sur\n";
		matriz[st.fil][st.col] = terreno[0];
		matriz[st.fil + 1][st.col + 1] = terreno[1];
		matriz[st.fil + 1][st.col] = terreno[2];
		matriz[st.fil + 1][st.col - 1] = terreno[3];
		matriz[st.fil + 2][st.col + 2] = terreno[4];
		matriz[st.fil + 2][st.col + 1] = terreno[5];
		matriz[st.fil + 2][st.col] = terreno[6];
		matriz[st.fil + 2][st.col - 1] = terreno[7];
		matriz[st.fil + 2][st.col - 2] = terreno[8];
		matriz[st.fil + 3][st.col + 3] = terreno[9];
		matriz[st.fil + 3][st.col + 2] = terreno[10];
		matriz[st.fil + 3][st.col + 1] = terreno[11];
		matriz[st.fil + 3][st.col] = terreno[12];
		matriz[st.fil + 3][st.col - 1] = terreno[13];
		matriz[st.fil + 3][st.col - 2] = terreno[14];
		matriz[st.fil + 3][st.col - 3] = terreno[15];
		break;
	case suroeste:
		cout << "Pinta terreno al suroeste\n";
		matriz[st.fil][st.col] = terreno[0];
		matriz[st.fil + 1][st.col] = terreno[1];
		matriz[st.fil + 1][st.col - 1] = terreno[2];
		matriz[st.fil][st.col - 1] = terreno[3];
		matriz[st.fil + 2][st.col] = terreno[4];
		matriz[st.fil + 2][st.col - 1] = terreno[5];
		matriz[st.fil + 2][st.col - 2] = terreno[6];
		matriz[st.fil + 1][st.col - 2] = terreno[7];
		matriz[st.fil][st.col - 2] = terreno[8];
		matriz[st.fil + 3][st.col] = terreno[9];
		matriz[st.fil + 3][st.col - 1] = terreno[10];
		matriz[st.fil + 3][st.col - 2] = terreno[11];
		matriz[st.fil + 3][st.col - 3] = terreno[12];
		matriz[st.fil + 2][st.col - 3] = terreno[13];
		matriz[st.fil + 1][st.col - 3] = terreno[14];
		matriz[st.fil][st.col - 3] = terreno[15];
		break;
	
	case oeste:
		cout << "Pinta terreno al oeste\n";
		matriz[st.fil][st.col] = terreno[0];
		matriz[st.fil + 1][st.col - 1] = terreno[1];
		matriz[st.fil][st.col - 1] = terreno[2];
		matriz[st.fil - 1][st.col - 1] = terreno[3];
		matriz[st.fil + 2][st.col - 2] = terreno[4];
		matriz[st.fil + 1][st.col - 2] = terreno[5];
		matriz[st.fil][st.col - 2] = terreno[6];
		matriz[st.fil - 1][st.col - 2] = terreno[7];
		matriz[st.fil - 2][st.col - 2] = terreno[8];
		matriz[st.fil + 3][st.col - 3] = terreno[9];
		matriz[st.fil + 2][st.col - 3] = terreno[10];
		matriz[st.fil + 1][st.col - 3] = terreno[11];
		matriz[st.fil][st.col - 3] = terreno[12];
		matriz[st.fil - 1][st.col - 3] = terreno[13];
		matriz[st.fil - 2][st.col - 3] = terreno[14];
		matriz[st.fil - 3][st.col - 3] = terreno[15];
		break;
	
	case noroeste:
		cout << "Pinta terreno al noroeste\n";
		matriz[st.fil][st.col] = terreno[0];
		matriz[st.fil][st.col - 1] = terreno[1];
		matriz[st.fil - 1][st.col - 1] = terreno[2];
		matriz[st.fil - 1][st.col] = terreno[3];
		matriz[st.fil][st.col - 2] = terreno[4];
		matriz[st.fil - 1][st.col - 2] = terreno[5];
		matriz[st.fil - 2][st.col - 2] = terreno[6];
		matriz[st.fil - 2][st.col - 1] = terreno[7];
		matriz[st.fil - 2][st.col] = terreno[8];
		matriz[st.fil][st.col - 3] = terreno[9];
		matriz[st.fil - 1][st.col - 3] = terreno[10];
		matriz[st.fil - 2][st.col - 3] = terreno[11];
		matriz[st.fil - 3][st.col - 3] = terreno[12];
		matriz[st.fil - 3][st.col - 2] = terreno[13];
		matriz[st.fil - 3][st.col - 1] = terreno[14];
		matriz[st.fil - 3][st.col] = terreno[15];
		break;
	}

}

void ComportamientoJugador::RecargarPilas(Sensores& sensores){
	if(sensores.terreno[0] == 'X' and necesita_recargar and queda_poca_vida){
		recargando_pilas = true;
		cout << "Recargando mi bateria... Dame un descanso\n";
		sensores.bateria += 10;
	}else
		recargando_pilas = false;
}

bool ComportamientoJugador::SiguienteCasillaLibre(const Sensores &sensores){
	bool siguiente_casilla_libre = false;
	if( (sensores.terreno[2] == 'T' 	// Suelo Arenoso
		or sensores.terreno[2] == 'S' 	// Suelo Pedregoso
		or sensores.terreno[2] == 'G' 	// Posicinamiento
		or sensores.terreno[2] == 'B'	// Bosque
		or sensores.terreno[2] == 'A'	// Agua
		or sensores.terreno[2] == 'K'	// Bikini
		or sensores.terreno[2] == 'D'	// Zapatillas
		or sensores.terreno[2] == 'X'	// Recarga
		) and sensores.agentes[2] == '_'
		  and !sensores.colision
		  and !recargando_pilas){	// Si no esta recargando
			siguiente_casilla_libre = true;
	}

	return siguiente_casilla_libre;
		
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