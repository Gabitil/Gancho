#ifndef PLAYER_ANIMS_H
#define PLAYER_ANIMS_H

// Estados de Animação
typedef enum {
  ANIM_IDLE,     // Parado (player.obj)
  ANIM_RUNNING,  // Correndo (player_correndo_1 a 5.obj)
  ANIM_GRAPPLER  // Gancho (player_grappler.obj)
} PlayerAnimState;

// Variáveis de Animação Globais (Declarações)
extern PlayerAnimState currentPlayerAnimState;
extern float animTimer;

#endif  // PLAYER_ANIMS_H