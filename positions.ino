struct position {
  long base;
  long brazo;
  long antebrazo;
  long mano;
};
position *lPositions = NULL;

void savePosition()
{
  nPositions++;
  lPositions = (position*) realloc (lPositions, nPositions * sizeof(position));
  position *pos = &lPositions[nPositions - 1];
  pos->base = baseStepper.currentPosition();
  pos->brazo = brazoStepper.currentPosition();
  pos->antebrazo = antebrazoStepper.currentPosition();

  char body[255] = {0};
  sprintf(body, "SAVED POSITION Base: %05d Brazo: %05d AnteBrazo: %05d Posiciones: %2d Actual: %2d",
          pos->base,
          pos->brazo,
          pos->antebrazo,
          nPositions,
          currentPosition);
  Serial.println(body);

}

void delPosition()
{
  if (nPositions > 0) {
    nPositions--;
    lPositions = (position*) realloc (lPositions, nPositions * sizeof(position));
    currentPosition = -1;
  }
}

void goToPosition(int newPosition)
{
  if (!nPositions) {
    return;
  }

  if (newPosition > nPositions || newPosition < 0) {
    newPosition = 0;
  }
  currentPosition = newPosition;
  position *pos = &lPositions[currentPosition];
  mueveStepper(&baseStepper, pos->base, brazoSpeed);
  mueveStepper(&brazoStepper, pos->brazo, brazoSpeed);
  mueveStepper(&antebrazoStepper, pos->antebrazo, brazoSpeed);

  char body[255] = {0};
  sprintf(body, "GO TO POSITION Base: %05d Brazo: %05d AnteBrazo: %05d Posiciones: %2d Actual: %2d",
          pos->base,
          pos->brazo,
          pos->antebrazo,
          nPositions,
          currentPosition);
  Serial.println(body);
}

bool nextPosition()
{
  if (currentPosition < 0) {
    return false;
  }
  currentPosition++;
  if (currentPosition == nPositions) {
    currentPosition = 0;
//    return false;
  }

  Serial.print("Siguiente posición ");
  Serial.println(currentPosition);
  goToPosition(currentPosition);
  return true;
}

void deletePositions()
{
  free(lPositions);
  lPositions = NULL;
  nPositions = 0;
  currentPosition = -1;
}

void muestraPosiciones()
{
  Serial.print("Total posiciones ");
  Serial.println(nPositions);

  char body[255] = {0};
  for (int pos = 0; pos < nPositions; pos++) {
    sprintf(body, "Pos: %d Base: %05d Brazo: %05d AnteBrazo: %05d",
            pos,
            lPositions[pos].base,
            lPositions[pos].brazo,
            lPositions[pos].antebrazo);
    Serial.println(body);
  }
}

