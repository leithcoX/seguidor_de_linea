#include <stdlib.h>  // atof, strtol
#include <string.h>  // strlen, strchr

#include "protocol.h"

// -----------------------------------------------------------------------
// Estado interno del buffer de linea
// -----------------------------------------------------------------------
static char s_buffer[PROTOCOL_MAX_LINE + 1];
static uint8_t s_index;

void protocol_init(void) {
    s_index = 0;
    s_buffer[0] = '\0';
}

// -----------------------------------------------------------------------
// Helper: clampea un long a int16_t dentro del rango de velocidad valido
// -----------------------------------------------------------------------
static int16_t clamp_speed(long v) {
    if (v < PROTOCOL_SPEED_MIN) return PROTOCOL_SPEED_MIN;
    if (v > PROTOCOL_SPEED_MAX) return PROTOCOL_SPEED_MAX;
    return (int16_t)v;
}

// -----------------------------------------------------------------------
// Parsea una linea completa (ya sin '\n'/'\r') segun el protocolo:
//
//   M0 / M1            -> modo
//   V<int16>,<int16>   -> velocidad izq,der (con signo = direccion)
//   P<float>           -> nuevo Kp
//   I<float>           -> nuevo Ki
//   D<float>           -> nuevo Kd
//   G                  -> pedir datos puntuales
//   R<uint16>          -> tasa de streaming en ms (0 = detener)
// -----------------------------------------------------------------------
cmd_type_t protocol_parse_line(const char* line, command_t* out_cmd) {
    out_cmd->type = CMD_INVALID;

    size_t len = strlen(line);
    if (len == 0) {
        out_cmd->type = CMD_NONE;
        return out_cmd->type;
    }

    char head = line[0];
    const char* payload = line + 1;  // resto de la linea despues de la letra

    switch (head) {
        case 'M': {  // ---- Modo ----
            if (len < 2) break;
            if (payload[0] == '0') {
                out_cmd->mode = MODE_LINE_FOLLOWER;
            } else if (payload[0] == '1') {
                out_cmd->mode = MODE_MANUAL;
            } else {
                break;
            }
            out_cmd->type = CMD_SET_MODE;
            break;
        }

        case 'V': {  // ---- Velocidad izq,der ----
            if (len < 3) break;

            char* comma = strchr(payload, ',');
            if (comma == NULL) break;

            char* endptr_left = NULL;
            char* endptr_right = NULL;

            long left = strtol(payload, &endptr_left, 10);
            if (endptr_left != comma)
                break;  // no consumio hasta la coma -> formato invalido

            long right = strtol(comma + 1, &endptr_right, 10);
            if (endptr_right == comma + 1)
                break;  // no se pudo parsear el segundo numero

            out_cmd->speed_left = clamp_speed(left);
            out_cmd->speed_right = clamp_speed(right);
            out_cmd->type = CMD_SET_SPEED;
            break;
        }

        case 'P':    // ---- Kp ----
        case 'I':    // ---- Ki ----
        case 'D': {  // ---- Kd ----
            if (len < 2) break;

            out_cmd->coef = (head == 'P')   ? COEF_KP
                            : (head == 'I') ? COEF_KI
                                            : COEF_KD;
            out_cmd->value = (float)atof(
                payload);  // atof: seguro en AVR, a diferencia de sscanf("%f")
            out_cmd->type = CMD_SET_COEF;
            break;
        }

        case 'G': {  // ---- Pedido de datos puntual ----
            out_cmd->type = CMD_GET_DATA;
            break;
        }

        case 'R': {  // ---- Tasa de streaming ----
            if (len < 2) break;

            char* endptr = NULL;
            long rate = strtol(payload, &endptr, 10);
            if (endptr == payload) break;  // nada numerico
            if (rate < 0) rate = 0;

            out_cmd->rate_ms = (uint16_t)rate;
            out_cmd->type = CMD_SET_RATE;
            break;
        }

        default:
            // header desconocido -> CMD_INVALID (ya seteado arriba)
            break;
    }

    return out_cmd->type;
}

// -----------------------------------------------------------------------
// Alimenta el parser byte a byte. Ignora '\r'. Cuando llega '\n' cierra
// la linea, la parsea y resetea el buffer para la proxima.
// -----------------------------------------------------------------------
bool protocol_feed(char c, command_t* out_cmd) {
    if (c == '\r') {
        return false;  // ignorar CR, esperamos LF
    }

    if (c == '\n') {
        s_buffer[s_index] = '\0';
        s_index = 0;  // resetear para la proxima linea

        if (s_buffer[0] == '\0') {
            return false;  // linea vacia, no generamos comando
        }

        protocol_parse_line(s_buffer, out_cmd);
        return true;
    }

    if (s_index < PROTOCOL_MAX_LINE) {
        s_buffer[s_index++] = c;
    }
    // si se llena el buffer sin encontrar '\n', se descartan bytes extra
    // hasta que llegue el '\n' (evita overflow con basura/ruido en el enlace)

    return false;
}
