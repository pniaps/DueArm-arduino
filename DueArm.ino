#include <AccelStepper.h>         //Control de motores paso a paso
#include <MultiStepper.h>
#include <Servo.h>

AccelStepper brazoStepper(AccelStepper::DRIVER, 54, 55); //Step, Dir, En 38
AccelStepper antebrazoStepper(AccelStepper::DRIVER, 46, 48); //Step, Dir, En 62
AccelStepper baseStepper(AccelStepper::DRIVER, 60, 61); //Step, Dir, En 56
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

  ATBT("AT");
  ATBT("AT+NAMEDueArm");
  ATBT("AT+VERSION");
  ATBT("AT+PIN8888");

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
    }
    brazoStepper.disableOutputs();
    antebrazoStepper.disableOutputs();
    baseStepper.disableOutputs();
  }

}

void mueveStepper(AccelStepper *stepper, int steps, int speed) {
  Serial.print("Iniciamos movimiento ");
  Serial.println(steps);
  activado = true;
  stepper->enableOutputs();
  stepper->moveTo(stepper->targetPosition() + steps);
  stepper->setSpeed(speed);
  stepper->runSpeedToPosition();
}

void procesaComando(char datoSerie) {
  if (datoSerie == '1') {
    mueveStepper(&baseStepper, 600, brazoSpeed);
  } else if (datoSerie == '2') {
    mueveStepper(&baseStepper, -600, brazoSpeed);
  } else if (datoSerie == '4') {
    mueveStepper(&brazoStepper, 600, brazoSpeed);
  } else if (datoSerie == '5') {
    mueveStepper(&brazoStepper, -600, brazoSpeed);
  } else if (datoSerie == '7') {
    mueveStepper(&antebrazoStepper, 600, brazoSpeed);
  } else if (datoSerie == '8') {
    mueveStepper(&antebrazoStepper, -600, brazoSpeed);
  } else if (datoSerie == 'a') {
    servoMuneca.write(45);
  } else if (datoSerie == 'b') {
    servoMuneca.write(135);
  } else if (datoSerie == '0') {
    brazoStepper.disableOutputs();
    antebrazoStepper.disableOutputs();
    baseStepper.disableOutputs();
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

