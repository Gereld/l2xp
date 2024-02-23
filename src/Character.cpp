#include "Character.h"

Character::Character() {
	m_name; 
	m_last_target_id = 0;

	m_combat_mode = false;

	reset_all();
}

void Character::set_name( const std::string & name ) {
	if( name != m_name ) {
		m_name = name;
		reset_xp();
		reset_sp();
		reset_adena();
	}
}

void Character::add_xp( __int64 xp ) {
	start_timer();

	m_xp += xp;

	update_timer();
}

void Character::add_sp( int sp ) {
	start_timer();

	m_sp += sp;

	update_timer();
}

void Character::add_adena( __int64 adena ) {
	start_timer();

	m_adena += adena;

	update_timer();
}

const std::string & Character::get_name() const {
	return m_name;
}

__int64 Character::get_xp() const {
	return m_xp;
}

int Character::get_sp() const {
	return m_sp;
}

__int64 Character::get_adena() const {
	return m_adena;
}

void Character::reset_xp() {
	m_xp = 0;
}

void Character::reset_sp() {
	m_sp = 0;
}

void Character::reset_adena() {
	m_adena = 0;
}

void Character::reset_damage() {
	m_damage_type = DMG_NORMAL;

	m_hit_count = 0;
	m_damage = 0;

	m_pcrit_count = 0;
	m_pcrit_damage = 0;

	m_mcrit_count = 0;
	m_mcrit_damage = 0;
}

void Character::reset_all() {
	reset_xp();
	reset_sp();
	reset_adena();
	reset_damage();

	m_total_time = 0;
	m_timer_started = false;

	m_total_combat_time = 0;
	m_combat_timer_started = false;
}

double Character::get_xp_per_hour() const {
	if( get_elapsed_hours() == 0 ) {
		return 0.0;
	}
	return double( m_xp ) / get_elapsed_hours(); 
}

double Character::get_sp_per_hour() const {
	if( get_elapsed_hours() == 0 ) {
		return 0.0;
	}
	return double( m_sp ) / get_elapsed_hours(); 
}

double Character::get_adena_per_hour() const {
	if( get_elapsed_hours() == 0 ) {
		return 0.0;
	}
	return double( m_adena ) / get_elapsed_hours(); 
}

void Character::set_last_target_id( unsigned int id ) {
	m_last_target_id = id;
}

unsigned int Character::get_last_target_id() {
	return m_last_target_id;
}

void Character::add_damage( __int64 damage ) {
	start_timer();
	start_combat_timer();

	m_damage += damage;
	m_hit_count++;

	if( m_damage_type == DMG_PHYSICAL_CRIT ) {
		m_pcrit_count++;
		m_pcrit_damage += damage;
	}
	else if( m_damage_type == DMG_MAGICAL_CRIT ) {
		m_mcrit_count++;
		m_mcrit_damage += damage;
	}
	m_damage_type = DMG_NORMAL;

	update_timer();
	update_combat_timer();
}

void Character::set_damage_type( DamageType type ) {
	m_damage_type = type;
}

unsigned int Character::get_hit_count() const {
	return m_hit_count;
}

double Character::get_average_damage() const {
	if( m_hit_count == 0 ) {
		return 0.0;
	}
	return double( m_damage ) / double( m_hit_count );
}

double Character::get_average_normal_damage() const {
	if( m_hit_count == 0 ) {
		return 0.0;
	}
	return double( m_damage - m_pcrit_damage - m_mcrit_damage ) / double( m_hit_count - m_pcrit_count - m_mcrit_count );
}

double Character::get_damage_per_second() const {
	if( get_elapsed_time() == 0 ) {
		return 0.0;
	}
	return double( m_damage ) / get_elapsed_time(); 
}

unsigned int Character::get_pcrit_count() const {
	return m_pcrit_count;
}

double Character::get_pcrit_rate() const {
	if( m_hit_count == 0 ) {
		return 0.0;
	}
	return double( m_pcrit_count ) / double( m_hit_count );
}

double Character::get_pcrit_damage() const {
	if( m_pcrit_count == 0 ) {
		return 0.0;
	}
	return double( m_pcrit_damage ) / double( m_pcrit_count );
}

double Character::get_pcrit_multiplier() const {
	double damage = get_average_normal_damage();
	if( damage == 0 ) {
		return 0.0;
	}
	return get_pcrit_damage() / damage;
}

unsigned int Character::get_mcrit_count() const {
	return m_mcrit_count;
}

double Character::get_mcrit_rate() const {
	if( m_hit_count == 0 ) {
		return 0.0;
	}
	return double( m_mcrit_count ) / double( m_hit_count );
}

double Character::get_mcrit_damage() const {
	if( m_mcrit_count == 0 ) {
		return 0.0;
	}
	return double( m_mcrit_damage ) / double( m_mcrit_count );
}

double Character::get_mcrit_multiplier() const {
	double damage = get_average_normal_damage();
	if( damage == 0 ) {
		return 0.0;
	}
	return get_mcrit_damage() / damage;
}

double Character::get_elapsed_time() const {
	return m_combat_mode ? m_total_combat_time : m_total_time;
}

double Character::get_elapsed_hours() const {
	return get_elapsed_time() / (60 * 60);
}

void Character::start_timer() {
	if( !m_timer_started ) {
		m_timer.restart();
		m_timer_started = true;
	}
}

void Character::update_timer() {
	m_total_time = m_timer.elapsed();
}

void Character::start_combat_timer() {
	if( !m_combat_timer_started ) {
		m_combat_timer.restart();
		m_combat_timer_started = true;
		m_combat_time = 0;
	}
}

void Character::update_combat_timer() {
	m_total_combat_time -= m_combat_time;
	m_combat_time = m_combat_timer.elapsed();
	m_total_combat_time += m_combat_time;
}

void Character::stop_combat_timer() {
	m_combat_timer_started = false;
	m_combat_time = 0;
}

void Character::stop_combat_mode() {
	stop_combat_timer();
}

void Character::set_combat_mode( bool combat_mode ) {
	m_combat_mode = combat_mode;
}

bool Character::get_combat_mode() const {
	return m_combat_mode;
}

