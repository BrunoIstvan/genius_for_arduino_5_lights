#include <Arduino.h>
#include "genius.h"

#define LED_AZUL 2
#define LED_VERMELHO 3
#define LED_AMARELO 4
#define LED_VERDE 5
#define LED_ROXO 6

#define BUZZER 7

#define BOTAO_AZUL 8
#define BOTAO_VERMELHO 9
#define BOTAO_AMARELO 10
#define BOTAO_VERDE 11
#define BOTAO_ROXO 12

#define UM_SEGUNDO 1000
#define MEIO_SEGUNDO 350

#define INDEFINIDO -1
#define TAMANHO_SEQUENCIA 10

#define POTENCIOMETRO 0

int sequenciaLuzes[TAMANHO_SEQUENCIA];
int rodada = 0;
int passos_respondidos = 0;

enum ESTADOS
{
  PRONTO_PROXIMA_RODADA,
  USUARIO_RESPONDENDO,
  JOGO_FINALIZADO_SUCESSO,
  JOGO_FINALIZADO_FALHA
};

void setup()
{
  Serial.begin(9600);

  // inicializa portas de saída
  iniciaPortas();

  // inicializa botões de entrada
  iniciaBotoes();

  // inicializa o buzzer
  iniciaBuzzer();

  // inicializa os estados para iniciar o jogo
  iniciaJogo();
}

void loop()
{
  // recupera o estado atual do jogo
  int estado = estadoAtual();

  Serial.print("Estado atual: ");
  Serial.println(estado);

  switch (estado)
  {
  case PRONTO_PROXIMA_RODADA:
    Serial.println("Pronto para a proxima rodada");
    preparaNovaRodada();
    break;
  case USUARIO_RESPONDENDO:
    Serial.println("Jogador respondendo");
    processaRespostaJogador();
    break;
  case JOGO_FINALIZADO_SUCESSO:
    Serial.println("Jogo finalizado com sucesso");
    jogoFinalizadoSucesso();
    break;
  case JOGO_FINALIZADO_FALHA:
    Serial.println("Jogo finalizado com falha");
    jogoFinalizadoFalha();
    break;
  }

  delay(MEIO_SEGUNDO);
}

/**
 * Método que prepara o jogo para uma nova rodada
 **/
void preparaNovaRodada()
{
  rodada++;
  passos_respondidos = 0;

  // se o numero de rodadas é menor que o total de numeros da sequencia
  if (rodada <= TAMANHO_SEQUENCIA)
  {
    Serial.print("Rodada #");
    Serial.println(rodada);
    tocaLedsRodada();
  }
}

/**
 * Fica aguardando a jogada feita pelo jogador
 **/
void processaRespostaJogador()
{
  // resposta recuperada após usuário apertar o botão
  int resposta = leRespostaJogador();

  Serial.print("Botão apertado ");
  Serial.println(resposta);

  if (resposta == INDEFINIDO)
    return;

  if (resposta == sequenciaLuzes[passos_respondidos])
  {
    passos_respondidos++;
  }
  else
  {
    rodada = TAMANHO_SEQUENCIA + 2;
    Serial.println("Resposta errada");
  }
}

int estadoAtual()
{

  if (rodada <= TAMANHO_SEQUENCIA)
  {
    if (passos_respondidos == rodada)
    {
      return PRONTO_PROXIMA_RODADA;
    }
    return USUARIO_RESPONDENDO;
  }
  else if (rodada == TAMANHO_SEQUENCIA + 1)
  {
    return JOGO_FINALIZADO_SUCESSO;
  }
  return JOGO_FINALIZADO_FALHA;
}

/**
 * Toca todos leds na sequencia conforme a rodada atual
 **/
void tocaLedsRodada()
{
  for (int i = 0; i < rodada; i++)
  {
    piscaLED(sequenciaLuzes[i]);
  }
}

int leRespostaJogador()
{

  if (digitalRead(BOTAO_VERDE) == LOW)
  {
    return piscaLED(LED_VERDE);
  }
  else if (digitalRead(BOTAO_AMARELO) == LOW)
  {
    return piscaLED(LED_AMARELO);
  }
  else if (digitalRead(BOTAO_VERMELHO) == LOW)
  {
    return piscaLED(LED_VERMELHO);
  }
  else if (digitalRead(BOTAO_AZUL) == LOW)
  {
    return piscaLED(LED_AZUL);
  }
  else if (digitalRead(BOTAO_ROXO) == LOW)
  {
    return piscaLED(LED_ROXO);
  }
  else
  {
    return INDEFINIDO;
  }
}

void iniciaBuzzer()
{
  pinMode(BUZZER, OUTPUT);
}

/**
 * Inicializa os botões de entrada
 **/
void iniciaBotoes()
{
  pinMode(BOTAO_VERDE, INPUT_PULLUP);
  pinMode(BOTAO_AMARELO, INPUT_PULLUP);
  pinMode(BOTAO_VERMELHO, INPUT_PULLUP);
  pinMode(BOTAO_AZUL, INPUT_PULLUP);
  pinMode(BOTAO_ROXO, INPUT_PULLUP);
}

int sorteiaNumero()
{
  // sorteia um numero entre 2 e 5ou6
  return random(LED_AZUL, LED_ROXO + 1); // 2-5
}

void iniciaJogo()
{
  // le o valor da entrada 0 que varia conforme resistência do ar
  int jogo = analogRead(POTENCIOMETRO);
  Serial.println(jogo);
  // informa valor randomico para a semente do random
  randomSeed(jogo);

  // para todos os numeros da sequencial
  for (int i = 0; i < TAMANHO_SEQUENCIA; i++)
  {
    // será sorteado um numero para cada posição das sequencias
    sequenciaLuzes[i] = sorteiaNumero();
  }
}

/**
 * Inicializa as portas de saída 
 **/
void iniciaPortas()
{
  pinMode(LED_AMARELO, OUTPUT);
  pinMode(LED_AZUL, OUTPUT);
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_VERMELHO, OUTPUT);
  pinMode(LED_ROXO, OUTPUT);
}

int piscaLED(int portaLED)
{
  verificaSomDoLed(portaLED);
  digitalWrite(portaLED, HIGH);
  delay(MEIO_SEGUNDO); // Wait for 500 millisecond(s)
  digitalWrite(portaLED, LOW);
  delay(MEIO_SEGUNDO); // Wait for 1000 millisecond(s)
  return portaLED;
}

void jogoFinalizadoSucesso()
{
  piscaLED(LED_ROXO);
  piscaLED(LED_VERDE);
  piscaLED(LED_AMARELO);
  piscaLED(LED_VERMELHO);
  piscaLED(LED_AZUL);
}

void jogoFinalizadoFalha()
{
  Serial.println("Voce Perdeu");
  tocaSom(300);

  digitalWrite(LED_VERDE, HIGH);
  digitalWrite(LED_AMARELO, HIGH);
  digitalWrite(LED_VERMELHO, HIGH);
  digitalWrite(LED_AZUL, HIGH);
  digitalWrite(LED_ROXO, HIGH);
  delay(UM_SEGUNDO);
  digitalWrite(LED_VERDE, LOW);
  digitalWrite(LED_AMARELO, LOW);
  digitalWrite(LED_VERMELHO, LOW);
  digitalWrite(LED_AZUL, LOW);
  digitalWrite(LED_ROXO, LOW);
  delay(MEIO_SEGUNDO);
}

void tocaSom(int frequencia)
{
  tone(BUZZER, frequencia, 100);
}

void verificaSomDoLed(int portaLed)
{
  switch (portaLed)
  {
  case LED_ROXO:
    tocaSom(1000);
    break;
  case LED_VERDE:
    tocaSom(2000);
    break;
  case LED_AMARELO:
    tocaSom(3000);
    break;
  case LED_VERMELHO:
    tocaSom(4000);
    break;
  case LED_AZUL:
    tocaSom(5000);
    break;
  }
}