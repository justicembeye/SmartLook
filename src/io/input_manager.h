//
// Created by lozio on 20/05/25.
//

#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H


// Initialise le gestionnaire d'entrées (configure les broches)
void input_manager_setup();

// Vérifie si le bouton de sortie vient d'être pressé (détection de front descendant avec anti-rebond)
// Retourne true une seule fois par appui, après que l'état s'est stabilisé.
bool input_manager_is_exit_button_newly_pressed();

// (Optionnel) Fonction pour lire l'état brut (stable) du bouton si besoin ailleurs
// bool input_manager_get_exit_button_stable_state();


#endif //INPUT_MANAGER_H
