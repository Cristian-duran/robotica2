#include <Pixy2.h>

// Definición de pines para el sensor ultrasónico
#define TRIG_PIN 7
#define ECHO_PIN 6

// Definición de pines para el puente H
#define MOTOR_A1 4
#define MOTOR_A2 5
#define MOTOR_B1 8
#define MOTOR_B2 9
#define MOTOR_SPEED 3 // Pin PWM para controlar la velocidad

Pixy2 pixy;

int centerX = 158;  // Centro de la cámara (Pixy tiene resolución de 316x208)
int velocidad = 70; // Velocidad del carrito (0-255)
int umbralGiro = 30; // Margen de tolerancia para el centrado del objeto
int distanciaMinima = 10; // Distancia mínima para detenerse (en cm)

void setup() {
    Serial.begin(115200);
    pixy.init();

    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    pinMode(MOTOR_A1, OUTPUT);
    pinMode(MOTOR_A2, OUTPUT);
    pinMode(MOTOR_B1, OUTPUT);
    pinMode(MOTOR_B2, OUTPUT);
    pinMode(MOTOR_SPEED, OUTPUT);
    
    analogWrite(MOTOR_SPEED, velocidad); // Establece velocidad inicial
}

// Función para medir la distancia con el sensor ultrasónico
float medirDistancia() {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    
    long duration = pulseIn(ECHO_PIN, HIGH);
    float distance = duration * 0.034 / 2;
    return distance;
}

// Función para mover el carrito hacia adelante
void avanzar() {
    analogWrite(MOTOR_SPEED, velocidad);
    digitalWrite(MOTOR_A1, HIGH);
    digitalWrite(MOTOR_A2, LOW);
    digitalWrite(MOTOR_B1, LOW);
    digitalWrite(MOTOR_B2, HIGH);
}

// Función para girar a la izquierda
void girarIzquierda() {
    analogWrite(MOTOR_SPEED, velocidad);
    digitalWrite(MOTOR_A1, HIGH);
    digitalWrite(MOTOR_A2, LOW);
    digitalWrite(MOTOR_B1, HIGH);
    digitalWrite(MOTOR_B2, LOW);
}

// Función para girar a la derecha
void girarDerecha() {
    analogWrite(MOTOR_SPEED, velocidad);
    digitalWrite(MOTOR_A1, LOW);
    digitalWrite(MOTOR_A2, HIGH);
    digitalWrite(MOTOR_B1, LOW);
    digitalWrite(MOTOR_B2, HIGH);
}

// Función para detener el carrito
void detener() {
    digitalWrite(MOTOR_A1, LOW);
    digitalWrite(MOTOR_A2, LOW);
    digitalWrite(MOTOR_B1, LOW);
    digitalWrite(MOTOR_B2, LOW);
}

void loop() {
    float distancia = medirDistancia();
    Serial.print("Distancia: ");
    Serial.println(distancia);

    // Si hay un obstáculo cerca, detenerse
    if (distancia < distanciaMinima) {
        Serial.println("¡Obstáculo detectado! Deteniéndose...");
        detener();
        delay(100);
        return; // No sigue procesando el resto del código hasta la próxima iteración
    }
    
    pixy.ccc.getBlocks(); // Obtiene los bloques detectados
    
    if (pixy.ccc.numBlocks) { // Si detecta un color
        int objectX = pixy.ccc.blocks[0].m_x; // Posición X del objeto
        
        Serial.print("Objeto detectado en X: ");
        Serial.println(objectX);

        // Movimiento del carrito basado en la posición X del objeto
        if (objectX < centerX - umbralGiro) {
            girarIzquierda(); // Girar a la izquierda si el objeto está a la izquierda
        } 
        else if (objectX > centerX + umbralGiro) {
            girarDerecha(); // Girar a la derecha si el objeto está a la derecha
        } 
        else {
            avanzar(); // Si el objeto está centrado, avanzar
        }
    } else {
        detener(); // Si no detecta el objeto, se detiene
    }
    
    delay(50); // Pequeña pausa para evitar movimientos bruscos
}
