/**
 * ============================================================================
 * File: FSM.cpp (Modern Variant Implementation)
 * Author: Mario Galindo Queralt, Ph.D.
 * 
 * --- DESIGN OVERVIEW:
 * This file implements the FSM context logic and the main simulation loop.
 * It uses std::visit to dispatch events to the current state stored in 
 * the variant.
 * ============================================================================
 */

#include "FSM.h"

void print(std::string_view event) { std::cout << "\n" << event << ":\n"; }

//--------------------------------------------------------- FSM Constructor:
FSM::FSM(std::string name) : name{std::move(name)}
{
   std::get<Closed_without_CD>(state_).print_name();
}

//--------------------------------------------------------- FSM Dispatcher:
// We use [&] to capture any parameters (like 'n' in insert_CD) by reference.
#define DISPATCH(EVENT_CALL) \
   std::visit([&](auto& s) { \
      if constexpr (!std::is_same_v<std::decay_t<decltype(s)>, std::monostate>) s.EVENT_CALL; \
   }, state_);

void FSM::open()           { DISPATCH(open()); }
void FSM::close()          { DISPATCH(close()); }
void FSM::insert_CD(int n) { DISPATCH(insert_CD(n)); }
void FSM::remove_CD()      { DISPATCH(remove_CD()); }
void FSM::play()           { DISPATCH(play()); }
void FSM::stop()           { DISPATCH(stop()); }
void FSM::next_song()      { DISPATCH(next_song()); }
void FSM::previous_song()  { DISPATCH(previous_song()); }
void FSM::pause()          { DISPATCH(pause()); }

//--------------------------------------------------------- Main Simulation:
int main()
try
{
   std::cout << "=== CD PLAYER (VARIANT STATE MACHINE) ===\n" << std::endl;

   FSM CD_Player("    CD Player");

   print("open");          CD_Player.open();
   print("insert_CD");     CD_Player.insert_CD(3);
   print("close");         CD_Player.close();
   print("play");          CD_Player.play();
   print("next_song");     CD_Player.next_song();
   print("next_song");     CD_Player.next_song();
   print("play");          CD_Player.play();
   print("previous_song"); CD_Player.previous_song();
   print("pause");         CD_Player.pause();
   print("play");          CD_Player.play();
   print("next_song");     CD_Player.next_song();
   print("stop");          CD_Player.stop();
   print("stop");          CD_Player.stop();
   print("open");          CD_Player.open();
   print("open");          CD_Player.open();
   print("play");          CD_Player.play();
   print("play");          CD_Player.play();
   print("open");          CD_Player.open();
   print("remove_CD");     CD_Player.remove_CD();
   print("close");         CD_Player.close();
   print("insert_CD");     CD_Player.insert_CD(5);

   std::cout << "\n=== SIMULATION COMPLETED ===\n";
}
catch (char const* e) { std::cout << e; }
catch (const std::exception& e) { std::cout << e.what() << "\n"; }

//================================================================================ END
