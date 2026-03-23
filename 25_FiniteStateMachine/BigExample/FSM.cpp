//---------------- File: main.cpp (Finite State Machine Pattern):

#include "FSM.h"

void print(std::string_view event) {std::cout << event << ":\n";}

int main()
try
{
   FSM CD_Player("      CD Player", "Closed_without_CD");

   print("open");          CD_Player.open();
   print("insert_CD");     CD_Player.insert_CD(3);
   print("close");         CD_Player.close();
   print("play");          CD_Player.play();
   print("next_song");     CD_Player.next_song();
   print("play");          CD_Player.play();
   print("previous_song"); CD_Player.previous_song();
   print("pause");         CD_Player.pause();
   print("play");          CD_Player.play();
   print("next_song");     CD_Player.next_song();
   print("stop");          CD_Player.stop();
   print("open");          CD_Player.open();
   print("play");          CD_Player.play();
   print("open");          CD_Player.open();
   print("remove_CD");     CD_Player.remove_CD();
   print("close");         CD_Player.close();
   print("insert_CD");     CD_Player.insert_CD(5);
}
catch(std::exception& e) {std::cout << "      Error: " << e.what() << '\n';}
