#include "keyboardDriver.h"
#include "keyboardmap.h"

#define BUFFSIZE 128
#define TRUE 1
#define FALSE 0


char get_key();
int key_pressed();
int isShiftKey(char k);
int processKeyScanCode(char k);
int chooseKeyboard(char k);
int isAlpha(char k);

static int len = sizeof(kbdus);
static char buffer[BUFFSIZE];
static unsigned int ret_index = 0;
static unsigned int store_index = 0;
static int buff_size = 0;
static int shiftPressed = FALSE;
static int capsLockPressed = FALSE;
static int capsLockPressedCount = 0;
static int capsLockActivated = FALSE;
//static int altgrPressed = FALSE;


void store_scancode() {
  if (buff_size == BUFFSIZE)
    return;

  char k = get_key();

  if (k > 0 && k < TOPCHARCODE) { /*Se apreto una tecla */
    
    if(!processKeyScanCode(k)) { //caso de un caracter a imprimir en pantalla
      buff_size++;
      buffer[store_index++] = k;  // guarda el scancode de la tecla 
    }
  } else if (k < 0) { /*Se solto una tecla */
        processKeyScanCode(k + TOPCHARCODE);   
    }
  if (store_index == BUFFSIZE)
    store_index = 0;
}

int get_char() {
  if (buff_size == 0)
    return -1;
  char k = buffer[ret_index++];
  buff_size--;
  if (ret_index == BUFFSIZE)
    ret_index = 0;
  return chooseKeyboard(k);
}

int isShiftKey(char k) {
 return ((k == LSHIFTCODE) || (k == RSHIFTCODE));
}

/* Retorna TRUE si se toco algun caracter especial como SHIFT, CAPSLOCK */
int processKeyScanCode(char k){
  switch(k){
    case RSHIFTCODE:
          shiftPressed = !shiftPressed;
          return TRUE;
          break;
    case LSHIFTCODE:
          shiftPressed = !shiftPressed;
          return TRUE;
          break;
    case CAPSLOCK:
          capsLockPressed = !capsLockPressed;
          capsLockPressedCount += 1;
          if(capsLockPressedCount % 2 == 0) { //fue apretado y soltado 
            capsLockActivated = !capsLockActivated;
            capsLockPressedCount = 0;
          }
          return TRUE;
          break;
   // case ALTGR:
     //     altgrPressed = !altgrPressed;
      //    return TRUE;
        //  break;
    default: 
          return FALSE;
          break;
  }
  return FALSE;

}

int chooseKeyboard(char k) {
  if(shiftPressed)
    return shiftkbdus[k];
  if((capsLockPressed|| capsLockActivated) && isAlpha(k)) {
    return shiftkbdus[k];
  }
  return kbdus[k];
}

int isAlpha(char k) {
  if(( k> QPOSITION && k< PPOSITION) || (k > APOSITION && k < LPOSITION) || (k > ZPOSITION && k< LPOSITION)) 
    return TRUE;
  return FALSE;
}
