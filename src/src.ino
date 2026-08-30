#define __V3__

void myloop();
void mysetup();

#ifdef __CARRERA__
#include "Codigo_Carrera.h"
#endif

#ifdef __V1__
#include "v_1_seguidor.h"
#endif

#ifdef __V2__
#include "v_2_seguidor.h"
#endif 

#ifdef __V3__
#include "v_3_seguidor.h"
#endif 

#ifdef __TEST_P__
#include "test_protocol.h"
#endif

#ifdef __TEST_S__
#include "test_sensores.h"
#endif 

#ifdef __TEST_M__
#include "test_motores.h"
#endif

#ifdef __TEST_V__
#include "test_max_value.h"
#endif

void setup() {
  mysetup();
}

void loop() {
  myloop();
}
