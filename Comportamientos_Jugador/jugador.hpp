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
      last_rule = 0,
      reinicio_bloqueante = false,
      tamanio_mapa = size;
      iteraciones_desde_bloqueo = 0;
    }
     
    ComportamientoJugador(const ComportamientoJugador & comport) : Comportamiento(comport){}
    ~ComportamientoJugador(){}

    Action think(Sensores sensores);
    int interact(Action accion, int valor);

    void PonerTerrenoEnMatriz(const vector<unsigned char> &terreno, const state &st, 
							vector< vector< unsigned char> > &matriz, Sensores& sensores);
    
    void RecargarPilas(Sensores& sensores);

    bool casillaLibre(const Sensores &sensores, const int& num_casilla);

    Action GiroRandom(const Sensores &sensores);

    Action SiguienteAccion(const Sensores &sensores, const int& i);

    bool esMuro(const Sensores &sensores, const int& num_casilla);

    bool esPrecipicio(const Sensores &sensores, const int& num_casilla);

    bool casillaNoTransitable(const Sensores& sensores, const int& num_casilla);

    Orientacion orientacionActual(const Sensores &sensores, const state& st);

    void reinicio();

    void Posicionar (const Sensores &sensores, bool &bien_situado);

    Action salirDelPaso(const Sensores &sensores);

    void pintaBordes(const int& n, vector< vector< unsigned char> > &mapa);

    bool casillaLibreEspecial(const Sensores &sensores, const int& num_casilla);


  private:
  const int BATERIA_MAX_CARGA = 5000;
  const int BATERIA_MIN = 4000;
  const int TAM_SENSORES_TERR_AGEN = 16;

  // Declarar aquí las variables de estado
  
  state current_state; Orientacion brujula;
  Action last_action;;
  bool girar_derecha,
       bien_situado, 
       bikini_puesto, 
       zapatillas_puestas, 
       recargando_pilas,
       necesita_recargar,
	     queda_poca_vida,
       accion_pendiente,
       reinicio_bloqueante;
  int last_rule, tamanio_mapa,iteraciones_desde_bloqueo;

};
#endif
