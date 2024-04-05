#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "comportamientos/comportamiento.hpp"
using namespace std;

struct state // Estado
{
  int fil;
  int col;
  Orientacion brujula;
};


class ComportamientoJugador : public Comportamiento{

  public:
    ComportamientoJugador(unsigned int size) : Comportamiento(size){
      // Constructor de la clase
      current_state.fil = 99;
      current_state.col = 99;
      current_state.brujula = norte;
      last_action = actIDLE;
      girar_derecha = false;
      bien_situado = false;
      bikini_puesto = false;
      zapatillas_puestas = false;
      recargando_pilas = false;
      necesita_recargar = false;
      queda_poca_vida = false;
      accion_pendiente = false;
      
      mapaAuxiliar = vector<vector<unsigned char>>(size, vector<unsigned char>(size, '?'));
     }
     
    ComportamientoJugador(const ComportamientoJugador & comport) : Comportamiento(comport){}
    ~ComportamientoJugador(){}

    Action think(Sensores sensores);
    int interact(Action accion, int valor);

    void PonerTerrenoEnMatriz(const vector<unsigned char> &terreno, const state &st, 
							vector< vector< unsigned char> > &matriz, Sensores& sensores);
    
    void PonerTerrenoEnAuxiliar(const vector<unsigned char> &terreno, const state &st, 
							vector< vector< unsigned char> > &matriz, Sensores& sensores);
    
    void PonerInstanteEnMatriz(const int& instante_actual, const state &st, 
              vector< vector<int> > &matriz);
    
    void RecargarPilas(Sensores& sensores);

    bool CasillaLibre(const Sensores &sensores, const int& num_casilla);

    bool PuedoCorrer(const Sensores &sensores);

    Action GiraRandomDerecha(const Sensores &sensores);

    int CasillaAntigua(const Sensores &sensores, const vector< vector<int> >& matriz_instantes);

    Action SiguienteAccion(const int& i);

    int CasillaSinPintar(const int& i, const Sensores &sensores);

  private:
  const int BATERIA_MAX = 5000;
  const int BATERIA_MIN = 1000;
  const int TAM_SENSORES_TERR_AGEN = 16;

  // Declarar aquí las variables de estado
  state current_state; Orientacion brujula;
  Action last_action;
  bool girar_derecha,
       bien_situado, 
       bikini_puesto, 
       zapatillas_puestas, 
       recargando_pilas,
       necesita_recargar,
	     queda_poca_vida,
       accion_pendiente;

  vector< vector< unsigned char> > mapaAuxiliar;
};
#endif
