#pragma once
static void check_dodge(edict_t* self, vec3_t start, vec3_t dir, int speed);
qboolean fire_hit(edict_t* self, vec3_t aim, int damage, int kick);
static void fire_lead(edict_t* self, vec3_t start, vec3_t aimdir, int damage, int kick, int te_impact, int hspread, int vspread, int mod);
void fire_bullet(edict_t* self, vec3_t start, vec3_t aimdir, int damage, int kick, int hspread, int vspread, int mod);
void fire_shotgun(edict_t* self, vec3_t start, vec3_t aimdir, int damage, int kick, int hspread, int vspread, int count, int mod);

float distance(edict_t* ent1, edict_t* ent2);
edict_t* get_nearest_enemy(edict_t* enemy, edict_t* self);
void blaster_think(edict_t* self);
void blaster_touch(edict_t* self, edict_t* other, cplane_t* plane, csurface_t* surf);
void fire_blaster(edict_t* self, vec3_t start, vec3_t dir, int damage, int speed, int effect, qboolean hyper);
static void Grenade_Explode(edict_t* ent);
static void Grenade_Touch(edict_t* ent, edict_t* other, cplane_t* plane, csurface_t* surf);

void fire_grenade(edict_t* self, vec3_t start, vec3_t aimdir, int damage, int speed, float timer, float damage_radius);
void fire_grenade2(edict_t* self, vec3_t start, vec3_t aimdir, int damage, int speed, float timer, float damage_radius, qboolean held);
void fire_grenade3(edict_t* self, vec3_t start, vec3_t aimdir, int damage, int speed, float timer, float damage_radius, qboolean held, qboolean poison);
void PokeBall_Touch(edict_t* ent, edict_t* other, cplane_t* plane, csurface_t* surf);
void fire_pokeball(edict_t* self, vec3_t start, vec3_t aimdir, int damage, int speed, float timer, float damage_radius, qboolean held);
static void P_ProjectSource(gclient_t* client, vec3_t point, vec3_t distance, vec3_t forward, vec3_t right, vec3_t result);
void item_explode(edict_t* self);
void fire_item(edict_t* self, vec3_t start, vec3_t aimdir, int damage, int speed, float timer, float damage_radius, void (*touch)(edict_t* ent, edict_t* other, cplane_t* plane, csurface_t* surf), void(*explode)(edict_t* ent));


static void XSpeed_Touch(edict_t* ent, edict_t* other, cplane_t* plane, csurface_t* surf);
static void XSpeed_Effect(edict_t* ent, edict_t* other);
static void XSpeed_Explode(edict_t* ent);
void fire_xSpeed(edict_t* ent);

static void XAttack_Effect(edict_t* ent, edict_t* other);
static void XAttack_Explode(edict_t* ent);
static void XAttack_Touch(edict_t* ent, edict_t* other, cplane_t* plane, csurface_t* surf);
void fire_xAttack(edict_t* ent);

static void XDefense_Effect(edict_t* ent, edict_t* other);
static void XDefense_Explode(edict_t* ent);
static void XDefense_Touch(edict_t* ent, edict_t* other, cplane_t* plane, csurface_t* surf);
void fire_xDefense(edict_t* ent);

static void Potion_Effect(edict_t* ent, edict_t* other);
static void Potion_Explode(edict_t* ent);
static void Potion_Touch(edict_t* ent, edict_t* other, cplane_t* plane, csurface_t* surf);
void fire_potion(edict_t* ent);

static void Full_Heal_Effect(edict_t* ent, edict_t* other);
static void Full_Heal_Explode(edict_t* ent);
static void Full_Heal_Touch(edict_t* ent, edict_t* other, cplane_t* plane, csurface_t* surf);
void fire_full_heal(edict_t* ent);

void rocket_touch(edict_t* ent, edict_t* other, cplane_t* plane, csurface_t* surf);

void fire_rocket(edict_t* self, vec3_t start, vec3_t dir, int damage, int speed, float damage_radius, int radius_damage);
void fire_rail(edict_t* self, vec3_t start, vec3_t aimdir, int damage, int kick);
void bfg_explode(edict_t* self);
void bfg_touch(edict_t* self, edict_t* other, cplane_t* plane, csurface_t* surf);
void bfg_think(edict_t* self);
void fire_bfg(edict_t* self, vec3_t start, vec3_t dir, int damage, int speed, float damage_radius);