//
// Created by lozio on 20/05/25.
//

#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H


// Inizializza il gestore degli ingressi (configura i pin)
void input_manager_setup();

// Verifica se il pulsante di uscita è stato appena premuto (rilevamento fronte di discesa con anti-rimbalzo)
// Restituisce true una sola volta per pressione, dopo che lo stato si è stabilizzato.
bool input_manager_is_exit_button_newly_pressed();


#endif //INPUT_MANAGER_H
