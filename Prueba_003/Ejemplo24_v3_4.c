////////////////////////////////////////////////////////////////////////////////
/////                      ex_modbus_master.c                              /////
/////                                                                      /////
// Este codigo contiene la programacion que esta dentro del PIC MODBUS MASTER //
////                                                                        ////
////                                                                        ////   
//                                                                            //
//    Site Player             PIC MODBUS MASTER               MODBUS ESCLAVOS //
//    !--------!    INT EXT     !---------!                       !--------!  //
//    !        !----------------!         !                       !        !  // 
//    !        !                ! modbus  !                       !        !  //
//    !        !     RS232      !_master.c!     Tx   Rx   RS485   ! NODO1  !  //
//    !        !----------------!         !-----------------------!        !  //
//    !        !----------------!         !-----------------------!        !  //
//    !--------!     Tx  Rx     !---------!               | |     !--------!  //
//                                 |         LCD         R| |                 //
//                                 |      !--------!     S| |     !--------!  //       
//                                 |------! XXXXXX !     4| |     !        !  //
//                                        !--------!     8|  -----! NODO2  !  //
//                                                       5|  -----!        !  //
//   Nota: -Las tierras de todos los dispositivos         | |     !        !  //
//          tienen que estar unidas                       | |     !--------!  //
//         -Cuando trabajemos con mas de un modbus slave  | |                 //
//          solo se puede trabajar con RS485.            R| |     !--------!  //
//         -La linea INT EXT es la linea por donde se    S|  -----!        !  //
/////       producira la interrupcion cuando el cliete   4|  -----! NODO3  !  //
/////       Web  envie un formulario.                    8| |     !        !  //
/////                                                    5| |     !        !  //
/////                                                     | |     !--------!  //
/////                                                     . .                 //
/////                                                     . .                 //
/////                                                     . .                 //
/////                                                     . .                 //
/////                                                     . .                 //
/////                                                     . .                 //
////////////////////////////////////////////////////////////////////////////////
/////                                                                      /////
/////  Nota:                                                               /////
/////  Este codigo(que esta dentro del PIC MODBUS MASTER) consta           /////
/////  escencialmente de 3 partes :                                        /////
/////                                                                      /////
/////  1. Generacion de comandos modbus de LECTURA y comandos SitePlayer   /////
/////     --------------------------------------------------------------   /////
/////                                                                      /////
/////     Esta es la parte mas importante del programa, incluso se podria  /////
/////     decir que es la razon de ser del programa.                       /////
/////     Basicamente lo que se hace es generar los comandos modbus de     /////
/////     LECTURA los cuales se enviaran a los modbus esclavos.            /////
/////     Tambien se generan los comandos de ESCRITURA hacia el SitePlayer /////
/////     para escribir en este ultimo los datos obtenidos con los comandos ////
/////     modbus DE LECTURA.                                               /////
/////                                                                      /////
/////                                                                      /////
/////   2. Monitoreo de la configuracion  del SitePlayer                   /////
/////      ---------------------------------------------                   /////
/////                                                                      /////
/////      En esta parte lo que se hara simplemente es extrer datos        /////
/////      de la configuracion que tiene el SitePlayer como por ejemplo    /////
/////      la direccion IP , la direccion MAC , los BAUDIOS y algunos      /////
/////      otros valores que pueda tener el SitePlayer.                    /////
/////      Luego estos valores son mostrados en la pantalla LCD            /////
/////                                                                      /////
/////                                                                      /////
/////   3. Obtencion de valores de un formulario Web                       /////
/////      ------------------------------------------                      /////
/////                                                                      /////
/////      Esta funcion se ejecutara solamente cuando desde el cliente     /////
/////      Web se haiga enviado un formulario, cuando esto suceda el Site  /////
/////      Player emitira un voltaje en uno de sus pines para hacerle      /////
/////      saber al PIC MODBUS MASTER que hay un grupo de valores que este /////
/////      ultimo tiene que recoger.                                       /////
/////      Para ello el PIC MODBUS MASTER tiene que tener configurado      /////
/////      la interrupcion #INT EXT para poder reaccionar justo cuando     /////
/////      cuando se produzca el envio del formulario.                     /////
/////      Una vez que el PIC MODBUS MASTER recoge los valores del Site    /////
/////      Player,pueden ocurrir cualquiera de las 2 opciones:             /////
/////              3.1 Escribir los valores que se acaban de recoger en    /////
/////                  la memoria de datos EEPROM del PIC MODBUS MASTER.   /////
/////              3.2 Crear el comando modbus de ESCRITURA hacia los      /////
/////                  modbus esclavos.                                    /////
/////                                                                      /////
////////////////////////////////////////////////////////////////////////////////








//////////////    Configuracion general del PIC MODBUS MASTER  /////////////////
//                                                                            //
#include <16f877a.h>                                                          //
#fuses HS    //Uso HS ya que el cristal que voy a utilizar es mayor que 4Mhz  //
#use delay(clock=20Mhz)                                                       //
#include <math.h>                                                             //
#include <stdlib.h>                                                           //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


///////////////////// Bits indicadores de encendido    /////////////////////////
#use standard_io(D)

///////      Configuracion de la comunicacion RS232 entre el     ///////////////                                                                            //
///////          PIC MODBUS MASTER y una PC para pruebas.        ///////////////
//                                                                            //
//NOTA: Esta es la configuracion por defecto que se usa en el HIPERTERMINAL   //
#use rs232(baud=9600,parity=N,xmit=PIN_C1,rcv=PIN_C2,bits=8,stream=PC,errors) //
////////////////////////////////////////////////////////////////////////////////






//111111111111111111111111111111111111111111111111111111111111111111111111111111
///////      Configuracion de la comunicacion RS232 entre el PIC        ////////
///////             MODBUS MASTER y el SP(SitePlayer)                   ////////
//                                                                            //
// 1  Definicion numerica de los identificadores                              //
//    Esto es necesario para validar los identificadores                      //
//    Esoy en libertad de colocar cualquier numero                            //
#define HARDWARE 333333                                                       //
#define SOFTWARE 444444                                                       //
                                                                              //
// 2  Primero lo que haremos es definir el baudiaje por defecto que           //
//    presenta el SitePlayer cuando este es reseteado.                        //
#define BAUD_SITEPLAYER_DEFAULT 9600                                          //
                                                                              //
// 3   Ahora definimos cual sera el baudiaje con el que nos comunicaremos     //
//     con el Siteplayer en el modo de proceso.                               // 
#define BAUD_SITEPLAYER_PROCESO 113636                                        //
//Nota: Solo podremos cambiar el baudiaje cuando usamos el modulo USART del   //
//      PIC MODBUS MASTER ya que SOLO en este caso se pueden alcanzar altos   //
//      baudiajes.                                                            //
//      En caso contrario(SOFTWARE) solo podremos trabajar en el baudiaje     //
//      por defecto del Sitelayer,y se ignora esta variable.                  //
//      Tenemos que tener en cuenta que el PIC MODBUS MASTER solo acepta      //
//      una cantidad discreta de baudiajes en su modulo USART , asi que       //
//      aqui no podemos poner cualquier numero de baudios.                    //
                                                                              //
// 4   Ahora definimos si el PIC MODBUS MASTER usara o no,el modulo USART     //
///    para comunicarse serialmente con el SitePlayer.                        //
#define TX_RX_SITEPALYER_RS232_MODO HARDWARE                                  //
//      Use la opcion SOFTWARE si no piensa usar el modulo USART              //  
//      del PIC MODBUS MASTER para comunicarse con el SP.                     //
                                                                              //
// 5   Ahora configuraremos los parametros necesarios para la transmision     //
//     recepcion de datos , asi como tambien algunas funciones.               //    
//                                                                            //
#if (TX_RX_SITEPALYER_RS232_MODO == HARDWARE)                                 //
#use rs232(baud=BAUD_SITEPLAYER_DEFAULT,UART1,parity=N,stream=SP_RS232,errors)//
#byte TXSTA = getenv("sfr:TXSTA")                                             //
#bit TRMT = TXSTA.1                                                           //
                                                                              //
// Nota: Aunque elijamos modo HARDWARE tenemos que iniciar con el baudiaje    //                                                                              
//       por defecto del SitePlayer,luego este baudiaje sera modificado al    //
//       encender el PIC con el SP.                                           //
//       Para ello revise la funcion  "funcion_ADAPTAR_BAUD_SP_Y_PIC()"       //                                                                      
//                                                                            //  
//  Esta funcion nos sirve para que no se ejecute ninguna instruccion         //
//  subsiguiente hasta que se haiga vaceado todo el buffer de transmision     //
//  de la UART.                                                               //
//  NOTA: Este tipo de funcion no es necesaria cuando usamos el               //
//  modo SOFTWARE.                                                            //
#define funcion_buffer_tx_limpio()\                                           
  {\                                                                          
    while(!TRMT);\                                                            
 }                                                                            //                                                                            //
//Solo he definido estas variables para unas pruebas.                         //
#bit BRGH   = getenv("bit:BRGH")                                              //
#byte SPBRG = getenv("sfr:SPBRG")                                             //
//                                                                            //
#else //TX_RX_SITEPALYER_RS232_MODO == SOFTWARE                               //
//Definimos cuales son los pines que se usaran para la comunicacion           //
#define TX_SITEPLAYER_SOFTWARE  0 //puede poner algo como :PIN_B1             //
#define RX_SITEPLAYER_SOFTWARE  0 //puede pomer algo como :PIN_B2             //
#use rs232(baud=BAUD_SITEPLAYER_DEFAULT,xmit=TX_SITEPLAYER_SOFTWARE,rcv=RX_SITEPLAYER_SOFTWARE,parity=N,stream=SP_RS232,errors)
#endif                                                                        //
////////////////////////////////////////////////////////////////////////////////
//111111111111111111111111111111111111111111111111111111111111111111111111111111










#define MODBUS_SERIAL_RX_BUFFER_SIZE   10
#define MODBUS_SERIAL_BAUD             9600   // 19200
#define MODBUS_SERIAL_TX_PIN           PIN_B1   // Data transmit pin
#define MODBUS_SERIAL_RX_PIN           PIN_B3   // Data receive pin
#define MODBUS_SERIAL_ENABLE_PIN       PIN_B2   // Controls DE pin for RS485
#define MODBUS_SERIAL_RX_ENABLE        0        // Controls RE pin for RS485
#use rs232(baud=MODBUS_SERIAL_BAUD, xmit=MODBUS_SERIAL_TX_PIN, rcv=MODBUS_SERIAL_RX_PIN, parity=N, stream=MODBUS_SERIAL,errors)
//#use rs232(baud=MODBUS_SERIAL_BAUD, xmit=MODBUS_SERIAL_TX_PIN, rcv=MODBUS_SERIAL_RX_PIN, parity=N, stream=MODBUS_SERIAL,errors,RESTART_WDT)


enum type_data{INPUT_DISCRETE=1,COIL=2,INPUT_REGISTER=3,HOLDING_REGISTER=4};

enum posbit_datanumeric{BIT_0=1,BIT_1=2,BIT_2=3,BIT_3=4,BIT_4=5,BIT_5=6,BIT_6=7,
                        BIT_7=8,BIT_8=9,BIT_9=10,BIT_10=11,BIT_11=12,BIT_12=13,
                        BIT_13=14,BIT_14=15,BIT_15=16,Entero_de_16_bits=17,
                        Float_Little_Endian=18,Float_Word_Swapped=19};

int1 modbus_frame_OK;         // Este bit indica si el CRC estuvo OK o no.

int8 retry_count_read_MB;     // Este indica la cantidad de intentos que se hace
                              // para la lectura Modbus.
                                  
int8 retry_count_write_MB;    // Este indica la cantidad de intentos que se hace
                              // para la escritura Modbus.
                                  
int16 modbus_address_wait;    // Este indica la cantidad de tiempo que se puede
                              // esperar para la recepcion del primer
                              // byte(address), que segun la norma es 1seg.
                                  
int16 modbus_PDU_crc_wait;    // Este indica la cantidad de tiempo que se puede 
                              // esperar entre byte y byte, que la norma lo 
                              // identifica como t1.5 
float x;

int32 fx;                     // Este es el valor que recibimos del SitePlayer.

















/* Table of CRC values for high–order byte */
const unsigned char modbus_auchCRCHi[] = {
0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,
0x40,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,
0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,
0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,
0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,
0x40,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,
0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,
0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,
0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x01,0xC0,
0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,
0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,
0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x01,0xC0,
0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,
0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,
0x40
};

/* Table of CRC values for low–order byte */
const char modbus_auchCRCLo[] = {
0x00,0xC0,0xC1,0x01,0xC3,0x03,0x02,0xC2,0xC6,0x06,0x07,0xC7,0x05,0xC5,0xC4,
0x04,0xCC,0x0C,0x0D,0xCD,0x0F,0xCF,0xCE,0x0E,0x0A,0xCA,0xCB,0x0B,0xC9,0x09,
0x08,0xC8,0xD8,0x18,0x19,0xD9,0x1B,0xDB,0xDA,0x1A,0x1E,0xDE,0xDF,0x1F,0xDD,
0x1D,0x1C,0xDC,0x14,0xD4,0xD5,0x15,0xD7,0x17,0x16,0xD6,0xD2,0x12,0x13,0xD3,
0x11,0xD1,0xD0,0x10,0xF0,0x30,0x31,0xF1,0x33,0xF3,0xF2,0x32,0x36,0xF6,0xF7,
0x37,0xF5,0x35,0x34,0xF4,0x3C,0xFC,0xFD,0x3D,0xFF,0x3F,0x3E,0xFE,0xFA,0x3A,
0x3B,0xFB,0x39,0xF9,0xF8,0x38,0x28,0xE8,0xE9,0x29,0xEB,0x2B,0x2A,0xEA,0xEE,
0x2E,0x2F,0xEF,0x2D,0xED,0xEC,0x2C,0xE4,0x24,0x25,0xE5,0x27,0xE7,0xE6,0x26,
0x22,0xE2,0xE3,0x23,0xE1,0x21,0x20,0xE0,0xA0,0x60,0x61,0xA1,0x63,0xA3,0xA2,
0x62,0x66,0xA6,0xA7,0x67,0xA5,0x65,0x64,0xA4,0x6C,0xAC,0xAD,0x6D,0xAF,0x6F,
0x6E,0xAE,0xAA,0x6A,0x6B,0xAB,0x69,0xA9,0xA8,0x68,0x78,0xB8,0xB9,0x79,0xBB,
0x7B,0x7A,0xBA,0xBE,0x7E,0x7F,0xBF,0x7D,0xBD,0xBC,0x7C,0xB4,0x74,0x75,0xB5,
0x77,0xB7,0xB6,0x76,0x72,0xB2,0xB3,0x73,0xB1,0x71,0x70,0xB0,0x50,0x90,0x91,
0x51,0x93,0x53,0x52,0x92,0x96,0x56,0x57,0x97,0x55,0x95,0x94,0x54,0x9C,0x5C,
0x5D,0x9D,0x5F,0x9F,0x9E,0x5E,0x5A,0x9A,0x9B,0x5B,0x99,0x59,0x58,0x98,0x88,
0x48,0x49,0x89,0x4B,0x8B,0x8A,0x4A,0x4E,0x8E,0x8F,0x4F,0x8D,0x4D,0x4C,0x8C,
0x44,0x84,0x85,0x45,0x87,0x47,0x46,0x86,0x82,0x42,0x43,0x83,0x41,0x81,0x80,
0x40
};

typedef enum _exception{OK=0,ILLEGAL_FUNCTION=1,ILLEGAL_DATA_ADDRESS=2, 
ILLEGAL_DATA_VALUE=3,SLAVE_DEVICE_FAILURE=4,ACKNOWLEDGE=5,SLAVE_DEVICE_BUSY=6, 
MEMORY_PARITY_ERROR=8,GATEWAY_PATH_UNAVAILABLE=10,GATEWAY_TARGET_NO_RESPONSE=11,
TIMEOUT=12,FALTA_NUMERO_DE_NODO_O_PARAMENTROS_MODBUS=13} exception;

typedef enum _function{FUNC_READ_COILS=0x01,FUNC_READ_DISCRETE_INPUT=0x02,
FUNC_READ_HOLDING_REGISTERS=0x03,FUNC_READ_INPUT_REGISTERS=0x04,
FUNC_WRITE_SINGLE_COIL=0x05,FUNC_WRITE_SINGLE_REGISTER=0x06,
FUNC_READ_EXCEPTION_STATUS=0x07,FUNC_DIAGNOSTICS=0x08,
FUNC_GET_COMM_EVENT_COUNTER=0x0B,FUNC_GET_COMM_EVENT_LOG=0x0C,
FUNC_WRITE_MULTIPLE_COILS=0x0F,FUNC_WRITE_MULTIPLE_REGISTERS=0x10,
FUNC_REPORT_SLAVE_ID=0x11,FUNC_READ_FILE_RECORD=0x14,
FUNC_WRITE_FILE_RECORD=0x15,FUNC_MASK_WRITE_REGISTER=0x16,
FUNC_READ_WRITE_MULTIPLE_REGISTERS=0x17,FUNC_READ_FIFO_QUEUE=0x18} function;
   
 
union {
   int8 b[2];
   int16 d;
} modbus_serial_crc;


struct {
   int8 address;
   int8 len;                                //number of bytes in the message received
   function func;                           //the function of the message received
   exception error;                         //error recieved, if any
   int8 data[MODBUS_SERIAL_RX_BUFFER_SIZE]; //data of the message received
} modbus_rx;



/*
float f_IEEEtoPIC(int32 f)
PURPOSE: This function converts IEEE format to a float
PARAMS: 32 bit number
RETURNS: Floating point number
*/
  float f_IEEEtoPIC(int32 f){

   float * ret;
   #locate f=0x20
   
   ret = &f;
   
   #asm

   // swap the outer bytes
      movf   0x20,W
      xorwf  0x23,W
      xorwf  0x23,F
      xorwf  0x23,W
      movwf  0x20

   // swap the inner bytes
      movf   0x21,W
      xorwf  0x22,W
      xorwf  0x22,F
      xorwf  0x22,W
      movwf  0x21

   #endasm

   #if defined(__PCB__) || defined(__PCM__)
      #asm

      // switch the bits
      rlf   0x21
      rlf   0x20
      rrf   0x21
      
      #endasm
   #elif defined(__PCH__)
      #asm
      
      // switch the bits
      rlcf   0x21
      rlcf   0x20
      rrcf   0x21   
      
      #endasm
   #else
      #error Invalid complier defined
   #endif
   
   return *ret;
}


/*
int32 f_PICtoIEEE(float f)
PURPOSE: This function converts float from CCS -PIC format to IEEE format in little endian mode
PARAMS: Floating point number
RETURNS: 32 bit number
*/
int32 f_PICtoIEEE(float f)
{

   int32 * ret;
   #locate f=0x20

   ret = &f;   

   #if defined(__PCB__) || defined(__PCM__)
      #asm
      
      // switch the bits
      rlf   0x21
      rrf   0x20
      rrf   0x21
      
      #endasm
   #elif defined(__PCH__)
      #asm
      
      // switch the bits
      rlcf   0x21
      rrcf   0x20
      rrcf   0x21   
      
      #endasm
   #else
      #error Invalid complier defined
   #endif
   
   #asm

   // swap the outer bytes
      movf   0x20,W
      xorwf  0x23,W
      xorwf  0x23,F
      xorwf  0x23,W
      movwf  0x20

   // swap the inner bytes
      movf   0x21,W
      xorwf  0x22,W
      xorwf  0x22,F
      xorwf  0x22,W
      movwf  0x21
   
   #endasm
   
   return *ret;
}

 void enviar_data_to_SP(int32 fx,int16 address_SitePlayer){
 
     int8 comando_WriteX;   
 
     // Antes de enviar la trama al Siteplayer deshabilito la 
     // interrupcion por flanco de bajada, para que no se interrumpa
     // el envio de la trama.
        disable_interrupts(INT_EXT);//******************************************                                               
                                                                          
//    El comando de escritura lo generamos con el sgt comando:                 
//    WriteX = 90h => Write Using Extended Two Byte Addressing               
//    Para escribir la cantidad de bytes deseados se debe usar :            
//    WriteX +  cant_de_bytes -1                                             
      comando_WriteX = 0x90 + 4 - 1;                            
                                                                                                                                                     //
      fputc(comando_WriteX,SP_RS232);       //Comando escritura WriteX            
                                                                              
      fputc(make8(address_SitePlayer,0),SP_RS232);                            
                                                                             
      fputc(make8(address_SitePlayer,1),SP_RS232);
      
      fputc(make8(fx,0),SP_RS232);
      
      fputc(make8(fx,1),SP_RS232);
      
      fputc(make8(fx,2),SP_RS232);
      
      fputc(make8(fx,3),SP_RS232);
      
      // Habiltacion de la interrupcion de flanco de bajada, 
      // esto se vuelve a habilitar despues de haber enviado
      // toda la trama al Siteplayer.
         enable_interrupts(INT_EXT);//******************************************
                                                                                
 }

void modbus_calc_crc(char data){
  int8 uIndex ; // will index into CRC lookup table
  uIndex = (modbus_serial_crc.b[1]) ^ data; // calculate the CRC
  modbus_serial_crc.b[1] = (modbus_serial_crc.b[0]) ^ modbus_auchCRCHi[uIndex];
  modbus_serial_crc.b[0] = modbus_auchCRCLo[uIndex];
}


void modbus_serial_putc(int8 c){
   fputc(c,MODBUS_SERIAL);
   modbus_calc_crc(c);
   delay_us(1000000/MODBUS_SERIAL_BAUD); //one stop bit.  not exact
}


void modbus_serial_send_start(int8 to,int8 func){

   modbus_serial_crc.d = 0xFFFF;
   
   output_high(MODBUS_SERIAL_ENABLE_PIN);//Habilitar la transmision en el RS485.

   delay_us(35000000/MODBUS_SERIAL_BAUD); //3.5 character delay

   modbus_serial_putc(to);
   modbus_serial_putc(func);
   
}


void modbus_serial_send_stop(){
   int8 crc_low, crc_high;

   crc_high = modbus_serial_crc.b[1];  
   crc_low  = modbus_serial_crc.b[0];

   modbus_serial_putc(crc_high); 
   modbus_serial_putc(crc_low);
   
   modbus_serial_crc.d=0xFFFF;
   modbus_rx.len = 0;
   
   delay_us(35000000/MODBUS_SERIAL_BAUD); //3.5 character delay

   output_low(MODBUS_SERIAL_ENABLE_PIN);//Habilitar la recepcion en el RS485.
}


int1 kbhit_address_MB(int8 direccion_MB){ // Verificado 
 
   if(kbhit(MODBUS_SERIAL)){
      if(direccion_MB == fgetc(MODBUS_SERIAL)){
         modbus_rx.address = direccion_MB;
         modbus_calc_crc(direccion_MB);
         return TRUE;
      }else
         return FALSE;
   }else 
         return FALSE;
 }
   
 int1 kbhit_funcion_MB(){ // Verificado.
    
      if(kbhit(MODBUS_SERIAL)){
         modbus_rx.func = fgetc(MODBUS_SERIAL);
         modbus_calc_crc(modbus_rx.func);
         return TRUE;
      }else
         return FALSE;
  }
   
 int1 kbhit_data_crc_MB(){
 
      if(kbhit(MODBUS_SERIAL)){
         modbus_rx.data[modbus_rx.len] = fgetc(MODBUS_SERIAL);
         modbus_calc_crc(modbus_rx.data[modbus_rx.len]);
         modbus_rx.len++;
         //fprintf(PC,"Data %x",modbus_rx.data[modbus_rx.len]);******************************************************************************************************
         return TRUE;
      }else{
         return FALSE;
      }
 }
   
void funcion_get_PDU_crc_MB(){
  
   modbus_PDU_crc_wait = 100;
   
   while(!kbhit_funcion_MB() && --modbus_PDU_crc_wait);
 
   if(!modbus_PDU_crc_wait){ // Verificado
      modbus_rx.error = TIMEOUT;
      while(modbus_address_wait--);
      
   }else{
      //fprintf(PC,"modbus_address_wait = %Lu y modbus_PDU_crc_wait = %Lu",modbus_address_wait,modbus_PDU_crc_wait);***************************************************
      while(modbus_PDU_crc_wait){
         modbus_PDU_crc_wait=100;
         while(!kbhit_data_crc_MB() && --modbus_PDU_crc_wait);
      }
      
      if(modbus_serial_crc.d == 0x0000){
          modbus_rx.len-=2;
          modbus_frame_OK=TRUE;
      }else
          modbus_frame_OK=FALSE;
          
      if(modbus_frame_OK){
         if(modbus_rx.func & 0x80)           //did we receive an error?
            modbus_rx.error = modbus_rx.data[0];  //if so grab the error and return true
         else
            modbus_rx.error = 0;
      }else{
          modbus_rx.error = TIMEOUT;
          while(modbus_address_wait--); 
      } 
   } 
}



























void modbus_write_single_coil(int8 address, int16 output_address, int1 on){

       //output_bit( PIN_D4, 1);///*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*--*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
       
       modbus_serial_send_start(address, FUNC_WRITE_SINGLE_COIL);

       modbus_serial_putc(make8(output_address,1));
       modbus_serial_putc(make8(output_address,0));

       if(on)
          modbus_serial_putc(0xFF);
       else
          modbus_serial_putc(0x00);
   
       modbus_serial_putc(0x00);

       modbus_serial_send_stop();
       
       //output_bit( PIN_D4, 0);///*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*--*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

       //**********************************************************************************************************************************************************************
       modbus_address_wait = 50000;
       modbus_frame_OK     = FALSE;
       modbus_rx.error     = 0;
       
       while(!kbhit_address_MB(address) && --modbus_address_wait);
    
       if(!modbus_address_wait)
          modbus_rx.error = TIMEOUT;
       else
          funcion_get_PDU_crc_MB();
      //**********************************************************************************************************************************************************************
  }

 void modbus_write_single_register(int8 address, int16 reg_address, int16 reg_value){
       //output_bit( PIN_D4, 1);///*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*--*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
       modbus_serial_send_start(address, FUNC_WRITE_SINGLE_REGISTER);

       modbus_serial_putc(make8(reg_address,1));
       modbus_serial_putc(make8(reg_address,0));

       modbus_serial_putc(make8(reg_value,1));
       modbus_serial_putc(make8(reg_value,0));

       modbus_serial_send_stop();
   
       //output_bit( PIN_D4, 0);///*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*--*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
   
       //**********************************************************************************************************************************************************************
       modbus_address_wait = 50000;
       modbus_frame_OK     = FALSE;
       modbus_rx.error     = 0;
       
       while(!kbhit_address_MB(address) && --modbus_address_wait);
    
       if(!modbus_address_wait)
          modbus_rx.error = TIMEOUT;
       else
          funcion_get_PDU_crc_MB();
       //**********************************************************************************************************************************************************************
  }

 void modbus_write_multiple_registers(int8 address, int16 start_address, int16 quantity, int16 *values){
   
       int8 i,count;
       
       output_bit( PIN_D4, 1);///*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*--*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
       
       count = quantity*2;

       modbus_serial_send_start(address, FUNC_WRITE_MULTIPLE_REGISTERS);

       modbus_serial_putc(make8(start_address,1));
       modbus_serial_putc(make8(start_address,0));

       modbus_serial_putc(make8(quantity,1));
       modbus_serial_putc(make8(quantity,0));
   
       modbus_serial_putc(count);

       for(i=0; i < quantity; ++i){
          modbus_serial_putc(make8(values[i],1));
          modbus_serial_putc(make8(values[i],0));
       }

       modbus_serial_send_stop();

       output_bit( PIN_D4, 0);///*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*--*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

       //**********************************************************************************************************************************************************************
       modbus_address_wait = 50000;
       modbus_frame_OK     = FALSE;
       modbus_rx.error     = 0;
       
       while(!kbhit_address_MB(address) && --modbus_address_wait);
    
       if(!modbus_address_wait)
          modbus_rx.error = TIMEOUT;
       else
          funcion_get_PDU_crc_MB();
       //**********************************************************************************************************************************************************************
  }

 void modbus_read_discrete_input(int8 address,int16 start_address,int16 quantity){
 
       output_bit( PIN_D4, 1);///*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*--*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
       
       modbus_serial_send_start(address,FUNC_READ_DISCRETE_INPUT);

       modbus_serial_putc(make8(start_address,1));
       modbus_serial_putc(make8(start_address,0));

       modbus_serial_putc(make8(quantity,1));
       modbus_serial_putc(make8(quantity,0));

       modbus_serial_send_stop();
      
       output_bit( PIN_D4, 0);///*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*--*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*     
      
       //**********************************************************************************************************************************************************************
       modbus_address_wait = 50000;
       modbus_frame_OK     = FALSE;
       modbus_rx.error     = 0;
       
       while(!kbhit_address_MB(address) && --modbus_address_wait);
    
       if(!modbus_address_wait)
          modbus_rx.error = TIMEOUT;
       else
          funcion_get_PDU_crc_MB();
      //**********************************************************************************************************************************************************************
      
 }
 
 void modbus_read_coils(int8 address, int16 start_address, int16 quantity){
 
      output_bit( PIN_D4, 1);///*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*--*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
 
       modbus_serial_send_start(address,FUNC_READ_COILS);

       modbus_serial_putc(make8(start_address,1));
       modbus_serial_putc(make8(start_address,0));

       modbus_serial_putc(make8(quantity,1));
       modbus_serial_putc(make8(quantity,0));

       modbus_serial_send_stop();
   
       output_bit( PIN_D4, 0);///*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*--*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*   
   
       //**********************************************************************************************************************************************************************
       modbus_address_wait = 50000;
       modbus_frame_OK     = FALSE;
       modbus_rx.error     = 0;
       
       while(!kbhit_address_MB(address) && --modbus_address_wait);
    
       if(!modbus_address_wait)
          modbus_rx.error = TIMEOUT;
       else
          funcion_get_PDU_crc_MB();
      //**********************************************************************************************************************************************************************
    }

 void modbus_read_input_registers(int8 address, int16 start_address, int16 quantity){
 
      output_bit( PIN_D4, 1);///*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*--*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
 
       modbus_serial_send_start(address, FUNC_READ_INPUT_REGISTERS);

       modbus_serial_putc(make8(start_address,1));
       modbus_serial_putc(make8(start_address,0));

       modbus_serial_putc(make8(quantity,1));
       modbus_serial_putc(make8(quantity,0));

       modbus_serial_send_stop();
   
       output_bit( PIN_D4, 0);///*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*--*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*   
   
       //**********************************************************************************************************************************************************************
       modbus_address_wait = 50000;
       modbus_frame_OK     = FALSE;
       modbus_rx.error     = 0;
       
       while(!kbhit_address_MB(address) && --modbus_address_wait);
    
       if(!modbus_address_wait)
          modbus_rx.error = TIMEOUT;
       else
          funcion_get_PDU_crc_MB();
      //**********************************************************************************************************************************************************************
  }

 void modbus_read_holding_registers(int8 address, int16 start_address, int16 quantity){
  
      output_bit( PIN_D4, 1);///*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*--*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
  
      modbus_serial_send_start(address, FUNC_READ_HOLDING_REGISTERS);

      modbus_serial_putc(make8(start_address,1));
      modbus_serial_putc(make8(start_address,0));

      modbus_serial_putc(make8(quantity,1));
      modbus_serial_putc(make8(quantity,0));

      modbus_serial_send_stop();
   
      output_bit( PIN_D4, 0);///*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*--*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*   
   
      //**********************************************************************************************************************************************************************
      modbus_address_wait = 50000;
      modbus_frame_OK     = FALSE;
      modbus_rx.error     = 0;
       
      while(!kbhit_address_MB(address) && --modbus_address_wait);
    
      if(!modbus_address_wait)
          modbus_rx.error = TIMEOUT;
      else
          funcion_get_PDU_crc_MB();
      //**********************************************************************************************************************************************************************
  }


          
 void funcion_OK(type_data tipo_de_dato,posbit_datanumeric posbit_datanumerico,int16 SP){
 
    switch(tipo_de_dato){
       
        case INPUT_DISCRETE : 
             //fprintf(PC,"Cantidad de Bytes = %u\r\n",modbus_rx.data[0]);
             //fprintf(PC,"Dato INPUT_DISCRETE = %u y Posicion SitePlayer = %Lu\r\n",modbus_rx.data[1],SP);
             fx = modbus_rx.data[1]*10 + 16000000;
             enviar_data_to_SP(fx,SP);   
        break;
     
        case COIL : 
             //fprintf(PC,"Cantidad de Bytes = %u\r\n",modbus_rx.data[0]);
             //fprintf(PC,"Dato COIL = %u y Posicion SitePlayer = %Lu\r\n",modbus_rx.data[1],SP);
             fx = modbus_rx.data[1]*10 + 16000000;
             enviar_data_to_SP(fx,SP);
        break;

        case INPUT_REGISTER : 
             //fprintf(PC,"Cantidad de Bytes = %u\r\n",modbus_rx.data[0]);
          
           if(posbit_datanumerico <= BIT_15){
              //fprintf(PC,"Dato INPUT_REGISTER = %Lu\r\n",make16(modbus_rx.data[1],modbus_rx.data[2]));
              //fprintf(PC,"Dato INPUT_REGISTER BIT(0-15)->%u, Valor = %u\r\n",posbit_datanumerico-1,bit_test(make16(modbus_rx.data[1],modbus_rx.data[2]),posbit_datanumerico-1));
              //fprintf(PC,"Posicion SitePlayer = %Lu\r\n",SP);
              fx = 10*bit_test(make16(modbus_rx.data[1],modbus_rx.data[2]),posbit_datanumerico-1) + 16000000;
              enviar_data_to_SP(fx,SP);
             
           }else if(posbit_datanumerico == Entero_de_16_bits){
              //fprintf(PC,"Dato INPUT_REGISTER  Entero_de_16_bits = %Lu\r\n",make16(modbus_rx.data[1],modbus_rx.data[2]));
              //fprintf(PC,"Posicion SitePlayer = %Lu\r\n",SP);
              //fx = 10*make16(modbus_rx.data[1],modbus_rx.data[2]) + 16000000;    ESTO NO FUNCIONA !!!!!!!!!!!!!!!!!!
              x = make16(modbus_rx.data[1],modbus_rx.data[2]);
              //fprintf(PC,"x = %f\r\n",x);
              fx = 10*x + 16000000;
              //fprintf(PC,"fx = %Lu\r\n",fx);
              enviar_data_to_SP(fx,SP);
          
           }else if((posbit_datanumerico == Float_Little_Endian)){
              //fprintf(PC,"Dato INPUT_REGISTER = %f y Posicion SitePlayer = %Lu\r\n",f_IEEEtoPIC(make32(modbus_rx.data[1],modbus_rx.data[2],modbus_rx.data[3],modbus_rx.data[4])),SP);
              x  = f_IEEEtoPIC(make32(modbus_rx.data[1],modbus_rx.data[2],modbus_rx.data[3],modbus_rx.data[4]));
              fx = floor(10*x)+16000000;
              enviar_data_to_SP(fx,SP);
             
           }else if((posbit_datanumerico == Float_Word_Swapped)){
              //fprintf(PC,"Dato INPUT_REGISTER = %f y Posicion SitePlayer = %Lu\r\n",f_IEEEtoPIC(make32(modbus_rx.data[3],modbus_rx.data[4],modbus_rx.data[1],modbus_rx.data[2])),SP);
              x = f_IEEEtoPIC(make32(modbus_rx.data[3],modbus_rx.data[4],modbus_rx.data[1],modbus_rx.data[2]));
              fx = floor(10*x)+16000000;
              enviar_data_to_SP(fx,SP);
          }    
        break;  
     
        case HOLDING_REGISTER : 
             //fprintf(PC,"Cantidad de Bytes = %u\r\n",modbus_rx.data[0]);
          
           if(posbit_datanumerico <= BIT_15){
               //fprintf(PC,"Dato HOLDING_REGISTER = %Lu\r\n",make16(modbus_rx.data[1],modbus_rx.data[2]));
               //fprintf(PC,"Dato HOLDING_REGISTER BIT(0-15)->%u, Valor = %u\r\n",posbit_datanumerico-1,bit_test(make16(modbus_rx.data[1],modbus_rx.data[2]),posbit_datanumerico-1));
               //fprintf(PC,"Posicion SitePlayer = %Lu\r\n",SP);
               fx = 10*bit_test(make16(modbus_rx.data[1],modbus_rx.data[2]),posbit_datanumerico-1) + 16000000;
               enviar_data_to_SP(fx,SP);
               
           }else if(posbit_datanumerico == Entero_de_16_bits){
               //fprintf(PC,"Dato HOLDING_REGISTER  Entero_de_16_bits = %Lu\r\n",make16(modbus_rx.data[1],modbus_rx.data[2]));
               //fprintf(PC,"Posicion SitePlayer = %Lu\r\n",SP);
               // fx = 10*make16(modbus_rx.data[1],modbus_rx.data[2]) + 16000000;   ESTO NO FUNCIONA !!!!!!!!!!!!!!!!!!
               x = make16(modbus_rx.data[1],modbus_rx.data[2]);
               //fprintf(PC,"x = %f\r\n",x);
               fx = 10*x + 16000000;
               //fprintf(PC,"fx = %Lu\r\n",fx);
               enviar_data_to_SP(fx,SP);
          
           }else if((posbit_datanumerico == Float_Little_Endian)){
               //fprintf(PC,"Dato HOLDING_REGISTER Float_Little_Endian = %f\r\n",f_IEEEtoPIC(make32(modbus_rx.data[1],modbus_rx.data[2],modbus_rx.data[3],modbus_rx.data[4])));
               //fprintf(PC,"Posicion SitePlayer = %Lu\r\n",SP);
               x = f_IEEEtoPIC(make32(modbus_rx.data[1],modbus_rx.data[2],modbus_rx.data[3],modbus_rx.data[4]));
               fx = floor(10*x)+16000000;
               enviar_data_to_SP(fx,SP);
             
           }else if((posbit_datanumerico == Float_Word_Swapped)){
               //fprintf(PC,"Dato HOLDING_REGISTER = %Lu\r\n",make16(modbus_rx.data[1],modbus_rx.data[2]));
               //fprintf(PC,"Dato HOLDING_REGISTER = %Lu\r\n",make16(modbus_rx.data[3],modbus_rx.data[4]));
               //fprintf(PC,"Dato HOLDING_REGISTER Float_Word_Swapped = %f\r\n",f_IEEEtoPIC(make32(modbus_rx.data[3],modbus_rx.data[4],modbus_rx.data[1],modbus_rx.data[2])));
               //fprintf(PC,"Posicion SitePlayer = %Lu\r\n",SP);
               x = f_IEEEtoPIC(make32(modbus_rx.data[3],modbus_rx.data[4],modbus_rx.data[1],modbus_rx.data[2]));
               fx = floor(10*x)+16000000;
               enviar_data_to_SP(fx,SP);
          }
          
        break;  
    }
 }
                  
 void funcion_ILLEGAL_FUNCTION(int16 SP){
     //fprintf(PC,"..Ejecutando funcion_ILLEGAL_FUNCTION"); 
     //fprintf(PC," y Posicion SitePlayer = %Lu\r\n",SP); 
     enviar_data_to_SP( 10*(1600000+ILLEGAL_FUNCTION) + 16000000,SP);
  }
            
 void funcion_ILLEGAL_DATA_ADDRESS(int16 SP){
     //fprintf(PC,"..Ejecutando funcion_ILLEGAL_DATA_ADDRESS"); 
     //fprintf(PC," y Posicion SitePlayer = %Lu\r\n",SP);
     enviar_data_to_SP( 10*(1600000+ILLEGAL_DATA_ADDRESS) + 16000000,SP);
  }
             
 void funcion_ILLEGAL_DATA_VALUE(int16 SP){
    //fprintf(PC,"..Ejecutando funcion_ILLEGAL_DATA_VALUE"); 
    //fprintf(PC," y Posicion SitePlayer = %Lu\r\n",SP);
    enviar_data_to_SP( 10*(1600000+ILLEGAL_DATA_VALUE) + 16000000,SP);
  }
 
 void funcion_SLAVE_DEVICE_FAILURE(int16 SP){
    //fprintf(PC,"..Ejecutando funcion_SLAVE_DEVICE_FAILURE"); 
    //fprintf(PC," y Posicion SitePlayer = %Lu\r\n",SP);
    enviar_data_to_SP( 10*(1600000+SLAVE_DEVICE_FAILURE) + 16000000,SP);
  }

 void funcion_ACKNOWLEDGE(int16 SP){
    //fprintf(PC,"..Ejecutando funcion_ACKNOWLEDGE"); 
    //fprintf(PC," y Posicion SitePlayer = %Lu\r\n",SP);
    enviar_data_to_SP( 10*(1600000+ACKNOWLEDGE) + 16000000,SP);
  }
  
 void funcion_SLAVE_DEVICE_BUSY(int16 SP){
   //fprintf(PC,"..Ejecutando funcion_SLAVE_DEVICE_BUSY"); 
   //fprintf(PC," y Posicion SitePlayer = %Lu\r\n",SP);
   enviar_data_to_SP( 10*(1600000+SLAVE_DEVICE_BUSY) + 16000000,SP);
 }
 
 void funcion_MEMORY_PARITY_ERROR(int16 SP){
    //fprintf(PC,"..Ejecutando funcion_MEMORY_PARITY_ERROR"); 
    //fprintf(PC," y Posicion SitePlayer = %Lu\r\n",SP);
    enviar_data_to_SP( 10*(1600000+MEMORY_PARITY_ERROR) + 16000000,SP);
 }
 
 void funcion_GATEWAY_PATH_UNAVAILABLE(int16 SP){
    //fprintf(PC,"..Ejecutando funcion_GATEWAY_PATH_UNAVAILABLE");
    //fprintf(PC," y Posicion SitePlayer = %Lu\r\n",SP);
    enviar_data_to_SP( 10*(1600000+GATEWAY_PATH_UNAVAILABLE) + 16000000,SP);
 }
 
 void funcion_GATEWAY_TARGET_NO_RESPONSE(int16 SP){
    //fprintf(PC,"..Ejecutando funcion_GATEWAY_TARGET_NO_RESPONSE");
    //fprintf(PC," y Posicion SitePlayer = %Lu\r\n",SP);
    enviar_data_to_SP( 10*(1600000+GATEWAY_TARGET_NO_RESPONSE) + 16000000,SP);
 }

 void  funcion_TIMEOUT(int16 SP){
     //fprintf(PC,"..Ejecutando funcion_TIMEOUT",); 
     //fprintf(PC," y Posicion SitePlayer = %Lu\r\n",SP);
     enviar_data_to_SP( 10*(1600000+TIMEOUT) + 16000000,SP);
  }
    
 void funcion_falta_parametros_MB(int16 SP){
      enviar_data_to_SP( 10*(1600000+FALTA_NUMERO_DE_NODO_O_PARAMENTROS_MODBUS) + 16000000,SP);
 }
    
    
    

 void transaccion_read_MB_and_write_SP(int8 numero_nodo_modbus,type_data tipo_de_dato,posbit_datanumeric posbit_datanumerico,int8 starting_address,int16 address_SP){
   
   int8 cantidad_de_regitros;
   retry_count_read_MB = 3;
   
   output_bit( PIN_D2, 1); ///*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*--*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
   
   // Proceso de lectura de los esclavos Modbus.
   
   switch(tipo_de_dato){
     
     case INPUT_DISCRETE : 
       
      do{
         restart_wdt();
         modbus_read_discrete_input(numero_nodo_modbus,starting_address,0x0001);
      }while((modbus_rx.error == TIMEOUT) && --retry_count_read_MB);
      
     break;
     
     case COIL : 
       
      do{
         restart_wdt();
         modbus_read_coils(numero_nodo_modbus,starting_address,0x0001);
      }while((modbus_rx.error == TIMEOUT) && --retry_count_read_MB);
      
     break;
     
     case INPUT_REGISTER : 
       
      if((posbit_datanumerico==Float_Little_Endian) || (posbit_datanumerico==Float_Word_Swapped))
         cantidad_de_regitros = 2;
      else
         cantidad_de_regitros = 1;
         
      do{
         restart_wdt();
         modbus_read_input_registers(numero_nodo_modbus,starting_address,cantidad_de_regitros);
      }while((modbus_rx.error == TIMEOUT) && --retry_count_read_MB);
      
     break;  
     
     case HOLDING_REGISTER : 
       
       if((posbit_datanumerico==Float_Little_Endian) || (posbit_datanumerico==Float_Word_Swapped))
         cantidad_de_regitros = 2;
      else
         cantidad_de_regitros = 1;
       
      do{
         restart_wdt();
         modbus_read_holding_registers(numero_nodo_modbus,starting_address,cantidad_de_regitros);
      }while((modbus_rx.error == TIMEOUT) && --retry_count_read_MB);
      
     break; 
     
  }   
       output_bit( PIN_D2, 0);///*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*--*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
  
       //output_bit( PIN_D3, 1);///*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*--*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
      // Proceso de escritura en el Site Player  
      
      if(!retry_count_read_MB)
         funcion_TIMEOUT(address_SP);// Verificado !!
         
      else{
         switch(modbus_rx.error){
            case OK :
               funcion_OK(tipo_de_dato,posbit_datanumerico,address_SP);// Verificado !!
               break;
            case ILLEGAL_FUNCTION :
               funcion_ILLEGAL_FUNCTION(address_SP);// Verificado !!
               break;
            case ILLEGAL_DATA_ADDRESS :
               funcion_ILLEGAL_DATA_ADDRESS(address_SP);// Verificado !!
               break;
            case ILLEGAL_DATA_VALUE :
               funcion_ILLEGAL_DATA_VALUE(address_SP);
               break;
            case SLAVE_DEVICE_FAILURE :
               funcion_SLAVE_DEVICE_FAILURE(address_SP);
               break;
            case ACKNOWLEDGE :
               funcion_ACKNOWLEDGE(address_SP);
               break;
            case SLAVE_DEVICE_BUSY :
               funcion_SLAVE_DEVICE_BUSY(address_SP);
               break;
            case MEMORY_PARITY_ERROR :
               funcion_MEMORY_PARITY_ERROR(address_SP);
               break;
            case GATEWAY_PATH_UNAVAILABLE :
               funcion_GATEWAY_PATH_UNAVAILABLE(address_SP);
               break;
            case GATEWAY_TARGET_NO_RESPONSE :
               funcion_GATEWAY_TARGET_NO_RESPONSE(address_SP);
               break;
            }
        } 
        //output_bit( PIN_D3, 0);///*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*--*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
    }
  
 void transaccion_write_MB(int8 numero_nodo_modbus,type_data tipo_de_dato,posbit_datanumeric posbit_datanumerico,int8 starting_address, float value){
 
      int8 cantidad_de_regitros;
      int16 reg_array[2];
      int16 reg;
      retry_count_write_MB = 2;
      retry_count_read_MB = 2;
      
      // Proceso de escritura de los esclavos Modbus.
      
      switch(tipo_de_dato){
      
         case COIL : 
       
             if( (value ==1) || (value ==0)){//Me aseguro que el valor es de tipo binario.
        
                //fprintf(PC,"El valor de float value = %f y el bit = %u y el starting addres es %u\r\n",value,(int1)value,starting_address);
                do{
                   modbus_write_single_coil(numero_nodo_modbus,starting_address, (int1)value);
                }while((modbus_rx.error == TIMEOUT) && --retry_count_write_MB);
      
             }
         break;
       
         case HOLDING_REGISTER : 
       
             if(posbit_datanumerico == Float_Little_Endian){   // Verificado !!
                cantidad_de_regitros = 2;
                reg_array[1] = make16(make8(f_PICtoIEEE(value),1),make8(f_PICtoIEEE(value),0));
                reg_array[0] = make16(make8(f_PICtoIEEE(value),3),make8(f_PICtoIEEE(value),2));
                //fprintf(PC,"numero_nodo_modbus[0] = %Lx\r\n",reg_array[0]);fprintf(PC,"numero_nodo_modbus[1] = %Lx\r\n",reg_array[1]);

                do{
                   modbus_write_multiple_registers(numero_nodo_modbus,starting_address,cantidad_de_regitros,reg_array);
                }while((modbus_rx.error == TIMEOUT) && --retry_count_write_MB);
            
             }else if(posbit_datanumerico == Float_Word_Swapped){  // Verificado !!
                cantidad_de_regitros = 2;
                reg_array[0] = make16(make8(f_PICtoIEEE(value),1),make8(f_PICtoIEEE(value),0));
                reg_array[1] = make16(make8(f_PICtoIEEE(value),3),make8(f_PICtoIEEE(value),2));
                //fprintf(PC,"numero_nodo_modbus[0] = %Lx\r\n",reg_array[0]);fprintf(PC,"numero_nodo_modbus[1] = %Lx\r\n",reg_array[1]);

                do{
                   modbus_write_multiple_registers(numero_nodo_modbus,starting_address,cantidad_de_regitros,reg_array);
                }while((modbus_rx.error == TIMEOUT) && --retry_count_write_MB);
            
             }else if(posbit_datanumerico == Entero_de_16_bits) {  // Verificado !! incluso con decimales !!
            
                do{
                   modbus_write_single_register(numero_nodo_modbus,starting_address,(int16)value);
                }while((modbus_rx.error == TIMEOUT) && --retry_count_write_MB);
            
             }else{
                cantidad_de_regitros = 1;
              
                do{
                  modbus_read_holding_registers(numero_nodo_modbus,starting_address,cantidad_de_regitros);
                }while((modbus_rx.error == TIMEOUT) && --retry_count_read_MB);
                
                if(retry_count_read_MB >0){
                   //fprintf(PC,"retry_count_read_MB = %u y posbit_datanumerico %u\r\n",retry_count_read_MB,posbit_datanumerico-1);
               
                   if(value ==0){
                     reg = make16(modbus_rx.data[1],modbus_rx.data[2]);
                     //fprintf(PC,"reg = %Lu\r\n",reg);
                     bit_clear(reg,posbit_datanumerico-1);
                     //fprintf(PC,"reg = %Lu\r\n",reg);
                   }else if(value ==1){
                     reg = make16(modbus_rx.data[1],modbus_rx.data[2]);
                     //fprintf(PC,"reg = %Lu\r\n",reg);
                     bit_set(reg,posbit_datanumerico-1);
                     //fprintf(PC,"reg = %Lu\r\n",reg);
                   }
                   
                   do{
                      modbus_write_single_register(numero_nodo_modbus,starting_address,reg);
                   }while((modbus_rx.error == TIMEOUT) && --retry_count_write_MB);  
                     
                }
             }
                          
         break;  
      
      }
 }
























//2222222222222222222222222222222222222222222222222222222222222222222222222222//
////////////////////////////////////////////////////////////////////////////////
///////           Configuracion de los COMANDOS MODBUS                  ////////
//                      y COMANDOS SitePlayer                                 //
//                                                                            //
// 1  Definicion numerica de idetificadores                                   //
//    Esto es necesario para validar los identificadores                      //
//    Estoy en libertad de colocar cualquier numero                           //
#define PRUEBA_PC        111111                                               //
#define COMANDOS_MODBUS  222222                                               //
//                                                                            //
//                                                                            //
// 2    Ahora elegiremos el modo de trabajo.                                  //
//      Existen 2 modos :  a. PRUEBA_PC =>  Este modo de trabajo lo usaremos  //
//                            para visualizar atraves de una PC los valores   //
//                            de los campos de una TRAMA MODBUS.              //
//                            Simplemente nos sirve para confirmar que los    //
//                            datos con los cuales se "llenaran" los campos   //
//                            de la TRAMA MODBUS son los correctos.           //
//                                                                            //
//                         b. COMANDOS_MODBUS => Este modo lo uso cuando ya   //
//                            confirme que la TRAMA MODBUS es "llenada"       //
//                            correctamente, eso lo consigo con el modo       //
//                            PRUEBA_PC.                                      //
//                            Esto es para el proceso propiamente dicho.      //
//                                                                            //
      #define MODO_DE_TRABAJO COMANDOS_MODBUS   //la otra opcion seria usar   //
//                                        MODO_DE_TRABAJO == COMANDOS_MODBUS  //
//                                        MODO_DE_TRABAJO == PRUEBA_PC        //
////////////////////////////////////////////////////////////////////////////////
//2222222222222222222222222222222222222222222222222222222222222222222222222222//







//3333333333333333333333333333333333333333333333333333333333333333333333333333//
////////////////////////////////////////////////////////////////////////////////
///////////////        Definicion de variables EXTRAS              /////////////
//                                                                            //
//  Nota : Si bien estos valores son generados de forma automatica, en este   //
//         caso obtendremos los valores para esta seccion a partir de la      //
//         imagen de nombre "EEPROM PIC Y RAM SP" la cual esta adjunta en     //
//         esta carpeta.                                                      //
//                                                                            //
//  Estas variables tienen como finalidad principal, poder obtener las        //
//  direcciones que determinada variable puede ocupar ya sea tanto en la      //
//  memoria EEPROM asi como tambien en la memoria RAM del SitePlayer          //
//                                                                            //
// Todas estas lineas han sido generadas de forma AUTOMATICA por el programa  //
// PHP , la fuente de donde el PHP obtuvo sus datos ha sido la base de datos. //
//                                                                            //
//         ASUMIREMOS QUE PHP NOS ENTREGA LO ESCRITO LINEAS ABAJO             //
//                                                                            //
// Cantidad de nodos  que hay asociados a este PIC MODBUS MASTER              //
#define CANTIDAD_DE_NODOS 3                                                   //
//                                                                            //
// Este array contiene la cantidad de variables que contiene cada nodo        //
int8 array_CANTIDAD_DE_VARIABLES[CANTIDAD_DE_NODOS]  = {1 , 2 , 1 };         //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
// Direccion inicial de la EEPROM que usaremos para escribir los registros    //
// MODBUS.                                                                    //
// Este registro corresponderia a la primera variable del primer nodo         //
#define DIRECCION_INICIAL_EEPROM 0                                            //
//                                                                            //
// Cantidad de bytes que se usa para almacenar el numero de nodo en la EEPROM //
#define BYTES_POR_NODO_EEPROM 1                                               //
//                                                                            //
// Cantidad de bytes que se usa para almacenar el tipo de dato, el starting   //
// address y el posbit_datanumerico en la memoria EEPROM                      //
#define BYTES_POR_VARIABLE_EEPROM 2                                           //
//                                                                            //
// Este array contiene las direcciones que cada uno de los nodos tiene en la  //
// memoria EEPROM.                                                            //
   int8 array_STARTING_ADDRESS_NODO_EEPROM[CANTIDAD_DE_NODOS] =
                                      {DIRECCION_INICIAL_EEPROM , 3 , 8 };    //
//                                                                            //
// Direccion final de la memoria EEPROM que se ha usado para escribir los     //
// registros MODBUS                                                           //
#define DIRECCION_FINAL_EEPROM 10                                             //
//                                                                            //
// Porcentaje que se ha usado de la memoria EEPROM.                           //
// Simplemte es una division del (DIRECCION_FINAL_EEPROM/256)*100             //
#define PORCENTAJE_EEPROM_USADO 4                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
// Direccion inicial de la memoria RAM del SitePlayer que es usado para       //
// almacenar los valores actuales de las variables.                           //
#define DIRECCION_INICIAL_SP_VARIABLE 16                                      //
//                                                                            //
// Cantidad de bytes que ocupa cada variable dentro de la memoria RAM del     //
// SitePlayer                                                                 //
#define BYTES_POR_VARIABLE_SP 4                                               //
//                                                                            //
// Este array indica la direccion en la RAM del SitePlayer desde donde        //
// empiezan a enlistar las variables correspondientes a cada NODO.            //
   int16 array_STARTING_ADDRESS_VARIABLES_SP_POR_NODO[CANTIDAD_DE_NODOS] =
                               { DIRECCION_INICIAL_SP_VARIABLE , 20 , 28 };   //
//                                                                            //
// Ultima direccion de la memoria RAM del SitePlayer en ser usada             //
#define DIRECCION_FINAL_SP 31                                                 //
//                                                                            //
// Porcentaje que se ha usado de la memoria RAM del SitePlayer.               //
// Simplemte es una division del (DIRECCION_FINAL_SP/765)*100                 //
#define PORCENTAJE_SP_USADO 5                                                 //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//Proposito :Esta funcion tiene por finalidad mostrar atraves del LCD , y a   //
//           traves de la PC caso de estar en modo Prueba PC, las variables   //
//           que se obtuvo de la BD mediante PHP.                             //
// Input : Ninguno                                                            //
// Output: Ninguno                                                            //
 void funcion_show_data_from_BD_to_LCD_PC(void){                              //
   int8 i;                                                                    //
   // AQUI TENDRA QUE HABER CODIGO QUE MUESTRE LAS VARIABLES ATRAVES          //
   // DEL LCD                                                                 //
                                                                              //
   //Uso esto para prueba solamente                                           //
   #if (MODO_DE_TRABAJO == PRUEBA_PC)                                         //
     fprintf(PC,"Cantidad de nodos : %u\r\n",CANTIDAD_DE_NODOS);              //
     for(i=0;i<CANTIDAD_DE_NODOS;i++)                                         //
         fprintf(PC,"Cantidad de variables del nodo %u :%u \r\n",i+1,array_CANTIDAD_DE_VARIABLES[i]);               
     fprintf(PC,"\r\nLa direccion inicial de la EEPROM es : %u \r\n",DIRECCION_INICIAL_EEPROM); 
     fprintf(PC,"El numero de nodo es almacenado en : %u byte de la EEPROM \r\n",BYTES_POR_NODO_EEPROM);
     fprintf(PC,"Los parametros MB de cada una de las variables");            //
     fprintf(PC," se almacenan en : %u bytes de la EEPROM \r\n",BYTES_POR_VARIABLE_EEPROM);
     for(i=0;i<CANTIDAD_DE_NODOS;i++)
         fprintf(PC,"La direccion EEPROM donde inicia el nodo %u : %u \r\n",i+1,array_STARTING_ADDRESS_NODO_EEPROM[i]);   
     fprintf(PC,"La ultima direccion de la EEPROM en ser utilizada es : %u \r\n",DIRECCION_FINAL_EEPROM);
     fprintf(PC,"El porcentaje de memoria EEPROM usada es : %u \r\n\r\n",PORCENTAJE_EEPROM_USADO);
     fprintf(PC,"La direccion inicial en ser utilizada de la memoria RAM");   //
     fprintf(PC," del SitePlayer para\r\nvalores actuales de las variables es : %u\r\n",DIRECCION_INICIAL_SP_VARIABLE);
     fprintf(PC,"La cantidad de bytes usados para almacenar ");               //
     fprintf(PC,"el valor actual de una variable\r\nen el SitePlayer es : %u\r\n",BYTES_POR_VARIABLE_SP);
     for(i=0;i<CANTIDAD_DE_NODOS;i++){
         fprintf(PC,"La direccion de la memoria RAM del SitePlayer donde");
         fprintf(PC," inicia las variables \r\ndel nodo %u : %Lu\r\n",i+1,array_STARTING_ADDRESS_VARIABLES_SP_POR_NODO[i]);
     }                                                                        //
     fprintf(PC,"La ultima direccion de la RAM del SitePlayer en ser utilizada es : %u \r\n",DIRECCION_FINAL_SP);
     fprintf(PC,"El porcentaje de memoria RAM del SitePlayer usada es : %u \r\n\r\n",PORCENTAJE_SP_USADO);
   #endif                                                                     //
 }                                                                            //
////////////////////////////////////////////////////////////////////////////////
//3333333333333333333333333333333333333333333333333333333333333333333333333333//




















//4444444444444444444444444444444444444444444444444444444444444444444444444444//
////////////////////////////////////////////////////////////////////////////////
//////////////////////  Funciones asociadas a la creacion de     ///////////////
//                          comando de LECTURA  modbus                        //
////////////////////////////////////////////////////////////////////////////////
//Proposito : Crear los comandos de Lectura MODBUS asi como los comandos de   //
//            escritura SitePlayer de todas la variables.                     //
//            La creacion de estos campos lo realizara con valores obtenidos  //
//            de la memoria EEPROM.                                           //
//Inputs : ninguno.                                                           //
//Output : ninguno.                                                           //
  void funcion_cmd_read_MB_write_SP(void){                                    //
       int8  numero_nodo_modbus;                                              //
       int8  tipo_de_dato;                                                    //
       int8  starting_address;                                                //
       int8  posbit_datanumerico;                                             //
       int16 address_SP;                                                      //
       int8  i,j;                                                             //
       int8  address_EEPROM_ordinal_nodo,address_EEPROM_ordinal_variable;     //
                                                                              //
       //Realizamos el bucle para cada uno de los nodos,y de esta forma       //
       //obtener su numero correspondiente.                                   //
       for(i=1;i<=CANTIDAD_DE_NODOS;i++){                                     //
//                                                                            //
//      Obtencion de la direccion del "i" esimo nodo  en la EEPROM.           //
        address_EEPROM_ordinal_nodo = array_STARTING_ADDRESS_NODO_EEPROM[i-1];//
//      Obtencion del numero nodo correspondiente.                            //
             numero_nodo_modbus = read_eeprom(address_EEPROM_ordinal_nodo);   //
                                                                              //
       //Realizamos el bucle para cada una de las variables de un nodo        //
       //especifico(en este caso el "i" esimo nodo)                           //
            for(j=1;j<=array_CANTIDAD_DE_VARIABLES[i-1];j++){                 //
                                                                              //
//      Obtencion de la direccion de la memoria EEPROM que contien la         //
//      combinacion del tipo_de_dato_MB y el posbit_datanumerico.             //
//      Tenia la sgt forma  xxxyyyyy  donde :  xxx => tipo_de_dato_MB.        //
//                                           yyyyy => posbit_datanumerico     //
        address_EEPROM_ordinal_variable = address_EEPROM_ordinal_nodo + 1 + BYTES_POR_VARIABLE_EEPROM*(j-1);
                                                                              //
//      Una vez obtenida la direccion que contien la combinacion del          //
//      tipo_de_dato_MB y el posbit_datanumerico,la cual es representada por  // 
//      index2, podemos obtener todo el contenido asociado a una variable     //
//      especifica.                                                           //
//                                                                            //
       tipo_de_dato = read_eeprom(address_EEPROM_ordinal_variable)>>5;        //
posbit_datanumerico = read_eeprom(address_EEPROM_ordinal_variable) & 0b00011111;     
   starting_address = read_eeprom(address_EEPROM_ordinal_variable+1);         //
//                                                                            //
//      Ahora toca obtener la direccion del SitePlayer a donde vamos a        //
//      escribir el valor que acabamos de obtener con el comando de           //
//      lectura Modbus.                                                       //
//                                                                            //                                                                              
        address_SP =  array_STARTING_ADDRESS_VARIABLES_SP_POR_NODO[i-1] + BYTES_POR_VARIABLE_SP*(j-1);                                                                  
//                                                                            //                                                                              
        //Este modo lo usaremos para testear el contenido de los campos MB    //
        #if (MODO_DE_TRABAJO == PRUEBA_PC)                                    //
        fprintf(PC,"El ordinal de nodo es %u:\r\n",i);                        //
        fprintf(PC,"El ordinal de variable es %u:\r\n",j);                    //
        fprintf(PC,"El numero de nodo es %u:\r\n",numero_nodo_modbus);        //
        fprintf(PC,"El tipo de dato  es %u:\r\n",tipo_de_dato);               //
        fprintf(PC,"El posbit numerico es %u:\r\n",posbit_datanumerico);      //
        fprintf(PC,"El starting address es %u:\r\n",starting_address);        //
        fprintf(PC,"La direccion del SP es %Lu :\r\n\r\n\r\n",address_SP);    // 
        delay_ms(5000);                                                       //
        //Estas funciones se activaran en el proceso.                         //
        #elif (MODO_DE_TRABAJO == COMANDOS_MODBUS)                            //
                                                                              //
        // Ahora sigue generar el comando de lectura Modbus, pero antes que   //
        // todo tenemos que garantizar que se han escrito valores VALIDOS     //
        // en los campos.                                                     //
        // Solo sera necesario evaluar el numero de nodo y el tipo de dato.   //
        // Como sabemos a diferencia de la memoria RAM, la memoria EEPROM     //
        // inicializa con UNOS.                                               //
           if((numero_nodo_modbus != 0b11111111)&&(tipo_de_dato != 0b00000111)){ 
               //transaccion_read_MB_and_write_SP(0xF7,INPUT_DISCRETE,0,2,16);
                 output_bit( PIN_D1, 1); 
                 transaccion_read_MB_and_write_SP(numero_nodo_modbus,tipo_de_dato,posbit_datanumerico,starting_address,address_SP);
                 output_bit( PIN_D1, 0);
              }                                                               //
        //Esto se ejecuta cuando la variable no tiene aun parametros  MB      //
          else {                                                              //
             funcion_buffer_tx_limpio();  // IMPORTANTISIMO                   //
             funcion_falta_parametros_MB(address_SP);                         //
           }                                                                  //
        #else                                                                 //
        #endif                                                                //
              }                                                               //
       }                                                                      //
  }                                                                           //
////////////////////////////////////////////////////////////////////////////////
//4444444444444444444444444444444444444444444444444444444444444444444444444444//










//5555555555555555555555555555555555555555555555555555555555555555555555555555//
////////////////////////////////////////////////////////////////////////////////
///////     Configuracion  del IRQ_BUFFER del PIC MODBUS MASTER        /////////
//                                                                            //
// 1 Como sabemos el IRQ_BUFFER es un array el cual almacena los registros    //
//   que el PIC MODBUS MASTER obtuvo del SitePlayer.                          //
#define LENGTH_IRQ_BUFFER      0xC    //Cantidad de bytes del Buffer en hex.  //
//(1->registro_status_byte_check_error , 2->registro_ordinal_nodo ........    //   
//....., 10->registro_posbit_datonumerico, 11-> registro_status_byte_original)//
// LENGTH_IRQ_BUFFER tiene que ser menor o igual a 15(0xF).                   //
//                                                                            //
// 2 Definicion de variables que asocien el nombre de cada uno de los         //
//   registros que se extraen del SitePlayer con la posicion que ocupan       //
//   en IRQ_BUFFER.                                                           //
#define registro_status_byte_check_error        0                             //
#define registro_ordinal_nodo                   1                             //
#define registro_numero_nodo                    2                             //
#define registro_ordinal_variable               3                             //
#define registro_valor_variable_raw_byte_0      4     // LSB                  //
#define registro_valor_variable_raw_byte_1      5                             //
#define registro_valor_variable_raw_byte_2      6                             //
#define registro_valor_variable_raw_byte_3      7     // MSB                  //
#define registro_tipo_de_dato_MB                8                             //
#define registro_starting_address_MB            9                             //
#define registro_posbit_datonumerico           10                             //
#define registro_status_byte_original          11                             //
//                                                                            //
// 3                                                                          //
#define registro_random_value_MBMA000X         15     // Es la direccion      //
//del SitePlayer donde se alamacena el valor aleatorio que permite saber si   //
//el SitePlayer y el PIC estan aun conectados.                                //
//El registro en el SitePlayer tiene el nombre del Id del Modbus Master por   //
//ejemplo : MBMA005                                                           //
// 4                                                                          //
#define INICIAL_ADDRESS_SP_IRQ_BUFFER 0x00  //Es la direccion del primer      //
//registro del SitePlayer que contine los datos del formulario                //
//                                                                            //
// 5                                                                          //
#define TIMEOUT_IRQ_BUFFER    1000000    //La cantidad de intentos que        //
//se haran antes de confirmar que NO TUVO EXITO la recepcion de los           //
//registros del SP.                                                           //
////////////////////////////////////////////////////////////////////////////////

///////////                 Variable Globales                       ////////////
//                    Asociadas al array IRQ_BUFFER                           //
int8  IRQ_BUFFER[LENGTH_IRQ_BUFFER];                                          //
int8  index_irq_buffer;                                                       //
////////////////////////////////////////////////////////////////////////////////

///////      Funciones asociadas a la interrupcion #INT EXT       //////////////
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//Proposito : generar el comando de lectura para leer los datos del           //
//            SP(SitePlayer).                                                 //
//            No se encarga de recepcionar los datos,eso lo hara otra funcion.//
//Nota: Si usa USART,incluye una funcion para LIMPIAR el buffer               //
//      del recepcion.                                                        //
//Inputs :  int8 cant_de_bytes    Indica el tamaño del IRQ_BUFFER y por       //
//                                consiguiente la cantidad de bytes que       //
//                                hay que solicitar al SItePlayer.            //
//                                Max valor 16, ya que no se pueden leer      //
//                                mas de 16 bytes de un solo comando.         //
//                                                                            //
//     int16 primera_direccion    Indica la primera direccion del             //
//                                SitePlayer que hay que leer.                //
//                                                                            //
//Outputs :  ninguno                                                          //
void funcion_send_command_IRQ_BUFFER(int8 cant_de_bytes,int16 primera_direccion){
      int8 comando_ReadX;                                                     //
//                                                                            //
//    El comando de lectura lo generamos con el sgt comando:                  //
//    ReadX = 0D0h => Read Using Extended Two Byte Addressing                 //
//    Para leer la cantidad de bytes deseados se debe usar :                  //
//    ReadX +  cant_de_bytes -1                                               //
      comando_ReadX = 0xD0 + cant_de_bytes - 1;                               //
//                                                                            //      
//    Limpeza del buffer de recepcion.                                        //
      #if (TX_RX_SITEPALYER_RS232_MODO == HARDWARE)                           //
         while(kbhit(SP_RS232)){                                              //
              fgetc(SP_RS232);                                                //
         }                                                                    //
      #endif                                                                  //
                                                                              //
      fputc(comando_ReadX,SP_RS232);       //Comando lectura ReadX            //
                                                                              //
      fputc(make8(primera_direccion,0),SP_RS232);                             //
                                                                              //
      fputc(make8(primera_direccion,1),SP_RS232);                             //
    }                                                                         //
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//Proposito : esta funcion tiene varios propositos :                          //
//             1. Verificar si se ha recibido un caracter mendiante la        //
//                la funcion kbhit().                                         //
//             2. Luego de recibir el caracter se evalua en que posicion      //
//                del array IRQ_BUFFER[] se debe colocar.                     //
//             3. En caso de que el caracter recibido sea el correspondiente  //
//                a la ultima posicion del array IRQ_BUFFER se emite una      //
//                confirmacion mediante una variable booleana.                //
//Input : Ninguno                                                             //
//Output: 1 si es que se completaron todas las posiciones del array           //
//          IRQ_BUFFER[].                                                     //
//        0 por que no se ha registrado un nuevo caracter, o por que no se    //
//          ha llenado por completo el array IRQ_BUFFER[LENGTH_IRQ_BUFFER].   //
  int1 get_OK(void){                                                          //
    int8 c;                                                                   //
    if(kbhit(SP_RS232)==1){ //se ha registrado un nuevo caracter              //
        c = fgetc(SP_RS232);                                                  //
        IRQ_BUFFER[index_irq_buffer]=c;                                       //
        index_irq_buffer++;                                                   //
        if(index_irq_buffer == LENGTH_IRQ_BUFFER)                             //
          return TRUE;         //se lleno el array IRQ_BUFFER por completo.   //
                                                                              //
          return FALSE;        //aun no se lleno el array IRQ_BUFFER.         //
       }                                                                      //
          return FALSE;       //kbhit()==0.                                   //
    }                                                                         //
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//Proposito:consulta  "TIMEOUT_IRQ_BUFFER" veces(en el peor de los casos),    //
//          si es que ya se recibieron los LENGTH_IRQ_BUFFER bytes que nos    //
//          deberia haber enviado el SitePlayer.                              //                 
//Input : Ninguno.                                                            //
//Ouputs: 1 si es que se recibieron los LENGTH_IRQ_BUFFER bytes del SP.       //
//        0 si es que no se recibieron los LENGTH_IRQ_BUFFER bytes del SP     //
//          DURANTE las "TIMEOUT_IRQ_BUFFER" veces consultadas.               //
//Nota: Esta funcion solo confirma la recepcion de los LENGTH_IRQ_BUFFER      //
//      bytes pero no verifica el contenido de estas,eso lo hace otra funcion.//
  int1 funcion_IRQ_BUFFER_OK(void){                                           //
        int32 wait_irq_buffer = TIMEOUT_IRQ_BUFFER;                           //
        index_irq_buffer = 0;                                                 //
        while((!get_OK()) && (--wait_irq_buffer));                            //
        if(!wait_irq_buffer)                                                  //
           return FALSE;//Si en los TIMEOUT_IRQ_BUFFER no se recibieron los   //
                        //LENGTH_IRQ_BUFFER  Bytes del SP.                    //
        else                                                                  //
           return TRUE; //Si la razon por la que se salio del bucle while     //
                        //es por que get_OK()==1                              //
  }                                                                           //
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//Proposito: esta funcion lo que hace es resetear(escribir CERO) el byte del  //
//           SP que este vinculado a "registro_status_byte_check_error" .     //
// Input : ninguno.                                                           //
// Output: ninguno.                                                           //
  void reset_status_byte_check_error(void){                                   //
//                                                                            //  
     int8 comando_reset_status_byte;                                        //
                                                                              //
 #if (MODO_DE_TRABAJO == PRUEBA_PC)                                           //
  fprintf(PC,"\r\nReseteando el registro :registro_status_byte_check_error\r\n\r\n");//
 #endif                                                                       //
//                                                                            //
//    El comando de escritura lo generamos con el sgt comando :               // 
//    Write = 80h => Write Object to SitePlayer.                              //
//    Como solo queremos escribir un byte el comando sera :                   //
//    Write + cant de bytes -1                                                //
      comando_reset_status_byte = 0x80 + 1 -1 ;                               //
                                                                              //
//    Enviamos el primer byte el cual corresponde al comando calculado        //
//    lineas arriba                                                           //
      fputc(comando_reset_status_byte,SP_RS232);                              //
                                                                              //
//    Enviamos el segundo byte correspondiente a la direccion donde se        //
//    escribira el dato.                                                      //
      fputc(INICIAL_ADDRESS_SP_IRQ_BUFFER,SP_RS232);                          //
                                                                              //
//    Enviamos el dato propiamente dicho, en este caso CERO                   //
      fputc(0x00,SP_RS232);                                                   //
   }                                                                          //
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//Proposito: esta funcion lo que hace es mostrar todo el contenido del        //
//           array IRQ_BUFFER.                                                //
//  Input : ninguno                                                           //
//  Output: ninguno                                                           //
 #if (MODO_DE_TRABAJO == PRUEBA_PC)                                           //
    void funcion_show_IRQ_BUFFER(void){                                       //
      int8 i;                                                                 //
      fprintf(PC,"\r\nEl array IRQ_BUFFER se completo! \r\n");                //
      for(i=0;i<LENGTH_IRQ_BUFFER;i++){                                       //
           fprintf(PC,"El registro %u es:%u\r\n",i,IRQ_BUFFER[i]);            //
      }                                                                       //
   }                                                                          //
 #endif                                                                       //
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//Proposito: esta funcion se encarga de revisar el contenido del              //
//           array IRQ_BUFFER, como por ejemplo :                             //
//      1.  Verificar que el primer byte(registro_Status_Byte_Check_Error)    // 
//          sea distinto de CERO.                                             //
//      2.  Verificar que el primer byte(registro_Status_Byte_Check_Error)    //
//          sea igual al ultimo byte(registro_Status_Byte_Original).          //
// Input  : Ninguno.                                                          //
// Output : 00000xxx(en binario) donde  si xxx es :                           //
//              a. 001(=1)   -> el formulario enviado corresponde a un        //
//                              Submit Nodo MB.                               //
//              b. 010(=2)   -> el formulario enviado corresponde a un        //
//                              Submit Registros MB.                          //
//              c. 100(=4)   -> el formulario enviado corresponde a un        //
//                              Submit tipo Valor Variable.                   //
//              d. otros     -> el array IRQ_BUFFER presenta algun error.     //
//                                                                            //
  int8 funcion_check_IRQ_BUFFER(void){                                        //
       int8 tipo_submit;                                                      //
//                                                                            //
//     Verificar que "registro_Status_Byte_Check_Error" sea distinto de CERO. //
        if(IRQ_BUFFER[registro_status_byte_check_error] == 0){                //
           fprintf(PC,"El primer registro es cero,probablemete se trate de ruido electrico\r\n");
           return (0);                                                        //
        }                                                                     //
//                                                                            //
//     Verificar que "registro_Status_Byte_Check_Error" sea igual a           //
//     "registro_Status_Byte_Original".                                       //
        if(IRQ_BUFFER[registro_status_byte_check_error] != IRQ_BUFFER[registro_status_byte_original]){
           fprintf(PC,"El primer registro y el ultimo registro son diferentes\r\n");
           return (0);                                                        //
        }                                                                     //
//                                                                            //
//    Se procedera a devolver los 3 bits menos significativos del             //
//    IRQ_BUFFER[registro_status_byte_check_error] , pero previamente se      //
//    convertiran a CERO los 5 bits mas significativos.                       //
      tipo_submit=IRQ_BUFFER[registro_status_byte_check_error] & (0b00000111);//
//                                                                            //
        return tipo_submit;                                                   //
   }                                                                          //
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Proposito: esta funcion se ejecuta cuando se recibe un formulario por      //
// parte del SitePlayer del tipo "Submit Nodo MB" y tiene por finalidad       //
// escribir esos valores del formulario mencionado en la memoria EEPROM.      //
// Input  : ninguno.                                                          //
// Output : ninguno.                                                          //
// Nota   : Para realizar la escritura sobre la EEPROM necesitaremos          //
//          los arrays : "array_STARTING_ADDRESS_NODO_EEPROM" y "IRQ_BUFFER". //
                                                                              //
   void funcion_submit_nodo_MB(void){                                         //
                                                                              //
   int8 address_EEPROM_ordinal_nodo;                                          //
                                                                              //
   //Obtencion de la direccion EEPROM donde se colocara el numero de nodo.    //
   address_EEPROM_ordinal_nodo = array_STARTING_ADDRESS_NODO_EEPROM[ IRQ_BUFFER[registro_ordinal_nodo]-1 ];
                                                                              //
   //Escritura del numero de nodo en la EEPROM                                //
   write_eeprom(address_EEPROM_ordinal_nodo,IRQ_BUFFER[registro_numero_nodo]);//
                                                                              //
   #if (MODO_DE_TRABAJO == PRUEBA_PC)                                         //
      fprintf(PC,"Se acaba de escribir en la EERPROM el sgt numero de nodo es %u:\r\n",IRQ_BUFFER[registro_numero_nodo]);
      fprintf(PC,"La direccion donde se escribio el numero de nodo es %u:\r\n",address_EEPROM_ordinal_nodo);
   #endif                                                                     //
                                                                              //
   }                                                                          //
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Proposito: esta funcion se ejecuta cuando se recibe un formulario por      //
// parte del SitePlayer del tipo "Submit Registros MB" y tiene por finalidad  //
// escribir esos valores del formulario mencionado en la memoria EEPROM.      //
// Input  : ninguno.                                                          //
// Output : ninguno.                                                          //
// Nota   : Para realizar la escritura sobre la EEPROM necesitaremos          //        
//          los arrays : "array_STARTING_ADDRESS_NODO_EEPROM" y "IRQ_BUFFER". //
//                                                                            //
 void funcion_submit_regristros_MB(){                                         //
     int8 address_EEPROM_ordinal_nodo ,address_EEPROM_ordinal_variable;       //
     int8 x,y ;                                                               //
                                                                              //
    // Primero ubicamos la direccion del numero de nodo en la EEPROM          //
    // correspondiente a esa variable.                                        //
    address_EEPROM_ordinal_nodo = array_STARTING_ADDRESS_NODO_EEPROM[ IRQ_BUFFER[registro_ordinal_nodo]-1 ];
                                                                              //
    // Ahora vamos a buscar la direccion de la EEPROM donde se van a ubicar   //
    // los registros MB(registro_tipo_de_dato_MB,registro_starting_address_MB,//
    // y registro_posbit_datonumerico)                                        //
    address_EEPROM_ordinal_variable = address_EEPROM_ordinal_nodo + 1 + BYTES_POR_VARIABLE_EEPROM*(IRQ_BUFFER[registro_ordinal_variable]-1);
                                                                              //
    // Escritura del primer byte : xxxyyyyy  donde :                          //
    // xxx    -> son 3 bits que representan el tipo_de_dato_MB(input discrete,//
    // coil,input register y holding register)                                //
    // yyyyy  -> son 5 bits que representan posbit_datonumerico (bit0,bit1 ...//
    // bit15,entero de 16 bits,float little endian, float word swapp)         //
      x = IRQ_BUFFER[registro_tipo_de_dato_MB]     & (0b00000111);            //
      x = x<<5;                                                               //
      y = IRQ_BUFFER[registro_posbit_datonumerico] & (0b00011111);            //
      write_eeprom(address_EEPROM_ordinal_variable,x|y);                      //
                                                                              //
    // Escritura del segundo byte el cual corresponde al starting_address_MB  //
      write_eeprom(address_EEPROM_ordinal_variable+1,IRQ_BUFFER[registro_starting_address_MB]);
                                                                              //
    #if (MODO_DE_TRABAJO == PRUEBA_PC)                                        //
      fprintf(PC,"Se acaba de escribir en la EERPROM el sgt tipo de dato MB es %u:\r\n",IRQ_BUFFER[registro_tipo_de_dato_MB]);
      //fprintf(PC,"Se acaba de escribir en la EERPROM el sgt posbit datomunerico %u:\r\n",IRQ_BUFFER[registro_posbit_datonumerico]);
      //fprintf(PC,"Se acaba de escribir en la EERPROM el sgt starting address MB %u:\r\n",IRQ_BUFFER[registro_starting_address_MB]);
      //fprintf(PC,"La direccion donde se escribio el numero de nodo es %u y %u :\r\n\r\n",address_EEPROM_ordinal_variable,address_EEPROM_ordinal_variable+1);
   #endif                                                                     //
     }                                                                        //
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Proposito: esta funcion tiene por finalidad obtener el valor original      //
//            de la variable , el cual fue colocado por el usuario en la      //
//            pagina web.                                                     //
//            Ya que como recordaremos este valor fue modificado y convertido //
//            a entero positivo por el backend para que pueda ser almacenado  //
//            en el SitePlayer.                                               //
//  Input : fx -> representa el valor almacenado en el SitePlayer.            //
//  Output:  x -> representa el valor original de la variable.                //
  float funcion_inversa(int32 fx){                                            //
    float x;                                                                  //
    x = ((float)fx-16000000)/10;                                              //
    //fprintf(PC,"El valor de x es :%f \r\n",x);                              //
    return x;                                                                 //
  }                                                                           //
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Proposito: esta funcion se ejecuta cuando se recibe un formulario por      //
//            parte del SitePlayer del tipo "Submit Valor Variable" y tiene   //
//            por finalidad  usar esos valores del formulario mencionado      //
//            para la creacion de los comandos de escritura MODBUS.           //
// Input  : ninguno                                                           //
// Output : ninguno                                                           //
// Nota   : Para realizar la creacion de los comandos de escritura MODBUS     //
//          tambien necesitaremos los arrays :                                //
//          "array_STARTING_ADDRESS_NODO_EEPROM" y "IRQ_BUFFER".              //
    void funcion_submit_valor_variable(){                                     //
         int8  numero_nodo_modbus;                                            //
         int8  tipo_de_dato;                                                  //
         int8  starting_address;                                              //
         int8  posbit_datanumerico;                                           //
         int32 value_variable_raw;                                            //
         float value_variable;                                                //
         int8  address_EEPROM_ordinal_nodo ,address_EEPROM_ordinal_variable;  //
                                                                              //
//  Ubicacion de la direccion eeprom que almacena el numero de nodo modbus y  //
//  extraccion del contenido en la variable numero_nodo_modbus.               //
    address_EEPROM_ordinal_nodo = array_STARTING_ADDRESS_NODO_EEPROM[ IRQ_BUFFER[registro_ordinal_nodo]-1 ];
    numero_nodo_modbus = read_eeprom(address_EEPROM_ordinal_nodo);            //
                                                                              //
//  Ubicacion de la direccion eeprom que almacena los registros MB y          //
//  extraccion del contenido para asignarlas en las sgtes variables.          //
    address_EEPROM_ordinal_variable = address_EEPROM_ordinal_nodo + 1 + BYTES_POR_VARIABLE_EEPROM*(IRQ_BUFFER[registro_ordinal_variable]-1);
                                                                              //
    tipo_de_dato        = read_eeprom(address_EEPROM_ordinal_variable)>>5;    //
    posbit_datanumerico = read_eeprom(address_EEPROM_ordinal_variable)& (0b00011111);        
    starting_address    = read_eeprom(address_EEPROM_ordinal_variable+1);     //
    value_variable_raw  = make32(IRQ_BUFFER[registro_valor_variable_raw_byte_3],IRQ_BUFFER[registro_valor_variable_raw_byte_2],IRQ_BUFFER[registro_valor_variable_raw_byte_1],IRQ_BUFFER[registro_valor_variable_raw_byte_0]);
    value_variable      = funcion_inversa(value_variable_raw);                //
                                                                              //
//   Uso esto para prueba solamente.                                          //
 #if (MODO_DE_TRABAJO == PRUEBA_PC)                                           //
                                                                              //
     fprintf(PC,"El address de la variable en la EEPROM es : %u\r\n",address_EEPROM_ordinal_variable);
     fprintf(PC,"El numero de nodo es %u:\r\n",numero_nodo_modbus);           //
     fprintf(PC,"El tipo de dato  es %u:\r\n",tipo_de_dato);                  //
     fprintf(PC,"El posbit numerico es %u:\r\n",posbit_datanumerico);         //
     fprintf(PC,"El starting address es %u:\r\n",starting_address);           //
     fprintf(PC,"El valor de f(x) es : %Lu\r\n",value_variable_raw);          //
     fprintf(PC,"El valor de x es :%f \r\n",value_variable);                  //
                                                                              //
  #elif (MODO_DE_TRABAJO == COMANDOS_MODBUS)                                  //
     if((numero_nodo_modbus != 0b11111111)&&(tipo_de_dato != 0b00000111)){    //
          //fprintf(PC,"El valor de f(x) es : %Lu\r\n",value_variable_raw);   //
          //fprintf(PC,"El valor de x es :%f \r\n",value_variable);           //
          transaccion_write_MB(numero_nodo_modbus,tipo_de_dato,posbit_datanumerico,starting_address,value_variable);    
     }                                                                        //
  #else                                                                       //
                                                                              //
  #endif                                                                      //
  }                                                                           //
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//Proposito :Esta funcion tiene por finalidad enviar un valor aleatorio a la  //
//           posicion 15 del SitePlayer, posicion que ha recibido el nombre   //
//           registro_random_value_MBMA000X(es importante mencionar que este  //
//           registro en el SitePlayer tiene el nombre del Id del Modbus      //
//           Master por ejmplo MBMA005.                                       //
// Input : Ninguno                                                            //
// Output: Ninguno                                                            //
 void ramdon_value_to_MBMA00X(){                                              //
//                                                                            //
// Write = 80h => Write Object to SitePlayer                                  //
// Write +  cant_de_bytes -1                                                  //
// comando_Write = 0x80 + 1 - 1;                                              //
   int8 comando_Write = 0x80;                                                 //
//                                                                            //
// Valor que recibira el valor aleatorio.                                     //
   int8 value_mbma00x;                                                        //
//                                                                            //
// Asignacion del valor aleatorio.                                            //
// El numero maximo que se puede asignar por defecto es 32676 , entonces lo   //
// cambiare por 127 con la directiva #define , pero antes eliminare la        //
// definicion por defecto con la directiva #undef                             //
#undef  RAND_MAX                                                              //
#define RAND_MAX  127                                                         //
   value_mbma00x = rand();                                                    //
//                                                                            //
// Antes de enviar la trama al Siteplayer deshabilito la                      //
// interrupcion por flanco de bajada, para que no se interrumpa               //
// el envio de la trama.                                                      //
   disable_interrupts(INT_EXT);//******************************************   //                                             
//                                                                            //
//                                                                            //
   fputc(comando_Write,SP_RS232);                                             //
//                                                                            //
   fputc(registro_random_value_MBMA000X,SP_RS232);                            //
//                                                                            //      
   fputc(value_mbma00x,SP_RS232);                                             //
//                                                                            //
// Habiltacion de la interrupcion de flanco de bajada,                        //
// esto se vuelve a habilitar despues de haber enviado                        //
// toda la trama al Siteplayer.                                               //
   enable_interrupts(INT_EXT);//******************************************    //                                                                        //
 }                                                                            //
////////////////////////////////////////////////////////////////////////////////
//5555555555555555555555555555555555555555555555555555555555555555555555555555//










//6666666666666666666666666666666666666666666666666666666666666666666666666666//
////////////////////////////////////////////////////////////////////////////////
///////        Funciones de arranque del PIC MODBUS MASTER              ////////
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//Proposito: Esta funcion devuelve el baudiaje a la que actualmente esta      //
//           trabajando el modulo USART del PIC MODBUS MASTER.                //
//           No tiene caso usarlo cuando trabajamos en el modo SOFTWARE.      //
// Input  : ninguno.                                                          //
// Output : baudiaje del modulo USART del PIC MODBUS MASTER.                  //
 #if (TX_RX_SITEPALYER_RS232_MODO == HARDWARE)                                //
    int32 baud1(void){                                                        //
        int32 baudios1,baudios2,baudios3;                                     //
                                                                              //
        baudios1 = (1+SPBRG);                                                 //
        baudios2 = 16*baudios1;                                               //
        baudios3 = 20000000/baudios2;                                         //
    return baudios3;                                                          //
 }                                                                            //
 #endif                                                                       //
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Proposito: esta funcion se encarga de generar y enviar los comandos para   //
//            configurar el baudiaje del SP al BAUD_SITEPLAYER_PROCESO.       //
// NOTA      : Esta funcion sera llamada solo cuando el PIC MODBUS MASTER     //
//             este configurada en modo HARDWARE.                             //
// Inputs:    int32  baud       Esta variable es el baudiaje al cual          //
//                              se quiere modificar el SitePlayer.            //
// Outputs:   ninguno                                                         //
#if (TX_RX_SITEPALYER_RS232_MODO == HARDWARE)                                 //
 void funcion_set_SP_BAUD(int32 baud){                                        //
   int16 set_bytes_baud_rate;                                                 //
   int16 set_delay_response;                                                  //
                                                                              //
// Se procede a calcular los 2 bytes para setear el baud rate del SP          //
// mediante la sgte ecuacion:  65536-(1250000/baudrate)                       //
   set_bytes_baud_rate = 65536-(1250000/baud);                                //
                                                                              //
// Se procede a calcular los 2 bytes para setaer el delay de la respuesta     //
// del UART del SitePlayer mediante la siguite ecuacion:                      //
// 65536 - (timeinmicroseconds * 3.333333)                                    //
// Nota: trabajeremos siempre con un delay de 300 useg, ese es por defecto.   //
   set_delay_response = 65536 - (300 * 3.333333);                             //
                                                                              //
// Uso esto para prueba solamente                                             //
 #if (MODO_DE_TRABAJO == PRUEBA_PC)                                           //
   fprintf(PC,"Set_bytes_baud_rate_0 %X : \r\n",make8(set_bytes_baud_rate,0));//
   fprintf(PC,"Set_bytes_baud_rate_1 %X : \r\n",make8(set_bytes_baud_rate,1));//
   fprintf(PC,"Set_delay_response_0  %X :\r\n",make8(set_delay_response,0));  //
   fprintf(PC,"Set_delay_response_1  %X :\r\n",make8(set_delay_response,1));  //
 #endif                                                                       //
//                                                                            //
// El comando de escritura de baudios y delay lo generamos                    //
// con el sgt comando:                                                        //
// CommParams = 33h => Sets Buad Rate and UART delay.                         //
   fputc(0x33,SP_RS232);                                                      //
                                                                              //
// Bytes correspondientes al baudiaje.                                        //
   fputc(make8(set_bytes_baud_rate,0),SP_RS232);                              //
   fputc(make8(set_bytes_baud_rate,1),SP_RS232);                              //
                                                                              // 
// Bytes correspondientes al delay.                                           //
   fputc(make8(set_delay_response,0),SP_RS232);                               //
   fputc(make8(set_delay_response,1),SP_RS232);                               //
                                                                              //
 }                                                                            //
#endif                                                                        //
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//Proposito : Esta funcion tiene los siguientes propositos :                  //
//            1. Mostrar los valores de algunos registros correspondientes a  //
//               la USART en el LCD.                                          //
//            2. Si estamos en el modo prueba enviar los mismos registros     //
//               hacia la PC de prueba.                                       //
 #if (TX_RX_SITEPALYER_RS232_MODO == HARDWARE)                                //
                                                                              //
   void show_registros_USART_by_LCD_Y_PC(void){                               //
                                                                              // 
      // AQUI SE TENDRA QUE ESCRIBIR CODIGO PARA MOSTRARLO                    // 
      // A TRAVES DE LA PANTALLA LCD.                                         // 
                                                                              //
       //Uso esto para prueba solamente                                       //
       #if (MODO_DE_TRABAJO == PRUEBA_PC)                                     //
          fprintf(PC,"El BRGH es %u:\r\n",BRGH);                              //
          fprintf(PC,"El SPBRG es %u:\r\n",SPBRG);                            //
          fprintf(PC,"El nuevo baudiaje del SP es  es %Lu:\r\n\r\n",baud1()); //
       #endif                                                                 //
    }                                                                         //
                                                                              //
 #endif                                                                       //
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//Proposito: Esta funcion tiene varios propositos.                            //
//         1. Configurar el SitePlayer al baudiaje de BAUD_SITEPLAYER_PROCESO //
//         2. El PIC MODBUS MASTER autoconfigura su propio baudiaje al valor  //
//            BAUD_SITEPLAYER_PROCESO mediante set_uart_speed()               //
//       Es necesario que antes de que el PIC MODBUS MASTER autoconfigure     //
//       su buadiaje con setuart_speed() se garantize que el buffer de        //
//       transmision haiga sido vaceado completamente.                        //
//         3.Mostrar valores de algunos registros atraves de la pantalla LCD  //
//           o PC de prueba.                                                  //
 #if (TX_RX_SITEPALYER_RS232_MODO == HARDWARE)                                //
                                                                              //
  void funcion_ADAPTAR_BAUD_SP_Y_PIC(void){                                   //
                                                                              //
     #if (MODO_DE_TRABAJO == PRUEBA_PC)                                       //
      fprintf(PC,"Iniciando cambio de baudiaje ....\r\n");                    //
     #endif                                                                   //
                                                                              //
      funcion_set_SP_BAUD(BAUD_SITEPLAYER_PROCESO);                           //
      funcion_buffer_tx_limpio();                         //IMPORTANTISIMO    //
      set_uart_speed(BAUD_SITEPLAYER_PROCESO,SP_RS232);                       //
      show_registros_USART_by_LCD_Y_PC();                                     //
   }                                                                          //
                                                                              //
 #endif                                                                       //
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//Proposito : esta funcion tiene como finalidad la de configurar las          //
//            interrupciones.                                                 //
// Input  : Ninguno                                                           //
// Output : Ninguno                                                           //
  void funcion_configurar_interrupciones(void){                               //
    enable_interrupts(GLOBAL);                                                //
    enable_interrupts(INT_EXT);                                               //
    ext_int_edge(H_TO_L);                                                     //
 }                                                                            //
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//Proposito: siempre se envia esta cadena 20 nulls cuando se inicia la        //
//           comunicacion con el SitePlayer.                                  //
// Input :Ninguno                                                             //
// Output:Ninguno                                                             //                         
  void funcion_NOP(void){                                                     //
     int8 i;                                                                  //
      for(i=0;i<20;i++){                                                      //
        fputc(0,SP_RS232);                                                    //  
      }                                                                       //
        delay_ms(50);                                                         //
                                                                              //
    #if (MODO_DE_TRABAJO == PRUEBA_PC)                                        //
      fprintf(PC,"20 NOP enviados\r\n\r\n");                                  //
    #endif                                                                    //
                                                                              //
  }                                                                           //
////////////////////////////////////////////////////////////////////////////////
//6666666666666666666666666666666666666666666666666666666666666666666666666666//



////////////////////////////////////////////////////////////////////////////////
#define INTERRUPT_SIGNAL       PIN_A0   // ON cuando comienza la interrupcion
                                        // OFF cuando termina la interrupcion
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////    Interrupcion del SitePlayer al PIC MODBUS MASTER     ///////////
#INT_EXT                                                                      //
  void funcion_IRQ_BUFFER(void){                                              //
       int8 submit;                                                           //
//                                                                            //       
//  Activar el led que indica que la interrupcion ha comenzado.               //    
    output_bit( INTERRUPT_SIGNAL, 1);                                         //
//                                                                            //
//  Generacion del comando de lectura del SitePlayer                          //
    funcion_send_command_IRQ_BUFFER(LENGTH_IRQ_BUFFER,INICIAL_ADDRESS_SP_IRQ_BUFFER);
//                                                                            //
//  Consultamos si se han recibido todos los registros del SitePlayer de      //
//  forma satisfactoria.                                                      //
     if(funcion_IRQ_BUFFER_OK() == 1){                                        //
                                                                              //
        //Solo para pruebas                                                   //
        #if (MODO_DE_TRABAJO == PRUEBA_PC)                                    //
             funcion_show_IRQ_BUFFER();                                       //
        #endif                                                                //
                                                                              //
        submit = funcion_check_IRQ_BUFFER();                                  //
        reset_status_byte_check_error();                                      //
                                                                              //
         switch (submit){                                                     //
            case 0b00000001 :                                                 //
                #if (MODO_DE_TRABAJO == PRUEBA_PC)                            //
                fprintf(PC,"El formulario es Submit Nodo - %u\r\n",submit);   //
                #endif                                                        //
                funcion_submit_nodo_MB();                                     //
                break;                                                        //
            case 0b00000010 :                                                 //
                #if (MODO_DE_TRABAJO == PRUEBA_PC)                            //
                fprintf(PC,"El formulario es  Registros MB - %u\r\n",submit); //
                #endif                                                        //
                funcion_submit_regristros_MB();                               //
                break;                                                        //
            case 0b00000100 :                                                 //
                #if (MODO_DE_TRABAJO == PRUEBA_PC)                            //
                fprintf(PC,"El formulario es Valor Variable - %u\r\n",submit);//
                #endif                                                        //
                funcion_submit_valor_variable();                              //
                break;                                                        //
            default :                                                         //
                #if (MODO_DE_TRABAJO == PRUEBA_PC)                            //
                fprintf(PC,"El numero no corresponde con ningun submit \r\n");//
                #endif                                                        //
                break;                                                        //
              }                                                               //
          }                                                                   //
      else{                                                                   //
         #if (MODO_DE_TRABAJO == PRUEBA_PC)                                   //
         fprintf(PC,"Los datos no fueron recibidos correctamente\r\n");       //
         fprintf(PC,"Asegurece de sincronizar los baudiajes\r\n");            //
         #endif                                                               //
         reset_status_byte_check_error();                                     //
             }                                                                //
//                                                                            //       
//  Desactivar el led que indica que la interrupcion ha terminado.            //    
    output_bit( INTERRUPT_SIGNAL, 0);                                         //
//                                                                            //
     }                                                                        //
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////






 void main(void){
  
  //output_high(PIN_D0);
  
  // Este delay es para esperar que el Siteplayer Inicializa
     restart_wdt();delay_ms(1000);
     restart_wdt();delay_ms(1000);
     restart_wdt();delay_ms(1000);
   
   //Configuracion de las interrupciones
   funcion_configurar_interrupciones();
   
   //Envio de 20 nulls.
   funcion_NOP();
   
   //Modificar el Baudiaje(si es que es posible) del PIC MODBUS MASTER y SP.  
   #if (TX_RX_SITEPALYER_RS232_MODO == HARDWARE)     
      funcion_ADAPTAR_BAUD_SP_Y_PIC();
   #endif
   
   //Visualizar los datos obtenidos de la BD.
   funcion_show_data_from_BD_to_LCD_PC();
   
   #if (MODO_DE_TRABAJO == COMANDOS_MODBUS) 
        fprintf(PC,"Actualmente esta en el modo COMANDOS_MODBUS\r\n"); 
        fprintf(PC,"Si quiere ver valores atraves del Hiperterminal  \r\n"); 
        fprintf(PC,"cambie al modo PRUEBA_PC  \r\n\r\n\r\n");                   
   #endif
   
   
   #fuses WDT                                                                  
   setup_wdt(WDT_2304MS);
   
   switch ( restart_cause() ) {
   case NORMAL_POWER_UP:
     output_bit( PIN_D0, 1);
   break;
   case WDT_TIMEOUT:
     output_bit( PIN_D7, 1);
   break;
  }
  
     while(1){
         restart_wdt();
         ramdon_value_to_MBMA00X();
         funcion_cmd_read_MB_write_SP();
         //funcion_comando_LCD();
      }
   }
