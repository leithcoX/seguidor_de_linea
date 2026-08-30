#ifndef __BL_PROTOCOL_H__
#define __BL_PROTOCOL_H__

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    MODE_LINE_FOLLOWER = 0,  // PID automatico
    MODE_MANUAL = 1          // controlado desde el celular
} mode_t;

typedef enum { COEF_KP = 0, COEF_KI, COEF_KD } pid_coef_t;

typedef enum {
    CMD_NONE = 0,   // linea vacia / todavia no hay comando completo
    CMD_SET_MODE,   // "M0" -> line follower | "M1" -> manual
    CMD_SET_SPEED,  // "V<int16>,<int16>"  -> velocidad izq,der (con signo)
    CMD_SET_COEF,   // "P<float>" / "I<float>" / "D<float>"
    CMD_GET_DATA,   // "G"  -> pide una lectura puntual de sensores/velocidad
    CMD_SET_RATE,   // "R<uint16>" -> tasa de streaming en ms (0 = detener)
    CMD_INVALID     // linea recibida pero con formato invalido
} cmd_type_t;

#define PROTOCOL_SPEED_MIN -255
#define PROTOCOL_SPEED_MAX 255

#define PROTOCOL_MAX_LINE 32

typedef struct {
    cmd_type_t type;
    mode_t mode;          // valido si type == CMD_SET_MODE
    int16_t speed_left;   // valido si type == CMD_SET_SPEED (-255..255)
    int16_t speed_right;  // valido si type == CMD_SET_SPEED (-255..255)
    pid_coef_t coef;      // valido si type == CMD_SET_COEF
    float value;          // valido si type == CMD_SET_COEF
    uint16_t rate_ms;     // valido si type == CMD_SET_RATE
} command_t;

void protocol_init(void);

/*
 * Alimenta el parser byte a byte, tal como van llegando del Serial/HM10.
 * Devuelve true cuando se completo una linea (delimitada por '\n') y
 * el resultado del parseo queda en *out_cmd.
 *
 * Uso tipico en loop():
 *
 *   while (Serial.available()) {
 *       command_t cmd;
 *       if (protocol_feed((char)Serial.read(), &cmd)) {
 *           procesar_comando(&cmd);
 *       }
 *   }
 */
bool protocol_feed(char c, command_t* out_cmd);

/*  Parsea una linea ya completa (sin '\n' ni '\r' al final) directamente.
 *  Util para testear el parser sin pasar por protocol_feed.
 */
cmd_type_t protocol_parse_line(const char* line, command_t* out_cmd);

#endif /* __BL_PROTOCOL_H__ */
