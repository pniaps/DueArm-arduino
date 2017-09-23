#include <AccelStepper.h>         //Control de motores paso a paso
#include <MultiStepper.h>
#include <Servo.h>

int nPositions = 0;
int currentPosition = -1;


#include <Encoder.h>
Encoder encoderBase(45, 47);
Encoder encoderBrazo(39, 37);
Encoder encoderAntebrazo(41, 43);

AccelStepper baseStepper(AccelStepper::DRIVER, 60, 61); //Step, Dir, En 56
AccelStepper brazoStepper(AccelStepper::DRIVER, 54, 55); //Step, Dir, En 38
AccelStepper antebrazoStepper(AccelStepper::DRIVER, 46, 48); //Step, Dir, En 62
MultiStepper steppers;

int brazoSpeed = 1000;
char datoSerie = 0;
bool activado = false;
Servo servoMuneca;

#define BT Serial3

void setup() {
  Serial.begin(115200);
  BT.begin(9600);
  Serial.println("Init ...");
  delay(1000);

  //  ATBT("AT");
  //  ATBT("AT+NAMEDueArm");
  //  ATBT("AT+VERSION");
  //  ATBT("AT+PIN8888");

  Serial.println("Initialized.");


  brazoStepper.setEnablePin(38);
  brazoStepper.setPinsInverted(false, false, true);
  brazoStepper.setMaxSpeed(1000);
  brazoStepper.disableOutputs();

  antebrazoStepper.setEnablePin(62);
  antebrazoStepper.setPinsInverted(false, false, true);
  antebrazoStepper.setMaxSpeed(1000);
  antebrazoStepper.disableOutputs();

  baseStepper.setEnablePin(56);
  baseStepper.setPinsInverted(false, false, true);
  baseStepper.setMaxSpeed(1000);
  baseStepper.disableOutputs();

  steppers.addStepper(brazoStepper);
  steppers.addStepper(antebrazoStepper);
  steppers.addStepper(baseStepper);

  servoMuneca.attach(11);
  servoMuneca.write(90);

}

void loop() {
  while (Serial.available() > 0) {
    datoSerie = Serial.read();
    procesaComando(datoSerie);
  }

  while (BT.available() > 0) {
    datoSerie = BT.read();
    procesaComando(datoSerie);
    Serial.write(datoSerie);
  }

  if (steppers.run() == 0) {
    if (activado) {
      Serial.println("Se desactivan drivers");
      activado = false;
      status();
    }
    if (!nextPosition()) {
      brazoStepper.disableOutputs();
      antebrazoStepper.disableOutputs();
      baseStepper.disableOutputs();
    }
  }

}

void mueveStepper(AccelStepper *stepper, int position, int speed) {
  Serial.print("Iniciamos movimiento ");
  if (stepper == &baseStepper) {
    Serial.print("base ");
  } else if (stepper == &brazoStepper) {
    Serial.print("brazo ");
  } else if (stepper == &antebrazoStepper) {
    Serial.print("antebrazo ");
  }
  Serial.println(position);
  activado = true;
  stepper->enableOutputs();
  stepper->moveTo(position);
  stepper->setSpeed(speed);
  stepper->runSpeedToPosition();
}

void procesaComando(char datoSerie) {
  if (datoSerie == '1') {
    mueveStepper(&baseStepper, baseStepper.targetPosition() + 600, brazoSpeed);
  } else if (datoSerie == '2') {
    mueveStepper(&baseStepper, baseStepper.targetPosition() - 600, brazoSpeed);
  } else if (datoSerie == 'y') {
    mueveStepper(&baseStepper, 14000, brazoSpeed);
  } else if (datoSerie == 'u') {
    mueveStepper(&baseStepper, -14000, brazoSpeed);
  } else if (datoSerie == '3') {
    baseStepper.stop();
    baseStepper.disableOutputs();
  } else if (datoSerie == '4') {
    mueveStepper(&brazoStepper, brazoStepper.targetPosition() + 600, brazoSpeed);
  } else if (datoSerie == '5') {
    mueveStepper(&brazoStepper, brazoStepper.targetPosition() - 600, brazoSpeed);
  } else if (datoSerie == 'h') {
    mueveStepper(&brazoStepper, 14000, brazoSpeed);
  } else if (datoSerie == 'j') {
    mueveStepper(&brazoStepper, -14000, brazoSpeed);
  } else if (datoSerie == '6') {
    brazoStepper.stop();
    brazoStepper.disableOutputs();
  } else if (datoSerie == '7') {
    mueveStepper(&antebrazoStepper, antebrazoStepper.targetPosition() - 600, brazoSpeed);
  } else if (datoSerie == '8') {
    mueveStepper(&antebrazoStepper, antebrazoStepper.targetPosition() + 600, brazoSpeed);
  } else if (datoSerie == 'n') {
    mueveStepper(&antebrazoStepper, -14000, brazoSpeed);
  } else if (datoSerie == 'm') {
    mueveStepper(&antebrazoStepper, 14000, brazoSpeed);
  } else if (datoSerie == '9') {
    antebrazoStepper.stop();
    antebrazoStepper.disableOutputs();
  } else if (datoSerie == '0') {
    brazoStepper.disableOutputs();
    antebrazoStepper.disableOutputs();
    baseStepper.disableOutputs();
  } else if (datoSerie == 'a') {
    servoMuneca.write(45);
  } else if (datoSerie == 'b') {
    servoMuneca.write(135);
  } else if (datoSerie == 'r') {
    reset();
  } else if (datoSerie == 's') {
    status();
  } else if (datoSerie == 'g') {
    savePosition();
  } else if (datoSerie == 'd') {
    delPosition();
  } else if (datoSerie == 'p') {
    deletePositions();
    status();
  } else if (datoSerie == 't') {
    muestraPosiciones();
  } else if (datoSerie == 'i') {
    goToPosition(0);
  }
}

void ATBT(char *str) {
  Serial.print("===> ");
  Serial.println(str);
  BT.print(str);
  //  BT.print("\r\n");
  BT.flush();
  delay(1000);
  Serial.print("<=== ");
  while (BT.available() > 0) {
    Serial.write(BT.read());
  }
  //  Serial.println(".");
  Serial.println();
  delay(1000);
}

void reset()
{
  brazoStepper.setCurrentPosition(0);
  antebrazoStepper.setCurrentPosition(0);
  baseStepper.setCurrentPosition(0);
  encoderBase.write(0);
  encoderBrazo.write(0);
  encoderAntebrazo.write(0);
  Serial.println("Reset =========================");
}

void status()
{
  char body[255] = {0};
  sprintf(body, "Base: %05d-%05d Brazo: %05d-%05d AnteBrazo: %05d-%05d Posiciones: %2d Actual: %2d",
          encoderBase.read(),
          baseStepper.currentPosition(),
          encoderBrazo.read(),
          brazoStepper.currentPosition(),
          encoderAntebrazo.read(),
          antebrazoStepper.currentPosition(),
          nPositions,
          currentPosition);
  Serial.println(body);
}
