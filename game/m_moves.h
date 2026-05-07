#pragma once
#include "g_local.h"

typedef void (*moves)(edict_t*);

void explode(edict_t* ent, edict_t* other, int damage, int radius);
int get_enemy(edict_t* self);
int get_all_enemies(edict_t* self, edict_t** enemies, int dist);
int get_all_allies(edict_t* self, edict_t** enemies, int dist);

void stand(edict_t* self);

void tackle(edict_t* self);
void Tackle(edict_t* self);

void bulletSeed(edict_t* self);
void BulletSeed(edict_t* self);

void Harden(edict_t* self);

void Explosion(edict_t* self);

void Hyperbeam(edict_t* self);

void BulkUp(edict_t* self);

void vineWhipRight(edict_t* self);
void vineWhipLeft(edict_t* self);
void VineWhip(edict_t* self);

void FlashCannon(edict_t* self);

void Hypnosis(edict_t* self);


void Screech(edict_t* self);

void RevivalBlessing(edict_t* self);


void Rest(edict_t* self);

void HealPulse(edict_t* self);

void SludgeBomb(edict_t* self);

void rageFist(edict_t* self);
void RageFist(edict_t* self);

void Struggle(edict_t* self);

void ThunderWave(edict_t* self);

void DrainPunch(edict_t* self);

void AuraSphere(edict_t* self);

void Agility(edict_t* self);

void Fly(edict_t* self);
void Hover(edict_t* self);
void Land(edict_t* self);

moves* getMoves(edict_t* self);

char** getMovesName(edict_t* self);