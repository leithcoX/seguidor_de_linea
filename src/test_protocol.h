#include "bl_protocol.h"

// -----------------------------------------------------------------------
// Test del protocolo usando el Serial Monitor en vez del HM10.
//
// El HM10 es un puente UART <-> BLE transparente: todo lo que escribas
// en el Serial Monitor le llega al Arduino exactamente igual que si
// viniera del modulo, asi que este test sirve para validar el parser
// sin tener el HM10 conectado.
//
// IMPORTANTE en el Serial Monitor:
//   - Baudrate: 9600 (o el que hayas puesto en Serial.begin)
//   - Terminador de linea: "Newline" o "Both NL & CR" (protocol_feed
//     ignora el '\r', asi que ambas opciones funcionan)
//
// Comandos para probar (uno por linea, Enter para enviar):
//   M0            -> modo line follower
//   M1            -> modo manual
//   V120,-80      -> velocidad izq=120 (adelante) der=-80 (reversa)
//   V999,0        -> prueba de clamping (se recorta a 255)
//   P2.5          -> Kp = 2.5
//   I0.01         -> Ki = 0.01
//   D0.8          -> Kd = 0.8
//   G             -> pedido de datos puntual
//   R200          -> tasa de streaming = 200ms
//   R0            -> apagar streaming
//   asdf          -> comando invalido (para probar el manejo de errores)
// -----------------------------------------------------------------------

static void printCommand(const command_t& cmd) {
    switch (cmd.type) {
        case CMD_SET_MODE:
            Serial.print(F("[MODO] "));
            Serial.println(cmd.mode == MODE_MANUAL ? F("MANUAL") : F("LINE_FOLLOWER"));
            break;

        case CMD_SET_SPEED:
            Serial.print(F("[VELOCIDAD] izq="));
            Serial.print(cmd.speed_left);
            Serial.print(F(" der="));
            Serial.println(cmd.speed_right);
            break;

        case CMD_SET_COEF: {
            const char* name = (cmd.coef == COEF_KP) ? "Kp" :
                                (cmd.coef == COEF_KI) ? "Ki" : "Kd";
            Serial.print(F("[COEFICIENTE] "));
            Serial.print(name);
            Serial.print(F(" = "));
            Serial.println(cmd.value, 4);
            break;
        }

        case CMD_GET_DATA:
            Serial.println(F("[PEDIDO DE DATOS] (G)"));
            break;

        case CMD_SET_RATE:
            Serial.print(F("[TASA STREAMING] "));
            Serial.print(cmd.rate_ms);
            Serial.println(F(" ms"));
            break;

        case CMD_INVALID:
            Serial.println(F("[ERROR] comando invalido"));
            break;

        case CMD_NONE:
            // linea vacia, no hacemos nada
            break;
    }
}

void mysetup() {
    Serial.begin(9600);
    protocol_init();
    Serial.println(F("=== Test de protocolo listo. Escribi un comando y Enter. ==="));
}

void myloop() {
    while (Serial.available()) {
        command_t cmd;
        if (protocol_feed((char)Serial.read(), &cmd)) {
            printCommand(cmd);
        }
    }
}
