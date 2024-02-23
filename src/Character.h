#pragma once

#include <boost/timer.hpp>

enum DamageType {
	DMG_NORMAL,
	DMG_PHYSICAL_CRIT,
	DMG_MAGICAL_CRIT,
};

class Character {
public:
	Character();

	void set_name( const std::string & name );
	void add_xp( __int64 xp );
	void add_sp( int sp );
	void add_adena( __int64 adena );
	void set_last_target_id( unsigned int id );
	void add_damage( __int64 damage );
	void set_damage_type( DamageType type );

	const std::string & get_name() const;
	__int64 get_xp() const;
	int get_sp() const;
	__int64 get_adena() const;

	void reset_xp();
	void reset_sp();
	void reset_adena();
	void reset_damage();
	void reset_all();

	double get_xp_per_hour() const;
	double get_sp_per_hour() const;
	double get_adena_per_hour() const;
	unsigned int get_last_target_id();
	unsigned int get_hit_count() const;

	double get_average_damage() const;
	double get_average_normal_damage() const;
	double get_damage_per_second() const;

	unsigned int get_pcrit_count() const;
	double get_pcrit_rate() const;
	double get_pcrit_damage() const;
	double get_pcrit_multiplier() const;

	unsigned int get_mcrit_count() const;
	double get_mcrit_rate() const;
	double get_mcrit_damage() const;
	double get_mcrit_multiplier() const;

	void stop_combat_mode();
	void set_combat_mode( bool combat_mode );
	bool get_combat_mode() const;

protected:
	std::string m_name; 
	__int64 m_xp;
	int m_sp;
	__int64 m_adena;
	unsigned int m_last_target_id;

	bool m_combat_mode;

	boost::timer m_timer;
	double m_total_time;
	bool m_timer_started;
	
	boost::timer m_combat_timer;
	double m_combat_time;
	double m_total_combat_time;
	bool m_combat_timer_started;

	DamageType m_damage_type;

	unsigned int m_hit_count;
	__int64 m_damage;

	unsigned int m_pcrit_count;
	__int64 m_pcrit_damage;

	unsigned int m_mcrit_count;
	__int64 m_mcrit_damage;

	double get_elapsed_time() const;
	double get_elapsed_hours() const;
	void start_timer();
	void update_timer();

	void start_combat_timer();
	void update_combat_timer();
	void stop_combat_timer();
};