#pragma once

#include "Measurement.h"

// memory map definition
// ! 5 ! 4 ! 3 ! 2 ! 1 ! 0 ! 9 ! 8 ! 7 ! 6 ! 5 ! 4 ! 3 ! 2 ! 1 ! 0 !
// !---------------------------------------------------------------!
// ! Num starts                                                    !
// !---------------------------------------------------------------!
// ! Num unhandled interrupts                                      !
// !---------------------------------------------------------------!
// ! System flags                                                  !
// !---------------------------------------------------------------!
// ! Retention                                                     !
// ! 1 - Save retained vars (clear on operate)                     !
// ! 2 - Load retained vars (clear on operate)                     !
// !---------------------------------------------------------------!

class SystemVars : public Measurement
{
  public:
	SystemVars();
	void tick();

  private:

};
