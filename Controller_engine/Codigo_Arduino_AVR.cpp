// Definições dos pinos (conforme sua imagem)
const int IN1 = 2;  // PD2 (pino 4 físico)
const int IN2 = 3;   // PD3 (pino 5 físico)
const int ENA = 5;   // PD5 (pino 11 físico) - PWM
const int POT = A0;  // PC0 (pino 23 físico) - ADC0

// Parâmetros ajustáveis
const int MIN_PWM = 30;    // Valor mínimo onde o motor começa a girar
const int MAX_PWM = 255;   // Valor máximo de PWM
const int DEADZONE = 20;   // Zona morta no potenciômetro (0-1023)

void setup() {
  // Configura os pinos de controle
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);
  
  // Configura sentido único (horário)
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  
  // Inicializa comunicação serial para debug (opcional)
  Serial.begin(9600);
}

// Função para mapeamento proporcional com deadzone
int mapProportional(int value) {
  if(value < DEADZONE) return 0;
  value -= DEADZONE;
  int effectiveRange = 1023 - DEADZONE;
  long mapped = (long)value * (MAX_PWM - MIN_PWM) / effectiveRange + MIN_PWM;
  return constrain(mapped, 0, MAX_PWM);
}

void loop() {
  // Lê o valor do potenciômetro (0-1023)
  int potValue = analogRead(POT);
  
  // Converte para velocidade proporcional
  int motorSpeed = mapProportional(potValue);
  
  // Aplica a velocidade ao motor
  analogWrite(ENA, motorSpeed);
  
  // Debug (opcional)
  Serial.print("Pot: ");
  Serial.print(potValue);
  Serial.print(" -> PWM: ");
  Serial.println(motorSpeed);
  
  // Pequeno delay para estabilidade
  delay(20);
}