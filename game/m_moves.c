#include "m_moves.h"

typedef void (*moves)(edict_t*);

void explode(edict_t* ent, int damage, int radius)
{
	vec3_t		origin;
	if (ent->owner->client)
		PlayerNoise(ent->owner, ent->s.origin, PNOISE_IMPACT);

	VectorMA(ent->s.origin, -0.02, ent->velocity, origin);
	T_RadiusDamage(ent, ent, damage, NULL, radius, MOD_R_SPLASH);

	gi.WriteByte(svc_temp_entity);
	if (ent->waterlevel)
		gi.WriteByte(TE_ROCKET_EXPLOSION_WATER);
	else
		gi.WriteByte(TE_ROCKET_EXPLOSION);
	gi.WritePosition(origin);
	gi.multicast(ent->s.origin, MULTICAST_PHS);

	
}
int get_enemy(edict_t* self) {
	if (!self || !self->inuse) return false;
	edict_t* current;
	current = g_edicts;
	float dist = -1;
	int found = false;
	for (; current < &g_edicts[globals.num_edicts]; current++) {
		if (!current->classname) {
			continue;
		}
		if (current == self){
			continue;
		}
		if (!(current->svflags & SVF_MONSTER)) {
			continue;
		}
		if (current->pokeTeam == self->pokeTeam) {
			continue;
		}
		if (current->deadflag & DEAD_DEAD) {
			continue;
		}
		
		if (distance(current, self) <= dist || dist == -1) {
			self->enemy = current;
			found = true;
			
			dist = distance(current, self);
		}
	}
	return found;
}

edict_t* get_dead_ally(edict_t* self, edict_t* ally) {
	edict_t* current;
	current = g_edicts;
	float dist = -1;
	for (; current < &g_edicts[globals.num_edicts]; current++) {
		if (!current->classname) {
			continue;
		}
		if (!(current->svflags & SVF_MONSTER)) {
			continue;
		}
		if (!(current->pokeTeam == self->pokeTeam)) {
			continue;
		}
		if (!(current->deadflag & DEAD_DEAD)) {
			continue;
		}
		if (distance(current, self) <= dist || dist == -1) {
			ally = current;
			dist = distance(current, self);
		}
	}
	return ally;
}
int get_all_enemies(edict_t* self, edict_t** enemies, int dist) {
	edict_t* current;
	int i = 0;
	current = g_edicts;
	for (; current < &g_edicts[globals.num_edicts]; current++) {
		if (!current->classname) {
			continue;
		}
		if (!(current->svflags & SVF_MONSTER)) {
			continue;
		}
		if (!(current->pokeTeam == self->pokeTeam)) {
			continue;
		}
		if (current->deadflag & DEAD_DEAD) {
			continue;
		}
		if (distance(current, self) <= dist) {
			enemies[i] = &current;
			i++;
		}
	}
	return i;
}
int get_all_allies(edict_t* self, edict_t** enemies, int dist) {
	edict_t* current;
	int i = 0;
	current = g_edicts;
	for (; current < &g_edicts[globals.num_edicts]; current++) {
		if (!current->classname) {
			continue;
		}
		if (!(current->svflags & SVF_MONSTER)) {
			continue;
		}
		if (!(current->pokeTeam != self->pokeTeam)) {
			continue;
		}
		if (current->deadflag & DEAD_DEAD) {
			continue;
		}
		if (distance(current, self) <= dist) {
			enemies[i] = &current;
			i++;
		}
	}
	return i;
}

mframe_t idle_frames[] = {
	ai_walk, 0, NULL,
	ai_walk, 0, NULL  
};
mmove_t move_idle = { 0, 1, idle_frames, stand }; 
void stand(edict_t* self) {
	self->monsterinfo.currentmove = &move_idle;
}
//Tackle code
void tackle(edict_t* self)
{
	static	vec3_t	aim = { MELEE_DISTANCE, 0, -24 };
	fire_hit(self, aim, 20, 400);
}//function that actually attacks
mframe_t tackle_frames[] = {
	ai_run, 10, NULL,
	ai_run, 10, NULL,
	ai_run, 10, NULL,
	ai_run, 10, NULL,
	ai_run, 10, tackle
};//frame data (schedules movement and attacks)
mmove_t move_tackle = { 0, 4, tackle_frames, stand };//Start and end frame and what to do next
void Tackle(edict_t* self) {
	int enemy = get_enemy(self);
	if (enemy == false) {
		gi.dprintf("no enemies\n");
		return;
	}
	self->monsterinfo.currentmove = &move_tackle;
}//Begins the tackle action

//Bullet Seed code
void bulletSeed(edict_t* self) {
	vec3_t  start;
	vec3_t  forward, right, up;
	vec3_t  aim;
	vec3_t  dir;
	vec3_t  end;
	float   r, u;

	if (!self->enemy || !self->enemy->inuse)
		return;

	// Compute proper muzzle position like the soldier does
	AngleVectors(self->s.angles, forward, right, NULL);
	G_ProjectSource(self->s.origin, monster_flash_offset[MZ2_SOLDIER_BLASTER_1], forward, right, start);
	if (!start) {
		gi.dprintf("bad projection\n");
		return;
	}
	VectorCopy(self->enemy->s.origin, end);
	end[2] += self->enemy->viewheight;
	VectorSubtract(end, start, aim);
	vectoangles(aim, dir);
	AngleVectors(dir, forward, right, up);

	r = crandom() * 1000;
	u = crandom() * 500;
	VectorMA(start, 8192, forward, end);
	VectorMA(end, r, right, end);
	VectorMA(end, u, up, end);

	VectorSubtract(end, start, aim);
	VectorNormalize(aim);

	monster_fire_blaster(self, start, aim, 5, 600, MZ2_SOLDIER_BLASTER_1, EF_BLASTER);
}
mframe_t bullet_seed_frames[] = {
	ai_move, 0, bulletSeed,
	ai_move, 10, NULL,
	ai_move, 10, NULL,
	ai_move, 0, bulletSeed,
	ai_move, 10, NULL,
	ai_move, 10, NULL,
	ai_move, 0, bulletSeed,
	ai_move, 10, NULL,
	ai_move, 10, NULL,
	ai_move, 0, bulletSeed,
	ai_move, 10, NULL,
	ai_move, 10, NULL,
	ai_move, 0, bulletSeed,
	ai_move, 10, NULL,
	ai_move, 10, NULL
};
mmove_t move_bullet_seed = { 0, 2, bullet_seed_frames, stand };
void BulletSeed(edict_t* self) {
	int enemy = get_enemy(self);
	if (enemy == false) {
		gi.dprintf("no enemies\n");
		return;
	}
	move_bullet_seed.lastframe = (1 + rand() % 4) * 3 - 1;
	self->monsterinfo.currentmove = &move_bullet_seed;
}


void Harden(edict_t* self) {
	self->s.renderfx |= RF_SHELL_GREEN;
	self->grey = 1;
	self->defenseMod += 0.5;
}


void Explosion(edict_t* self) {
	explode(self, 1000, 200);
}


void get_offsets(vec3_t pos, vec3_t orient, int radius, vec3_t* points[6]) {
	vec3_t n, u, v;

	float mag = sqrt(orient[0] * orient[0] + orient[1] * orient[1] + orient[2] * orient[2]);
	for (int i = 0; i < 3; i++) n[i] = orient[i] / mag;

	float temp[3] = { 0, 0, 0 };
	if (fabs(n[0]) < 0.9) temp[0] = 1; else temp[1] = 1;

	u[0] = n[1] * temp[2] - n[2] * temp[1];
	u[1] = n[2] * temp[0] - n[0] * temp[2];
	u[2] = n[0] * temp[1] - n[1] * temp[0];

	float u_mag = sqrt(u[0] * u[0] + u[1] * u[1] + u[2] * u[2]);
	for (int i = 0; i < 3; i++) u[i] /= u_mag;

	v[0] = n[1] * u[2] - n[2] * u[1];
	v[1] = n[2] * u[0] - n[0] * u[2];
	v[2] = n[0] * u[1] - n[1] * u[0];

	for (int i = 0; i < 6; i++) {
		float theta = (2.0f * M_PI * i) / 6.0f;
		float cos_t = cos(theta);
		float sin_t = sin(theta);
		vec3_t point;
		for (int j = 0; j < 3; j++) {
			point[j] = pos[j] + radius * (cos_t * u[j] + sin_t * v[j]);
		}
		points[i] = point;
	}
}
void hyperBeam(edict_t* self) {
	vec3_t	start;
	vec3_t	forward, right, up;
	vec3_t	aim;
	vec3_t	dir;
	vec3_t	end;
	float	r, u;
	VectorCopy(self->enemy->s.origin, end);
	end[2] += self->enemy->viewheight;
	VectorSubtract(end, start, aim);
	vectoangles(aim, dir);
	AngleVectors(dir, forward, right, up);

	r = crandom() * 1000;
	u = crandom() * 500;
	VectorMA(start, 8192, forward, end);
	VectorMA(end, r, right, end);
	VectorMA(end, u, up, end);

	VectorSubtract(end, start, aim);
	VectorNormalize(aim);
	vec3_t* points[6];
	get_offsets(self->s.origin, self->s.angles, 50, points);
	for (int i = 0; i < 6; i++) {
		monster_fire_railgun(self, points[i], aim, 5, 200, EF_HYPERBLASTER);
	}
}
mframe_t hyper_beam_frames[] = {
	ai_charge, 10, NULL,
	ai_charge, 10, NULL,
	ai_charge, 10, NULL,
	ai_charge, 10, NULL,
	ai_charge, 10, hyperBeam,
	ai_charge, 10, hyperBeam,
	ai_charge, 10, hyperBeam
};
mmove_t move_hyper_beam = { 0, 10, hyper_beam_frames, ai_stand };
void Hyperbeam(edict_t* self) {
	int enemy = get_enemy(self);
	if (enemy == false) {
		gi.dprintf("no enemies\n");
		return;
	}
	self->monsterinfo.currentmove = &move_hyper_beam;
}


void BulkUp(edict_t* self) {
	self->red = 1;
	self->damageMod += 0.5;
}


void vineWhipRight(edict_t* self) {
	vec3_t	aim;

	VectorSet(aim, MELEE_DISTANCE, self->maxs[0], 8);
	if (fire_hit(self, aim, 20, 40)) {}
		//gi.sound(self, CHAN_WEAPON, sound_melee3, 1, ATTN_NORM, 0);
}
void vineWhipLeft(edict_t* self) {
	vec3_t	aim;

	VectorSet(aim, MELEE_DISTANCE, self->mins[0], 8);
	if (fire_hit(self, aim, 20, 40)) {}
		//gi.sound(self, CHAN_WEAPON, sound_melee3, 1, ATTN_NORM, 0);
}
mframe_t vine_whip_frames[] = {
	ai_charge, 10, NULL,
	ai_charge, 10, NULL,
	ai_charge, 10, NULL,
	ai_charge, 10, NULL,
	ai_charge, 10, vineWhipRight,
	ai_charge, 10, NULL,
	ai_charge, 10, NULL,
	ai_charge, 10, NULL,
	ai_charge, 10, NULL,
	ai_charge, 10, vineWhipLeft
};
mmove_t move_vine_whip = {0, 4, vine_whip_frames, ai_stand};
void VineWhip(edict_t* self) {
	int enemy = get_enemy(self);
	if (enemy == false) {
		gi.dprintf("no enemies\n");
		return;
	}
	self->monsterinfo.currentmove = &move_vine_whip;
}

void flashCannon(edict_t* self) {
	edict_t* ent;
	edict_t* ignore;
	vec3_t	point;
	vec3_t	dir;
	vec3_t	start;
	vec3_t	end;
	int		dmg;
	trace_t	tr;

	if (deathmatch->value)
		dmg = 5;
	else
		dmg = 10;

	ent = NULL;
	while ((ent = findradius(ent, self->s.origin, 256)) != NULL){
		if (ent == self)
			continue;

		if (ent->pokeTeam == self->pokeTeam) {
			continue;
		}

		if (ent == self->owner)
			continue;

		if (!ent->takedamage)
			continue;

		if (!(ent->svflags & SVF_MONSTER) && (!ent->client) && (strcmp(ent->classname, "misc_explobox") != 0))
			continue;

		VectorMA(ent->absmin, 0.5, ent->size, point);

		VectorSubtract(point, self->s.origin, dir);
		VectorNormalize(dir);

		ignore = self;
		VectorCopy(self->s.origin, start);
		VectorMA(start, 2048, dir, end);
		while (1)
		{
			tr = gi.trace(start, NULL, NULL, end, ignore, CONTENTS_SOLID | CONTENTS_MONSTER | CONTENTS_DEADMONSTER);

			if (!tr.ent)
				break;

			if ((tr.ent->takedamage) && !(tr.ent->flags & FL_IMMUNE_LASER) && (tr.ent != self->owner))
				T_Damage(tr.ent, self, self->owner, dir, tr.endpos, vec3_origin, dmg, 1, DAMAGE_ENERGY, MOD_BFG_LASER);

			if (!(tr.ent->svflags & SVF_MONSTER) && (!tr.ent->client))
			{
				gi.WriteByte(svc_temp_entity);
				gi.WriteByte(TE_LASER_SPARKS);
				gi.WriteByte(4);
				gi.WritePosition(tr.endpos);
				gi.WriteDir(tr.plane.normal);
				gi.WriteByte(self->s.skinnum);
				gi.multicast(tr.endpos, MULTICAST_PVS);
				break;
			}

			ignore = tr.ent;
			VectorCopy(tr.endpos, start);
		}

		gi.WriteByte(svc_temp_entity);
		gi.WriteByte(TE_BFG_LASER);
		gi.WritePosition(self->s.origin);
		gi.WritePosition(tr.endpos);
		gi.multicast(self->s.origin, MULTICAST_PHS);
	}

}
mframe_t flash_cannon_frames[] = {
	ai_stand, 0, flashCannon,
	ai_stand, 0, flashCannon,
	ai_stand, 0, flashCannon,
	ai_stand, 0, flashCannon,
	ai_stand, 0, flashCannon,
	ai_stand, 0, flashCannon,
	ai_stand, 0, flashCannon,
	ai_stand, 0, flashCannon,
	ai_stand, 0, flashCannon,
	ai_stand, 0, flashCannon,
	ai_stand, 0, flashCannon
};
mmove_t move_flash_cannon = { 0, 10, flash_cannon_frames, ai_stand };
void FlashCannon(edict_t* self) {
	self->monsterinfo.currentmove = &move_flash_cannon;
}


void Hypnosis(edict_t* self) {
	edict_t* enemies[MAX_EDICTS];
	int count = get_all_enemies(self, 1000, enemies);
	for (int i = 0; i < count; i++) {
		edict_t* enemy = enemies[i];
		enemy->sleep += 500;
		enemy->sleeped = 1;
		enemy->speed *= 0.5;
	}
}


void Screech(edict_t* self) {
	edict_t* enemies[MAX_EDICTS];
	int count = get_all_enemies(self, 1000, enemies);
	for (int i = 0; i < count; i++) {
		edict_t* enemy = enemies[i];
		enemy->defenseMod *= 0.5;
	}
}


void RevivalBlessing(edict_t* self) {
	edict_t* ally;
	get_dead_ally(self, ally);
	if (ally) {
		ally->health = ally->max_health;
		ally->deadflag = DEAD_NO;
		ally->monsterinfo.currentmove = ai_stand;
	}
}


void Rest(edict_t* self) {
	self->sleep = 500;
	self->health = self->max_health;
	self->sleeped = 1;
}


void HealPulse(edict_t* self) {
	edict_t* allies[MAX_EDICTS];
	int count = get_all_allies(self, 1000, allies);
	for (int i = 0; i < count; i++) {
		edict_t* ally = allies[i];
		ally->health = ally->health+50 >= ally->max_health ? ally->max_health: ally->health + 50;
		ally->green = 100;
	}
}


void sludgeBomb(edict_t* self) {
	vec3_t	start;
	vec3_t	forward, right, up;
	vec3_t	aim;
	vec3_t	dir;
	vec3_t	end;
	float	r, u;
	VectorCopy(self->enemy->s.origin, end);
	end[2] += self->enemy->viewheight;
	VectorSubtract(end, start, aim);
	vectoangles(aim, dir);
	AngleVectors(dir, forward, right, up);

	r = crandom() * 1000;
	u = crandom() * 500;
	VectorMA(start, 8192, forward, end);
	VectorMA(end, r, right, end);
	VectorMA(end, u, up, end);

	VectorSubtract(end, start, aim);
	VectorNormalize(aim);
	monster_fire_grenade(self, self->s.origin, aim, 5, 600, EF_GRENADE);
}
mframe_t sludge_bomb_frames[] = {
	ai_charge, 10, NULL,
	ai_charge, 10, NULL,
	ai_charge, 10, NULL,
	ai_charge, 10, NULL,
	ai_charge, 10, sludgeBomb
};
mmove_t move_sludge_bomb = {0, 4, sludge_bomb_frames, ai_stand};
void SludgeBomb(edict_t* self) {
	int enemy = get_enemy(self);
	if (enemy == false) {
		gi.dprintf("no enemies\n");
		return;
	}
	self->monsterinfo.currentmove = &move_sludge_bomb;
}


void rageFist(edict_t* self)
{
	static	vec3_t	aim = { MELEE_DISTANCE, 0, -24 };
	fire_hit(self, aim, 10 * self->takedamage, 400);
}//function that actually attacks
mframe_t rage_fist_frames[] = {
	ai_charge, 10, NULL,
	ai_charge, 10, NULL,
	ai_charge, 10, NULL,
	ai_charge, 10, NULL,
	ai_charge, 10, rageFist
};//frame data (schedules movement and attacks)
mmove_t move_rage_fist = { 0, 4, rage_fist_frames, ai_stand };
void RageFist(edict_t* self) {
	int enemy = get_enemy(self);
	if (enemy == false) {
		gi.dprintf("no enemies\n");
		return;
	}
	self->monsterinfo.currentmove = &move_rage_fist;
}


void Struggle(edict_t* self) {
	Tackle(self);
	vec3_t dir = { 0,0,0 };
	T_Damage(self, self, self, dir, dir, dir, 5, 0, 0, MOD_FRIENDLY_FIRE);
}


void ThunderWave(edict_t* self) {
	edict_t* enemies[MAX_EDICTS];
	int count = get_all_enemies(self, 1000, enemies);
	for (int i = 0; i < count; i++) {
		edict_t* enemy = enemies[i];
		enemy->paralysis = 500;
		enemy->paralyised =  1;
	}
}


void DrainPunch(edict_t* self) {
	Tackle(self);
	self->health = self->health + 25 >= self->max_health ? self->max_health : self->health + 25;
}


void auraSphere(edict_t* self) {
	vec3_t	start;
	vec3_t	forward, right, up;
	vec3_t	aim;
	vec3_t	dir;
	vec3_t	end;
	float	r, u;
	VectorCopy(self->enemy->s.origin, end);
	end[2] += self->enemy->viewheight;
	VectorSubtract(end, start, aim);
	vectoangles(aim, dir);
	AngleVectors(dir, forward, right, up);

	r = crandom() * 1000;
	u = crandom() * 500;
	VectorMA(start, 8192, forward, end);
	VectorMA(end, r, right, end);
	VectorMA(end, u, up, end);

	VectorSubtract(end, start, aim);
	VectorNormalize(aim);
	monster_fire_rocket(self, self->s.origin, aim, 5, 600, EF_ROCKET);
}
mframe_t aura_sphere_frames[] = {
	ai_charge, 10, NULL,
	ai_charge, 10, NULL,
	ai_charge, 10, NULL,
	ai_charge, 10, NULL,
	ai_charge, 10, auraSphere
};
mmove_t move_aura_sphere = { 0, 4, aura_sphere_frames, ai_stand };
void AuraSphere(edict_t* self) {
	int enemy = get_enemy(self);
	if (enemy == false) {
		gi.dprintf("no enemies\n");
		return;
	}
	self->monsterinfo.currentmove = &move_aura_sphere;
}


void Agility(edict_t* self) {
	self->blue = 1;
	self->speedMod += 0.5;
	self->speed *= 1.5;
}



mframe_t fly_frames[] = {
	ai_charge, 10, NULL,
	ai_charge, 10, NULL,
	ai_charge, 10, NULL,
	ai_charge, 10, NULL,
	ai_charge, 10, NULL
};
mmove_t move_fly = { 0, 4, fly_frames, Hover };
mmove_t move_stop = { 0, 0, fly_frames, ai_stand };
void Fly(edict_t* self) {
	self->inAir = 1;
	self->airTimer = 2000;
	self->monsterinfo.currentmove = &move_fly;
}
void Hover(edict_t* self) {
	self->monsterinfo.currentmove = &move_fly;
}
void Land(edict_t* self) { //Not a real move - helps fly
	self->inAir = 0;
	self->monsterinfo.currentmove = &move_stop;
	explode(self, 100, 200);
}


moves* getMoves(edict_t* self) {
	static moves moveList[4];
	if (strstr(self->classname, "soldier") != NULL) {
		moveList[0] = Tackle;
		moveList[1] = BulletSeed;
		moveList[2] = Harden;
		moveList[3] = Explosion;
		return moveList;
	}
	if (stricmp(self->classname, "monster_gladiator") == 0) {
		moveList[0] = Tackle;
		moveList[1] = Hyperbeam;
		moveList[2] = Harden;
		moveList[3] = BulkUp;
		return moveList;
	}
	if (stricmp(self->classname, "monster_brain") == 0) {
		moveList[0] = VineWhip;
		moveList[1] = FlashCannon;
		moveList[2] = Hypnosis;
		moveList[3] = Screech;
		return moveList;
	}
	if (stricmp(self->classname, "monster_medic") == 0) {
		moveList[0] = RevivalBlessing;
		moveList[1] = Rest;
		moveList[2] = Tackle;
		moveList[3] = HealPulse;
		return moveList;
	}
	if (stricmp(self->classname, "monster_tank") == 0) {
		moveList[0] = Rest;
		moveList[1] = Harden;
		moveList[2] = Tackle;
		moveList[3] = SludgeBomb;
		return moveList;
	}
	if (stricmp(self->classname, "monster_berserker") == 0) {
		moveList[0] = RageFist;
		moveList[1] = Tackle;
		moveList[2] = BulkUp;
		moveList[3] = Struggle;
		return moveList;
	}
	if (stricmp(self->classname, "monster_insane") == 0) {
		moveList[0] = Struggle;
		moveList[1] = Tackle;
		moveList[2] = ThunderWave;
		moveList[3] = Screech;
		return moveList;
	}
	if (stricmp(self->classname, "monster_parasite") == 0) {
		moveList[0] = DrainPunch;
		moveList[1] = HealPulse;
		moveList[2] = Tackle;
		moveList[3] = SludgeBomb;
		return moveList;
	}
	if (stricmp(self->classname, "monster_gunner") == 0) {
		moveList[0] = BulletSeed;
		moveList[1] = AuraSphere;
		moveList[2] = Agility;
		moveList[3] = BulkUp;
		return moveList;
	}
	if (stricmp(self->classname, "monster_chick") == 0) {
		moveList[0] = Explosion;
		moveList[1] = AuraSphere;
		moveList[2] = Tackle;
		moveList[3] = Fly;
		return moveList;
	}
	gi.dprintf("Something is trying to get moves that shouldn't be (or a classname is wrong)\n");
	return moveList;
}

char** getMovesName(edict_t* self) {
	static char* moveList[4];
	if (strstr(self->classname, "soldier") != NULL) {
		moveList[0] = "Tackle";
		moveList[1] = "Bullet Seed";
		moveList[2] = "Harden";
		moveList[3] = "Explosion";
		return moveList;
	}
	if (stricmp(self->classname, "monster_gladiator") == 0) {
		moveList[0] = "Tackle";
		moveList[1] = "Hyperbeam";
		moveList[2] = "Harden";
		moveList[3] = "BulkUp";
		return moveList;
	}
	if (stricmp(self->classname, "monster_brain") == 0) {
		moveList[0] = "Vine Whip";
		moveList[1] = "Flash Cannon";
		moveList[2] = "Hypnosis";
		moveList[3] = "Screech";
		return moveList;
	}
	if (stricmp(self->classname, "monster_medic") == 0) {
		moveList[0] = "Revival Blessing";
		moveList[1] = "Rest";
		moveList[2] = "Tackle";
		moveList[3] = "Heal Pulse";
		return moveList;
	}
	if (stricmp(self->classname, "monster_tank") == 0) {
		moveList[0] = "Rest";
		moveList[1] = "Harden";
		moveList[2] = "Tackle";
		moveList[3] = "Sludge Bomb";
		return moveList;
	}
	if (stricmp(self->classname, "monster_berserker") == 0) {
		moveList[0] = "Rage Fist";
		moveList[1] = "Tackle";
		moveList[2] = "Bulk Up";
		moveList[3] = "Struggle";
		return moveList;
	}
	if (stricmp(self->classname, "monster_insane") == 0) {
		moveList[0] = "Struggle";
		moveList[1] = "Tackle";
		moveList[2] = "Thunder Wave";
		moveList[3] = "Screech";
		return moveList;
	}
	if (stricmp(self->classname, "monster_parasite") == 0) {
		moveList[0] = "Drain Punch";
		moveList[1] = "Heal Pulse";
		moveList[2] = "Tackle";
		moveList[3] = "Sludge Bomb";
		return moveList;
	}
	if (stricmp(self->classname, "monster_gunner") == 0) {
		moveList[0] = "Bullet Seed";
		moveList[1] = "Aura phere";
		moveList[2] = "Agility";
		moveList[3] = "Bulk Up";
		return moveList;
	}
	if (stricmp(self->classname, "monster_chick") == 0) {
		moveList[0] = "Explosion";
		moveList[1] = "Aura Sphere";
		moveList[2] = "Tackle";
		moveList[3] = "Fly";
		return moveList;
	}
	gi.dprintf("Something is trying to get moves that shouldn't be (or a classname is wrong)\n");
	return moveList;
}