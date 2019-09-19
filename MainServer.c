/*
  MainServer teste
  Gemap via FlagSystem
  Arquivo LogEscrita.log habilitado

  10.
  Correção Alarmes Datalog.
  Taxa de atualização do Datalog.
  Melhoria no rotina LeArquivoEscrita, proíbe escrita com tamanho 0.
  Exibe temperatura do core no Ethernet

  11.
  Atualização do Datalog a cada descarregamento.

  12.
  Limite de 500 registros no Datalog.
  Ajustes no tempo de atualização do Datalog.

  13.
  Thread para criar arquivo GEMAP.

  14.
  Correção no Datalog, durante o download
  não registrava.
  Adicionado "\r\n" no arquivo-historico.txt

  15.??

  16.
  Correção nas configurações de IP e Gateway.

  17.
  Melhoria na função Ipdevice.
  Adicionado arquivo backup do IP.
  Adicionado arquivo de log das alterações de IP.
  Tratamento do Gateway com valor incorreto.

  18.
  Adicionado controle de timeout para serial.

  19.
  Comentada chamada da função Ipdevice.

  20.
  Ipdevice é chamada apenas no inicio.
  A alteração de IP deve ser via arquivo ip.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/un.h>
#include <signal.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <byteswap.h>
#include <math.h>
#include <errno.h>

#include "Serial.h"
#include "SysConfig.h"
#include "Strings_GEMAP.h"
#include "Tabelas_GEMAP.h"
#include "Listas_Modbus.h"
#include "CRC16.h"

#define MODBUS_BUFFER_SIZE 256
#define TEMPERATURE_FILE "/sys/devices/virtual/thermal/thermal_zone0/temp"

//#define DEBUG_MODE

/////////////////////////////////////////////////////////////////////////////////////
///////////////////////////    System Type    ///////////////////////////////////////
//#define SDM_F30_60
//#define SDM_F120
#define SDM_MTM
/////////////////////////////////////////////////////////////////////////////////////

#define MAX_DLOG_HIST  500

#define NO_ERROR			0
#define SERIAL_NOT_FOUND	1

#define	DOWNLOAD	0
#define UPDATE		1

#define swVersion	"1.0.27"
#define swData		__DATE__
#define swTime		__TIME__

#define GEMAP_TIM			0l
#define HISTORICO_TIM		1
#define ESCRITA_TIM			2
#define MODBUSCLIENT_TIM	3
#define SERIAL_CHECK_TIM	4

//#define SET_DEBUG_TXT

#define MAX_IR_LIST_SIZE   	20
#define MAX_IR_LIST_LIMIT	(MAX_IR_LIST_SIZE-1)

#define MAX_HR_LIST_SIZE   	10
#define MAX_HR_LIST_LIMIT	(MAX_HR_LIST_SIZE-1)

#define TIMEOUT_REBOOT_ENABLE

#ifndef TIMEOUT_REBOOT_ENABLE
#warning "Reboot strategy disabled"
#endif

static unsigned char UnixPath[30] = "/tmp/modbus_socket";

static unsigned long int SerialTXCounter;
static unsigned long int SerialRXCounter;

static unsigned long int SerialTXMonitor;
static unsigned int CommPortFail;
static unsigned int Fault_COMM;

static unsigned char KeepAliveComm;
static unsigned int CommLoopFail;
static unsigned int ErrCommCounter;
static unsigned int MainServerLock;

unsigned char *SendPtr;

static unsigned int SendAddr;
static unsigned int CommTimeout;

static unsigned char IntToogle;

static unsigned char ReceiveAuxBuffer[MODBUS_BUFFER_SIZE];

static unsigned char IRSerialBuffer[MAX_IR_LIST_SIZE][MODBUS_BUFFER_SIZE];
static unsigned int IRStartAddr[MAX_IR_LIST_SIZE];
static unsigned int IRCount[MAX_IR_LIST_SIZE];
static unsigned int IRListSize;
static unsigned int IRAskSeq;
static unsigned int IRAskSeq2;
static unsigned char IRColum;
static unsigned int SerialRXListIR[MAX_IR_LIST_SIZE];
static unsigned int SerialTXListIR[MAX_IR_LIST_SIZE];

static unsigned char ToogleFlag;
static unsigned char ToogleFlag_HR;

static int SerialPort;

static unsigned char HRSerialBuffer[MAX_HR_LIST_SIZE][MODBUS_BUFFER_SIZE];
static unsigned int HRStartAddr[MAX_HR_LIST_SIZE];
static unsigned int HRCount[MAX_HR_LIST_SIZE];
static unsigned int HRListSize;
static unsigned int HRAskSeq;
static unsigned char HRColum;
static unsigned char HRPendent;
static unsigned int SerialRXListHR[MAX_HR_LIST_SIZE];
static unsigned int SerialTXListHR[MAX_HR_LIST_SIZE];

char *DataPtr;
unsigned char *AnsBufferPtr;

static unsigned char ComPath[30];
static unsigned long int BaudRate;

static unsigned char IRRespStatus;
static unsigned char HRRespStatus;
static unsigned char IRRespIdx;
static unsigned char HRRespIdx;

static unsigned char ModbusAddr;

static pthread_t chld_thr;
static pthread_t watchdog_thr;
static pthread_t gemapc_thr;

//File Variables
static unsigned int PointerDLogTOTAL,PointerDLog,PointerDLogMC,PointerDLogMC_old,PointerDLogREAD,PointerDLogWRITE;

unsigned int addr_req;

unsigned int count_req;

unsigned int flag_ReadReg_req;

unsigned int flag_ReadReg_ok;

unsigned int p1_buff;
unsigned char *ptr_buffer_local; // Alocar memória aqui!!!!
static unsigned char buffer_Modbus_Erro[10];

static FILE *LogIP;

static unsigned char LastIP[4] = 		{0, 0, 0, 0};
static unsigned char LastNetmask[4] = 	{0, 0, 0, 0};
static unsigned char LastGateway[4] = 	{0, 0, 0, 0};

static unsigned short UnidFlow,UnidTemp,UnidDens,UnidVol;
static unsigned short UnidLogFlow,UnidLogTemp,UnidLogDens,UnidLogVol;

//toradex -> MC
static int ip_toradex[4];
static int gateway_toradex[4];
static int mask_toradex[4];

typedef union b4
{
    unsigned long int LWData;
    unsigned int WData[2];
    unsigned char BData[4];
    float FData;
} BYTE4;

typedef union b8
{
    double DData;
    float FData[2];
    unsigned long int LWData[2];
    unsigned int WData[4];
    unsigned char BData[8];
} BYTE8;

// contadores de tempo
static time_t t_inicio;

// Timers
static unsigned int timer_end[10];

void StartTimeout(unsigned int timer_id, unsigned int timer_val);
unsigned int CheckTimeout(unsigned int timer_id, unsigned int restart_val);

void TrataASCIIChar(unsigned char *ptr_table, unsigned int size);
//uint32_t swap_uint32(uint32_t val);
int Ipdevice(char rw, char *wip, char *wmask, char *wgateway);
void LeArquivoEscrita(void);
void CriaArquivoGEMAP(unsigned char erro_id);
void CriaArquivoGEMAP_MTM(unsigned char erro_id);
void CriaArquivoHistorico(unsigned int tipo, unsigned int limpa_datalog);
void *ExecSerialFunc(void *arg);
void dostuff(int); /* function prototype */
void error(const char *);
float GetTemperatureGateway(void);
void GetUnit(FILE *ArqTemp, UnitTypes tipo);
char conta_bit (unsigned char num, int bit); //netmask
void CalBcast(char *wip, char *wmask, char *bcast); //bcast

void *SerialControl(void *arg);
void *GEMAPControl(void *arg);

int main(int argc, char *argv[])
{
  /**************************
       Variables Declaration
   **************************/

  // Unix socket
  int UNIX_sockfd, UNIX_servlen;
  socklen_t UNIX_clilen;
  struct sockaddr_un  UNIX_cli_addr, UNIX_serv_addr;

  // File Manipulation
  FILE *fpCFG, *fd_escrita;
  unsigned char LineCtl;
  unsigned char LineBuffer[250];
  unsigned char c;
  unsigned int LineCnt;
  unsigned char Cnt;
  unsigned char HandlingTableFlags[5];
  unsigned char i;

  // Aux Variables
  unsigned int wAuxValue;

  // First thing is create the temporary drive
  system("mkdir -p -m 777 /tmp/VirtualRAM/");

  //inicialização
  PointerDLogTOTAL = PointerDLog = PointerDLogREAD = PointerDLogMC = PointerDLogWRITE = PointerDLogMC_old = 0;

  if(argc > 1)
  {
    if(*argv[1] == '-')
    {
      switch(*(argv[1]+1))
      {
      case 'v':
      case 'V':
        printf("%s - %s %s", swVersion, swData, swTime);
        exit(0);

      default:
      break;
      }
    }
    else
      printf("Invalid Input argument! Continue loading System... \n");
  }

  // Wait init Linux
  //sleep(10);/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// MUDAR SLEEP

  //read from file Ip.txt
  Ipdevice(1,0,0,0);

  system("chmod 777 -R /tmp");
  system("chmod 777 -R /opt");

  /* Cria arquivo escrita vazio */
  fd_escrita = fopen("/tmp/VirtualRAM/arquivo-escrita.js", "w");
  fclose(fd_escrita);
  system("chmod 777 /tmp/VirtualRAM/arquivo-escrita.js");
  system("chmod 777 /www/pages/arquivo-escrita.js");

  /* Pega hora do incio do MainServer */
  t_inicio = time(NULL);

  if((fpCFG = 1 ? fopen("/opt/SDM/Execute/SDM.cfg", "r") : stdin) == NULL)
  {
    printf("\n *** The file SDM.cfg was not found.                          ***");
    printf("\n *** Please, check if the file exist or if the path is right. ***\n");
    //getchar();
    exit(-1);
  }
  else
  {
    rewind(fpCFG);
    c = getc(fpCFG);
    rewind(fpCFG);
    LineCtl = 0;
    LineCnt = 0;

    IRListSize = 0;          // Provisorio para inicialização
    HRListSize = 0;          // Provisorio para inicialização
    CommTimeout = 10;
    CommLoopFail = 50;
    memset(HandlingTableFlags,0,sizeof(HandlingTableFlags));

    while(!feof(fpCFG))
    {
      c = getc(fpCFG);
      while(c == '\0')
        c = getc(fpCFG);
      if(c == '#')
        LineCtl = 2;
      else if((c != '\n') && (LineCtl != 2))
      {
        LineCtl = 1;
        fseek(fpCFG,(ftell(fpCFG)-1),0);
        fscanf(fpCFG,"%s",LineBuffer);

        if(HandlingTableFlags[0] == 2)
        {
          if(IRColum == 0)
          {
            IRStartAddr[IRListSize] = atoi((const char *)&LineBuffer[0]);
            IRColum++;
          }
          else
            IRCount[IRListSize] = atoi((const char *)&LineBuffer[0]);
        }
        else if(HandlingTableFlags[1] == 2)
        {
          if(HRColum == 0)
          {
            HRStartAddr[HRListSize] = atoi((const char *)&LineBuffer[0]);
            HRColum++;
          }
          else
            HRCount[HRListSize] = atoi((const char *)&LineBuffer[0]);
        }
        else if(HandlingTableFlags[2] == 2)
        {
          strcpy((char *)&ComPath[0],(char *)&LineBuffer[0]);
          HandlingTableFlags[2] = 3;
        }
        else if(HandlingTableFlags[2] == 3)
        {
          BaudRate = atoi((const char *)&LineBuffer[0]);
          HandlingTableFlags[2] = 4;
        }
        else if(HandlingTableFlags[2] == 4)
        {
          CommTimeout = atoi((const char *)&LineBuffer[0]);
          HandlingTableFlags[2] = 5;
        }
        else if(HandlingTableFlags[2] == 5)
        {
          HandlingTableFlags[2] = 6;
        }
        else if(HandlingTableFlags[3] == 2)
        {
          HandlingTableFlags[3] = 3;
        }

        if((strcmp((char *)&LineBuffer[0],"<IR_TABLE>") == 0) && (HandlingTableFlags[0] == 0))
          HandlingTableFlags[0] = 1;
        else if((strcmp((char *)&LineBuffer[0],"</IR_TABLE>") == 0) && (HandlingTableFlags[0] == 2))
          HandlingTableFlags[0] = 0;

        if((strcmp((char *)&LineBuffer[0],"<HR_TABLE>") == 0) && (HandlingTableFlags[1] == 0))
          HandlingTableFlags[1] = 1;
        else if((strcmp((char *)&LineBuffer[0],"</HR_TABLE>") == 0) && (HandlingTableFlags[1] == 2))
          HandlingTableFlags[1] = 0;

        if((strcmp((char *)&LineBuffer[0],"<SERIAL_DATA_PORT>") == 0) && (HandlingTableFlags[2] == 0))
          HandlingTableFlags[2] = 1;
        else if((strcmp((char *)&LineBuffer[0],"</SERIAL_DATA_PORT>") == 0) && (HandlingTableFlags[2] > 1))
          HandlingTableFlags[2] = 0;

        if((strcmp((char *)&LineBuffer[0],"<UNIX_SOCKET>") == 0) && (HandlingTableFlags[3] == 0))
          HandlingTableFlags[3] = 1;
        else if((strcmp((char *)&LineBuffer[0],"</UNIX_SOCKET>") == 0) && (HandlingTableFlags[3] == 2))
          HandlingTableFlags[3] = 0;

      }
      else if(c== '\n')
      {
        if(LineCtl == 1)
          LineCnt++;

        if(HandlingTableFlags[0] == 1)
        {
          HandlingTableFlags[0] = 2;
          IRColum = 0;
        }
        else if(HandlingTableFlags[0] == 2)
        {
          if(LineCtl == 1)
          {
            if((IRListSize < MAX_IR_LIST_LIMIT))
              IRListSize++;
            else
              IRListSize = 0;
          }
          IRColum = 0;
        }

        if(HandlingTableFlags[1] == 1)
        {
          HandlingTableFlags[1] = 2;
          HRColum = 0;
        }
        else if(HandlingTableFlags[1] == 2)
        {
          if(LineCtl == 1)
          {
            if((HRListSize < MAX_HR_LIST_LIMIT))
              HRListSize++;
            else
              HRListSize = 0;
          }
          HRColum = 0;
        }
        if(HandlingTableFlags[2] == 1)
          HandlingTableFlags[2] = 2;

        if(HandlingTableFlags[3] == 1)
          HandlingTableFlags[3] = 2;
        LineCtl = 0;
      }
    }
    fclose(fpCFG);
  }

  if((fpCFG = 1 ? fopen("/tmp/VirtualRAM/MainServerVersion","w") : stdin) == NULL)
  {

  }
  else
  {
    fprintf(fpCFG,"%s\n",swVersion);
    fclose(fpCFG);
  }


  if((fpCFG = 1 ? fopen("/tmp/VirtualRAM/MasterServer.log", "w") : stdin) == NULL)
  {

  }

  ModbusAddr = 0x01;

  printf("\n*********************************************\n");
  printf("\n\n#Loading Configuration File...");
  printf("\n#Total of lines: %i\n\n\n",LineCnt);
  fprintf(fpCFG,"\n#Total of lines: %i\n\n\n",LineCnt);

  printf("%s - porta serial ", ComPath);



  /*** Serial Port ***/
  while((SerialPort = OpenSerial(ComPath,BaudRate)) == -1)
  {

    printf("Failed to open Serial Port...\n");

#ifdef SDM_MTM
    CriaArquivoGEMAP_MTM(SERIAL_NOT_FOUND);
#else
    CriaArquivoGEMAP(SERIAL_NOT_FOUND);
#endif

    sleep(1);

    //return 1;
  }
  //else
  printf("Opening Serial Port...\n");

  printf("\nSERIAL DEVICE PATH: %s\n",ComPath);
  fprintf(fpCFG,"\nSERIAL DEVICE PATH: %s\n",ComPath);
  printf("SERIAL BAUDRATE: %ld\n",BaudRate);
  fprintf(fpCFG,"SERIAL BAUDRATE: %ld\n",BaudRate);
  printf("SERIAL TIMEOUT: %d\n",CommTimeout);
  fprintf(fpCFG,"SERIAL TIMEOUT: %d\n",CommTimeout);
  printf("SERIAL KEEP ALIVE: %d\n\n",CommLoopFail);
  fprintf(fpCFG,"SERIAL KEEP ALIVE: %d\n\n",CommLoopFail);


  /* Debug */
  //printf("Print commport %d\n", SerialPort);
  //write(SerialPort,teste_serial,3);

  /*** Unix Socket ***/
  printf("\nUNIX SOCKET PATH: %s\n",UnixPath);
  fprintf(fpCFG,"\nUNIX SOCKET PATH: %s\n",UnixPath);


  /*** Input Register Table ***/
  //printf("\nINPUT REGISTERS TABLE (%d)\n",IRListSize);
  fprintf(fpCFG,"\nINPUT REGISTERS TABLE (%d)\n",IRListSize);
  for(Cnt=0; Cnt<IRListSize; Cnt++)
  {
    //printf("#%d - %d %d \n",Cnt, IRStartAddr[Cnt],IRCount[Cnt]);
    fprintf(fpCFG,"#%d - %d %d \n",Cnt, IRStartAddr[Cnt],IRCount[Cnt]);
  }


  /*** Holding Register Table ***/
  //printf("\nHOLDING REGISTERS TABLE (%d)\n",HRListSize);
  fprintf(fpCFG,"\nHOLDING REGISTERS TABLE (%d)\n",HRListSize);
  for(Cnt=0; Cnt<HRListSize; Cnt++)
  {
    //printf("#%d - %d %d \n",Cnt, HRStartAddr[Cnt],HRCount[Cnt]);
    wAuxValue = 0x0001;
    wAuxValue <<= Cnt;
    HRPendent |= wAuxValue;
  }
  //printf("*********************************************\n\n");
  //fclose(fpCFG);

  CommTimeout /= 10;        // The reference is 10ms

  /*** Starting Parametrization ***/
  LogIP = fopen("/opt/SDM/Execute/logip.txt","a");
  fprintf(LogIP, "Set MC -> IP , Netmask and Gateway\n");
  printf("\n\n-----------Write MC -> IP , Netmask and Gateway\n");


  for(i=0; i<4; i++)
  {
    ReceiveAuxBuffer[i]= ip_toradex[i];
    ReceiveAuxBuffer[i+16]= mask_toradex[i];
    ReceiveAuxBuffer[i+32]= gateway_toradex[i];

    LastIP[i]= ip_toradex[i];
    LastNetmask[i]= mask_toradex[i];
    LastGateway[i]= gateway_toradex[i];
  }

  //memcpy(&ReceiveAuxBuffer[0],GetSysIP(),4);
  memset(&ReceiveAuxBuffer[4],0,12);
  printf("IP: %d.%d.%d.%d\n",ReceiveAuxBuffer[0],ReceiveAuxBuffer[1],ReceiveAuxBuffer[2],ReceiveAuxBuffer[3]);
  fprintf(LogIP, "IP: %d.%d.%d.%d\n",ReceiveAuxBuffer[0],ReceiveAuxBuffer[1],ReceiveAuxBuffer[2],ReceiveAuxBuffer[3]);
  //memcpy(&ReceiveAuxBuffer[16],GetSysNetMask(),4);
  memset(&ReceiveAuxBuffer[20],0,12);
  printf("Netmask: %d.%d.%d.%d\n",ReceiveAuxBuffer[16],ReceiveAuxBuffer[17],ReceiveAuxBuffer[18],ReceiveAuxBuffer[19]);
  fprintf(LogIP, "Netmask: %d.%d.%d.%d\n",ReceiveAuxBuffer[16],ReceiveAuxBuffer[17],ReceiveAuxBuffer[18],ReceiveAuxBuffer[19]);
  //memcpy(&ReceiveAuxBuffer[32],GetSysGateway(),4);
  memset(&ReceiveAuxBuffer[36],0,12);
  printf("Gateway: %d.%d.%d.%d\n",ReceiveAuxBuffer[32],ReceiveAuxBuffer[33],ReceiveAuxBuffer[34],ReceiveAuxBuffer[35]);
  fprintf(LogIP, "Gateway: %d.%d.%d.%d\n",ReceiveAuxBuffer[32],ReceiveAuxBuffer[33],ReceiveAuxBuffer[34],ReceiveAuxBuffer[35]);

  PressetMultipleRegisters(SerialPort,1,700,48,ReceiveAuxBuffer);

  fclose(LogIP);
  printf("----------------------------- END write MC\n\n\n");

  /*** Socket UNIX - server ***/
  printf("Starting UNIX Socket...\n");
  if ((UNIX_sockfd = socket(AF_UNIX,SOCK_STREAM,0)) < 0)
    error("Error to creating UNIX socket");
  memset((char *)&UNIX_serv_addr,0,sizeof(UNIX_serv_addr));
  UNIX_serv_addr.sun_family = AF_UNIX;
  strcpy((char *)UNIX_serv_addr.sun_path, (char *)UnixPath);
  UNIX_servlen=strlen(UNIX_serv_addr.sun_path) + sizeof(UNIX_serv_addr.sun_family);
  unlink(UNIX_serv_addr.sun_path);
  if(bind(UNIX_sockfd,(struct sockaddr *)&UNIX_serv_addr,UNIX_servlen)<0)
    error("ERROR on binding UNIX socket");
  if(listen(UNIX_sockfd,5)<0)
    fprintf(fpCFG,"Fail to listen...\n");


  fclose(fpCFG);
  UNIX_clilen = sizeof(UNIX_cli_addr);

  signal(SIGCHLD,SIG_IGN);            // It avoids Zumbi process
  signal(SIGPIPE,SIG_IGN);

  IRAskSeq = 0;
  IRAskSeq2 = 0;
  HRAskSeq = 0;

  AnsBufferPtr = malloc(MODBUS_BUFFER_SIZE);
  DataPtr = malloc(MODBUS_BUFFER_SIZE);
  SendPtr = malloc(MODBUS_BUFFER_SIZE);
  ptr_buffer_local = malloc(MODBUS_BUFFER_SIZE);

  flag_ReadReg_req = 0;
  flag_ReadReg_ok = 0;
  addr_req = 0;
  count_req = 0;
  *DataPtr = 0;
  *AnsBufferPtr = 0;

  pthread_create(&chld_thr,NULL,ExecSerialFunc,NULL);
  pthread_create(&watchdog_thr,NULL,SerialControl,NULL);
  pthread_create(&gemapc_thr,NULL,GEMAPControl,NULL);

  system("chmod -R 777 /tmp/VirtualRAM/");

  printf("\n\nINICIADO MainServer\n\n");///////////////////////////////////////////////////////////////////////////////////////////////////////////////

  while(1)
  {
    int s = accept(UNIX_sockfd,(struct sockaddr *) &UNIX_cli_addr, &UNIX_clilen);
    dostuff(s);
    close(s);
  }
  return 0;
}

void *SerialControl(void *arg)
{
  (void)(arg);
  static unsigned long int SerialRX_old;

  while(1)
  {
    sleep(60);
    if (SerialRX_old == SerialRXCounter)
    {
      Fault_COMM++;
    }
    else if (SerialRXCounter > SerialRX_old)
    {
      SerialRX_old = SerialRXCounter;
    }

    if (Fault_COMM > 5)
    {
#ifdef SDM_MTM
      CriaArquivoGEMAP_MTM(SERIAL_NOT_FOUND);
#else
      CriaArquivoGEMAP(SERIAL_NOT_FOUND);
#endif
    }
  }

  return NULL;
}

void *GEMAPControl(void *arg)
{
  (void)(arg);
  static unsigned long int SerialRX_old;
  static unsigned int FlagTimeout = 0;

  while(1)
  {
    if (Fault_COMM <= 5)
    {
      usleep(900000);
#ifdef SDM_MTM
      CriaArquivoGEMAP_MTM(NO_ERROR);
#else
      CriaArquivoGEMAP(NO_ERROR);
#endif
    }

    if (FlagTimeout == 0)
    {
      FlagTimeout = 1;
      SerialRX_old = SerialRXCounter;
      StartTimeout(SERIAL_CHECK_TIM, 60000);
    }
    else
    {
#ifdef TIMEOUT_REBOOT_ENABLE
      if (CheckTimeout(SERIAL_CHECK_TIM, 60000))
      {
        if (SerialRX_old == SerialRXCounter)
        {
          /* Error */
          system("reboot");
        }
        else
        {
          /* Normal condition */
          SerialRX_old = SerialRXCounter;
        }
      }
#endif
    }

  }

  return NULL;
}

void *ExecSerialFunc(void *arg)
{
  (void)arg;
  unsigned int wAuxValue = 0, wAuxValue2 = 0;

  static unsigned char IP_Config_Changed = 0;

  unsigned long int lwAuxValue = 0;
  BYTE4 TempData;
  FILE *fpDBG_COM_DIAG;
  //static unsigned int t_cnt;
  FILE *fd2;
  static unsigned int TimeoutDatalog;

  TimeoutDatalog = 1500;

  StartTimeout(GEMAP_TIM, 1000);
  StartTimeout(HISTORICO_TIM, TimeoutDatalog);
  StartTimeout(ESCRITA_TIM, 2000);
  StartTimeout(MODBUSCLIENT_TIM, 500);

  while(1)
  {
    if(SerialTXCounter > 1000000000)
    {
      SerialTXCounter = 0;
      SerialRXCounter = 0;
    }

    if((fpDBG_COM_DIAG = 1 ? fopen("/tmp/VirtualRAM/MainServerDiag","w") : stdin) == NULL)
    {
      printf("\n\n *** ERROR creation ==> /tmp/VirtualRAM/MainServerDiag  ***\n\n");
    }
    else
    {
      rewind(fpDBG_COM_DIAG);
      fprintf(fpDBG_COM_DIAG,"%s %ld %s\n\n","<TX>",SerialTXCounter,"</TX>");
      fprintf(fpDBG_COM_DIAG,"%s %ld %s\n\n","<RX>",SerialRXCounter,"</RX>");
      fclose(fpDBG_COM_DIAG);
    }

    //printf("\n*DataPtr %d - ToogleFlag_HR %d \n", *DataPtr, ToogleFlag_HR);

    /*
    if ((IntToogle == 0) && (IRAskSeq2==0) && (IRStartAddr[LISTA_300] == 300))
    {
      clock_gettime(CLOCK_MONOTONIC, &start);
      diff = (unsigned int)((1000000L * start.tv_sec) + (start.tv_nsec/1000));
      printf("%u microseconds\n", diff);
    }
*/
    switch(*DataPtr)
    {
    case 0:
      //if((ToogleFlag == 1) && (HRPendent != 0))						// Leitura HR
      if (ToogleFlag_HR == 0)
      {
        //memset(&HRSerialBuffer[HRAskSeq][0],0,128);
        memset(&ReceiveAuxBuffer[0],0,MODBUS_BUFFER_SIZE);
        HRRespStatus = ReadHoldingRegister(SerialPort,1,HRStartAddr[HRAskSeq],HRCount[HRAskSeq],&ReceiveAuxBuffer[0]);
        SerialTXCounter++;
        SerialTXListHR[HRAskSeq]++;

        //printf("HR %d\n", HRStartAddr[HRAskSeq]);

        if((HRRespStatus == 0) && (!(ReceiveAuxBuffer[1] & 0x80)) && ((ReceiveAuxBuffer[2]+5) <= MODBUS_BUFFER_SIZE))
        {
          SerialRXCounter++;
          SerialRXListHR[HRAskSeq]++;
          memcpy(&HRSerialBuffer[HRAskSeq][0],&ReceiveAuxBuffer[0],(ReceiveAuxBuffer[2]+5));
        }
        else
        {
          ErrCommCounter++;
        }

        HRRespIdx = HRAskSeq;

        if(HRRespStatus == 0)
        {
          wAuxValue <<= HRAskSeq;
          HRPendent &= ~wAuxValue;

          if(++HRAskSeq >= HRListSize)
          {
            HRAskSeq = 0;
          }
        }
        ToogleFlag_HR = 1;
      }
      else																	// Leitura IR
      {
        ///memset(&IRSerialBuffer[IRAskSeq][0],0,128);
//        memset(&ReceiveAuxBuffer[0],0,128);
        if(IntToogle == 0)														// Priorizacao
        {
          if (IRAskSeq2==0)
          {
            IRRespStatus = ReadInputRegister(SerialPort,1,IRStartAddr[LISTA_300],IRCount[LISTA_300],&ReceiveAuxBuffer[0]);
            SerialTXCounter++;
            SerialTXListIR[LISTA_300]++;

            if((IRRespStatus == 0) && (!(ReceiveAuxBuffer[1] & 0x80)) && ((ReceiveAuxBuffer[2]+5) <= MODBUS_BUFFER_SIZE))
            {
              SerialRXCounter++;
              SerialRXListIR[LISTA_300]++;
              memcpy(&IRSerialBuffer[LISTA_300][0],&ReceiveAuxBuffer[0],(ReceiveAuxBuffer[2]+5));
            }
            else
            {
              ErrCommCounter++;
            }
          }
          else
          {
            IRRespStatus = ReadInputRegister(SerialPort,1,IRStartAddr[IRAskSeq2],IRCount[IRAskSeq2],&ReceiveAuxBuffer[0]);
            SerialTXCounter++;
            SerialTXListIR[IRAskSeq2]++;

            if((IRRespStatus == 0) && (!(ReceiveAuxBuffer[1] & 0x80)) && ((ReceiveAuxBuffer[2]+5) <= MODBUS_BUFFER_SIZE))
            {
              SerialRXCounter++;
              SerialRXListIR[IRAskSeq2]++;
              memcpy(&IRSerialBuffer[IRAskSeq2][0],&ReceiveAuxBuffer[0],(ReceiveAuxBuffer[2]+5));
            }
            else
            {
              ErrCommCounter++;
            }

            //printf("1- IR %d - %d\n", IRStartAddr[IRAskSeq2], IRCount[IRAskSeq2]);
          }

          IRRespIdx = IRAskSeq2;
          IRAskSeq2++;
          IRAskSeq2 &= 0x01;
          IntToogle = 1;
        }
        else
        {
          IRRespStatus = ReadInputRegister(SerialPort,1,IRStartAddr[IRAskSeq],IRCount[IRAskSeq],&ReceiveAuxBuffer[0]);
          SerialTXCounter++;
          SerialTXListIR[IRAskSeq]++;

          if((IRRespStatus == 0) && (!(ReceiveAuxBuffer[1] & 0x80)) && ((ReceiveAuxBuffer[2]+5) <= MODBUS_BUFFER_SIZE))
          {
            SerialRXCounter++;
            SerialRXListIR[IRAskSeq]++;

            memcpy(&IRSerialBuffer[IRAskSeq][0],&ReceiveAuxBuffer[0],(ReceiveAuxBuffer[2]+5));
            if((IRStartAddr[IRAskSeq] == 800) && (IRCount[IRAskSeq] == 16))
            {
              if(SerialTXCounter > 0)
              {
                lwAuxValue = SerialRXCounter;
                lwAuxValue *= 100;
                lwAuxValue /= SerialTXCounter;
              }
              wAuxValue = (unsigned int)lwAuxValue;
              TempData.WData[0] = wAuxValue;
              IRSerialBuffer[IRAskSeq][3] = TempData.BData[1];
              IRSerialBuffer[IRAskSeq][4] = TempData.BData[0];
              TempData.LWData = SerialTXCounter;
              IRSerialBuffer[IRAskSeq][5] = TempData.BData[3];
              IRSerialBuffer[IRAskSeq][6] = TempData.BData[2];
              IRSerialBuffer[IRAskSeq][7] = TempData.BData[1];
              IRSerialBuffer[IRAskSeq][8] = TempData.BData[0];
              TempData.LWData = SerialRXCounter;
              IRSerialBuffer[IRAskSeq][9] = TempData.BData[3];
              IRSerialBuffer[IRAskSeq][10] = TempData.BData[2];
              IRSerialBuffer[IRAskSeq][11] = TempData.BData[1];
              IRSerialBuffer[IRAskSeq][12] = TempData.BData[0];

              TempData.WData[0] = CRC16(&IRSerialBuffer[IRAskSeq][0],35);
              IRSerialBuffer[IRAskSeq][35] = TempData.BData[1];
              IRSerialBuffer[IRAskSeq][36] = TempData.BData[0];
            }
          }
          else
          {
            ErrCommCounter++;
          }

          IRRespIdx = IRAskSeq;
          IntToogle = 0;

          if(++IRAskSeq >= IRListSize)
          {
            IRAskSeq = 2;
            ToogleFlag_HR = 0;
          }

        }

        if((IRStartAddr[IRAskSeq] == 700) && (IRCount[IRAskSeq] == 24))
        {
          if ((IRSerialBuffer[IRAskSeq][3] != LastIP[0]) ||
              (IRSerialBuffer[IRAskSeq][4] != LastIP[1]) ||
              (IRSerialBuffer[IRAskSeq][5] != LastIP[2]) ||
              (IRSerialBuffer[IRAskSeq][6] != LastIP[3]))
          {
            IP_Config_Changed = 1;

          }

          if ((IRSerialBuffer[IRAskSeq][19] != LastNetmask[0]) ||
              (IRSerialBuffer[IRAskSeq][20] != LastNetmask[1]) ||
              (IRSerialBuffer[IRAskSeq][21] != LastNetmask[2]) ||
              (IRSerialBuffer[IRAskSeq][22] != LastNetmask[3]))
          {
            IP_Config_Changed = 1;
          }

          if ((IRSerialBuffer[IRAskSeq][35] != LastGateway[0]) ||
              (IRSerialBuffer[IRAskSeq][36] != LastGateway[1]) ||
              (IRSerialBuffer[IRAskSeq][37] != LastGateway[2]) ||
              (IRSerialBuffer[IRAskSeq][38] != LastGateway[3]))
          {
            IP_Config_Changed = 1;
          }

          if (IP_Config_Changed)
          {

            /* Validate IP and mask*/
            if(((IRSerialBuffer[IRAskSeq][3] == 0 )&& //IP
                (IRSerialBuffer[IRAskSeq][4]  == 0 )&&
                (IRSerialBuffer[IRAskSeq][5]  == 0 )&&
                (IRSerialBuffer[IRAskSeq][6]  == 0))||
               ((IRSerialBuffer[IRAskSeq][19]== 0 )&& //MASK
                (IRSerialBuffer[IRAskSeq][20] == 0 )&&
                (IRSerialBuffer[IRAskSeq][21] == 0 )&&
                (IRSerialBuffer[IRAskSeq][22] == 0)))
            {
              IP_Config_Changed = 0;
            }else{
              (void)memcpy(&LastIP[0], &IRSerialBuffer[IRAskSeq][3], 4);
              (void)memcpy(&LastNetmask[0], &IRSerialBuffer[IRAskSeq][19], 4);
              (void)memcpy(&LastGateway[0], &IRSerialBuffer[IRAskSeq][35], 4);


              IP_Config_Changed = 0;

              /* Write IP configuration */
              Ipdevice(2, (char*)&LastIP[0], (char*)&LastNetmask[0], (char*)&LastGateway[0]);

              ///*
              LogIP = fopen("/opt/SDM/Execute/logip.txt","a");
              fprintf(LogIP, "\nUpdate\n");
              fprintf(LogIP, "IP: %d.%d.%d.%d\n",LastIP[0],LastIP[1],LastIP[2],LastIP[3]);
              fprintf(LogIP, "Netmask: %d.%d.%d.%d\n",LastNetmask[0],LastNetmask[1],LastNetmask[2],LastNetmask[3]);
              fprintf(LogIP, "Gateway: %d.%d.%d.%d\n",LastGateway[0],LastGateway[1],LastGateway[2],LastGateway[3]);

              fclose(LogIP);
              //*/

              // Starting Parameterization
              memcpy(&ReceiveAuxBuffer[0],GetSysIP(),4);
              memset(&ReceiveAuxBuffer[4],0,12);
              printf("IP: %d.%d.%d.%d\n",ReceiveAuxBuffer[0],ReceiveAuxBuffer[1],ReceiveAuxBuffer[2],ReceiveAuxBuffer[3]);
              memcpy(&ReceiveAuxBuffer[16],GetSysNetMask(),4);
              memset(&ReceiveAuxBuffer[20],0,12);
              printf("Netmask: %d.%d.%d.%d\n",ReceiveAuxBuffer[16],ReceiveAuxBuffer[17],ReceiveAuxBuffer[18],ReceiveAuxBuffer[19]);
              memcpy(&ReceiveAuxBuffer[32],GetSysGateway(),4);
              memset(&ReceiveAuxBuffer[36],0,12);
              printf("Gateway: %d.%d.%d.%d\n",ReceiveAuxBuffer[32],ReceiveAuxBuffer[33],ReceiveAuxBuffer[34],ReceiveAuxBuffer[35]);
              PressetMultipleRegisters(SerialPort,1,700,48,ReceiveAuxBuffer);
            }
          } //if (IP_Config_Changed)

        }

      }

    break;

    case 1:
      SendAddr = (unsigned char)*(SendPtr+3);
      SendAddr <<= 8;
      SendAddr += (unsigned char)*(SendPtr+4);

      PressetSingleRegisters(SerialPort,(unsigned char)*(SendPtr+1),SendAddr,(unsigned char *)(SendPtr+5));

      fd2 = fopen("/tmp/VirtualRAM/LogEscrita.log", "a");
      fprintf(fd2, "\n%02x/%02x", IRSerialBuffer[LISTA_50][3], IRSerialBuffer[LISTA_50][4]);
      fprintf(fd2, "- %02x:%02x:%02x - ", IRSerialBuffer[LISTA_50][7], IRSerialBuffer[LISTA_50][8], IRSerialBuffer[LISTA_50][9]);
      fprintf(fd2, "Case 1 %d %d", SendAddr, (unsigned char)*(SendPtr+5));
      fclose(fd2);

      *SendPtr = 1;
      *DataPtr = 0;
    break;

    case 2:
      SendAddr = (unsigned char)*(SendPtr+3);
      SendAddr <<= 8;
      SendAddr += (unsigned char)*(SendPtr+4);

      fd2 = fopen("/tmp/VirtualRAM/LogEscrita.log", "a");
      fprintf(fd2, "\n%02x/%02x", IRSerialBuffer[LISTA_50][3], IRSerialBuffer[LISTA_50][4]);
      fprintf(fd2, "- %02x:%02x:%02x - ", IRSerialBuffer[LISTA_50][7], IRSerialBuffer[LISTA_50][8], IRSerialBuffer[LISTA_50][9]);
      fprintf(fd2, "Case 2 %d %d %d %d", SendAddr, (unsigned char)*(SendPtr+7), SendPtr[8], SendPtr[9]);
      fclose(fd2);


      PressetMultipleRegisters(SerialPort,(unsigned char)*(SendPtr+1),SendAddr,(unsigned char)*(SendPtr+7),
                               (unsigned char *)(SendPtr+8));

      *SendPtr = 1;
      *DataPtr = 0;
    break;

    case 3:
      //printf("\nCase 3\n");
      wAuxValue = (unsigned char)*(DataPtr+3);
      wAuxValue <<= 8;
      wAuxValue += (unsigned char)*(DataPtr+4);

      wAuxValue2 = (unsigned char)*(DataPtr+5);
      wAuxValue2 <<= 8;
      wAuxValue2 += (unsigned char)*(DataPtr+6);

      *(AnsBufferPtr+1) = wAuxValue2;

      IRRespStatus = ReadInputRegister(SerialPort,1,(unsigned int)wAuxValue,(unsigned int)*(AnsBufferPtr+1),
                                       (unsigned char *)&ReceiveAuxBuffer[0]);
      SerialTXCounter++;

      if((IRRespStatus == 0) && ((ReceiveAuxBuffer[2]+5) <= MODBUS_BUFFER_SIZE))
      {
        SerialRXCounter++;
        memcpy((AnsBufferPtr+2),&ReceiveAuxBuffer[0],(ReceiveAuxBuffer[2]+5));
      }

      *AnsBufferPtr = 2;
      *DataPtr = 0;
    break;

    case 4:
      wAuxValue = (unsigned char)*(DataPtr+3);
      wAuxValue <<= 8;
      wAuxValue += (unsigned char)*(DataPtr+4);

      wAuxValue2 = (unsigned char)*(DataPtr+5);
      wAuxValue2 <<= 8;
      wAuxValue2 += (unsigned char)*(DataPtr+6);

      *(AnsBufferPtr+1) = wAuxValue2;

      HRRespStatus = ReadHoldingRegister(SerialPort,1,(unsigned int)wAuxValue,(unsigned int)*(AnsBufferPtr+1),
                                         (unsigned char *)&ReceiveAuxBuffer[0]);
      SerialTXCounter++;

      if((HRRespStatus == 0) && ((ReceiveAuxBuffer[2]+5) <= MODBUS_BUFFER_SIZE))
      {
        SerialRXCounter++;
        memcpy((AnsBufferPtr+2),&ReceiveAuxBuffer[0],(ReceiveAuxBuffer[2]+5));
      }

      *AnsBufferPtr = 2;
      *DataPtr = 0;
    break;

    case 5:
      if (flag_ReadReg_req == 1)
      {
        IRRespStatus = ReadInputRegister(SerialPort,1,addr_req,count_req,ptr_buffer_local);
        flag_ReadReg_req = 0;

#ifdef DEBUG_MODE
        printf("\nIRRespStatus - %d\n", IRRespStatus);
#endif

        if (IRRespStatus == 0)
        {
          flag_ReadReg_ok = 1; //OK
        }
        else
        {
          flag_ReadReg_ok = 2; //ERRO
        }

      }

      if (flag_ReadReg_req == 2)
      {
        HRRespStatus = ReadHoldingRegister(SerialPort,1,addr_req,count_req,ptr_buffer_local);
        flag_ReadReg_req = 0;

#ifdef DEBUG_MODE
        printf("\nHRRespStatus - %d\n", HRRespStatus);
#endif

        if (HRRespStatus == 0)
        {
          flag_ReadReg_ok = 1; //OK
        }
        else
        {
          flag_ReadReg_ok = 2; //ERRO
        }
      }

      *DataPtr = 0;
    break;

    default:
      MainServerLock++;
    break;
    }

    if (CheckTimeout(ESCRITA_TIM, 1150))
    {
      ToogleFlag_HR = 0;
      LeArquivoEscrita();
      system("chmod 777 /tmp/VirtualRAM/arquivo-escrita.js");

      // Prioriza lista 45 (Liberação Modo Remoto)l
      HRRespStatus = ReadHoldingRegister(SerialPort,1,HRStartAddr[0],HRCount[0],&ReceiveAuxBuffer[0]);
      SerialTXCounter++;

      if((HRRespStatus == 0) && (!(ReceiveAuxBuffer[1] & 0x80)) && ((ReceiveAuxBuffer[2]+5) <= MODBUS_BUFFER_SIZE))
      {
        SerialRXCounter++;
        memcpy(&HRSerialBuffer[0][0],&ReceiveAuxBuffer[0],(ReceiveAuxBuffer[2]+5));
      }
      else
      {
        ErrCommCounter++;
      }

    }

    if (CheckTimeout(HISTORICO_TIM, TimeoutDatalog))
    {
      ToogleFlag = 1;

      PointerDLogMC = (IRSerialBuffer[LISTA_50][61]<<8) | IRSerialBuffer[LISTA_50][62];

      /* Caso o Ponteiro altere durante o download
       * então força a iniciar o download novamente
       */
      if ((PointerDLogMC != PointerDLogMC_old) && (PointerDLogTOTAL==0))
      {
        PointerDLog = 0;
        PointerDLogREAD = 0;
        PointerDLogWRITE = 0;
        PointerDLogMC_old = PointerDLogMC;
        CriaArquivoHistorico(DOWNLOAD, 1); /* Força a limpeza do arquivo */
      }

      /* Lê a memória totalmente */
      if(PointerDLogTOTAL==0)
      {
        if(PointerDLogREAD != PointerDLogMC)
        {
          PointerDLog = 0;
        }

        if(PointerDLog == 0)
        {
          PointerDLogREAD = PointerDLogWRITE = PointerDLogMC;
        }

        if(++PointerDLog <= MAX_DLOG_HIST)
        {
          CriaArquivoHistorico(DOWNLOAD, 0);
          if(PointerDLogWRITE==0)
          {
            PointerDLogWRITE = (MAX_DLOG_HIST-1);
          }
          else
          {
            PointerDLogWRITE--;
          }
        }
        else
        {
          TimeoutDatalog = 5000;
          PointerDLogTOTAL = 1;
          PointerDLogWRITE =PointerDLogMC;
        }
      }
      else
      {
        /* memória totalmente lida */
        if(PointerDLogWRITE != PointerDLogMC)
        {
          PointerDLogWRITE = PointerDLogMC;
          CriaArquivoHistorico(UPDATE, 0);
        }
      }
    }
    /*
     * Fim - Estrutura para Ethernet
     *
     */

    if(ErrCommCounter > CommLoopFail)
    {
      ErrCommCounter = 0;
      KeepAliveComm = 1;
    }

    if(KeepAliveComm == 2)
    {
      if(++CommPortFail > CommLoopFail)
      {
        MainServerLock++;
        KeepAliveComm = 0;
      }
      else
      {
        if(SerialTXCounter != SerialTXMonitor)
          KeepAliveComm = 0;
      }
    }
    else if(KeepAliveComm == 1)
    {
      CloseSerial(SerialPort);
      sleep(2);
      if((SerialPort = OpenSerial(ComPath,BaudRate)) == -1)
      {
        MainServerLock++;
        break;
      }

      KeepAliveComm = 2;
      SerialTXMonitor = SerialTXCounter;
      CommPortFail = 0;
    }
  }

  pthread_exit(0);
  return NULL;
}

void dostuff (int sock)
{
  int n;
  unsigned int StartAddr, Count;
  unsigned char DelayCounter;
  unsigned char InputBuffer[MODBUS_BUFFER_SIZE];
  BYTE4 TempData;
  unsigned int timeout_ms = 0;

  memset(InputBuffer,0,sizeof(InputBuffer));
  n = read(sock,InputBuffer,MODBUS_BUFFER_SIZE);
  if (n <= 0)
    return;


  if ((InputBuffer[1] == 0x04) || (InputBuffer[1] == 0x03))
  {
    StartAddr = ((unsigned short)(InputBuffer[2]) << 8) + InputBuffer[3];
    Count     = ((unsigned short)(InputBuffer[4]) << 8) + InputBuffer[5];

    addr_req = StartAddr;
    count_req = Count;
    *DataPtr = 5;				/* Força CASE 5 */

    if (InputBuffer[1] == 0x04)
    {
      flag_ReadReg_req = 1;	/* Case 5 com Input Register */
#ifdef DEBUG_MODE
      printf("READ INPUT - Addr %d - Count %02d\n", StartAddr, Count);
#endif
    }
    else if (InputBuffer[1] == 0x03)
    {
      flag_ReadReg_req = 2;	/* Case 5 com Holding Register */
#ifdef DEBUG_MODE
      printf("\nREAD HOLDING - Addr %d - Count %02d\n", StartAddr, Count);
#endif
    }

    flag_ReadReg_ok = 0;
    timeout_ms = 0;
    while(flag_ReadReg_ok == 0)
    {
      usleep(1000);
      timeout_ms++;
      if (timeout_ms > 10000)
      {
        flag_ReadReg_ok = 2;		//Retorna Erro
        break;
      }
    }
#ifdef DEBUG_MODE
    printf("Timeout Case 5 %dmS \n", timeout_ms);
#endif


    /* Retorne Erro */
    if (flag_ReadReg_ok == 2)
    {
      flag_ReadReg_ok = 0;

      memset(buffer_Modbus_Erro,0,sizeof(buffer_Modbus_Erro));

      buffer_Modbus_Erro[0] = 0x01;
      buffer_Modbus_Erro[1] = InputBuffer[1] | 0x80;
      buffer_Modbus_Erro[2] = 0x04;

      TempData.WData[0] = CRC16(&buffer_Modbus_Erro[0],3);
      buffer_Modbus_Erro[3] = TempData.BData[1];
      buffer_Modbus_Erro[4] = TempData.BData[0];

      (void)write(sock, &buffer_Modbus_Erro, 5);

#ifdef DEBUG_MODE
      printf("*********************************************************\n");
      for (t_cnt=0; t_cnt<7; t_cnt++)
      {
        printf("buffer_Modbus_Erro[%d] = %02x \n", t_cnt, buffer_Modbus_Erro[t_cnt]);
      }
#endif
    }

    /* Retorna dados lidos da MC */
    if (flag_ReadReg_ok == 1)
    {
      flag_ReadReg_ok = 0;

      n = write(sock,ptr_buffer_local,(ptr_buffer_local[2]+5));

#ifdef DEBUG_MODE
      printf("*********************************************************\n");
      for (t_cnt=0; t_cnt<30; t_cnt++)
      {
        printf("ptr_buffer_local[%d] = %02x \n", t_cnt, *ptr_buffer_local);
        ptr_buffer_local++;
      }
#endif
    }
  }
  else if(InputBuffer[1] == 0x06)
  {
    if((*DataPtr) == 0)
    {
      memcpy((SendPtr+1),&InputBuffer[0],8);
      *SendPtr = 0;
      *DataPtr = 1;

      DelayCounter = 0;
      if(CommTimeout > 2500)                     // It limits the receive delay in 5 seconds
        CommTimeout = 2500;

      if(CommTimeout == 0)                      // It limits the minimum receive delay in 10 miliseconds
        CommTimeout = 1;

      while(DelayCounter < CommTimeout)
      {
        if((*SendPtr) == 1)
        {
          n = write(sock,(SendPtr+5),8);
          *SendPtr = 0;
          break;
        }
        usleep(2000);
        DelayCounter++;
      }
    }
  }
  else if(InputBuffer[1] == 0x10)
  {
    if((*DataPtr) == 0)
    {
      if((InputBuffer[6]+9) <= MODBUS_BUFFER_SIZE)
        memcpy((SendPtr+1),&InputBuffer[0],(InputBuffer[6]+9));
      *SendPtr = 0;
      *DataPtr = 2;

      DelayCounter = 0;
      if(CommTimeout > 2500)                     // It limits the receive delay in 5 seconds
        CommTimeout = 2500;

      if(CommTimeout == 0)                      // It limits the minimum receive delay in 100 miliseconds
        CommTimeout = 1;

      while(DelayCounter < CommTimeout)
      {
        if((*SendPtr) == 1)
        {
          n = write(sock,(SendPtr+8),8);
          *SendPtr = 0;
          break;
        }
        usleep(2000);
        DelayCounter++;
      }
    }
  }

  usleep(1000);
}


void error(const char *msg)
{
  (void)msg;
  MainServerLock++;
  exit(0);
}

int Ipdevice(char rw, char *wip, char *wmask, char *wgateway)
{
  char txt_ip[] = {"IP <"};
  char txt_mask[] = {"MASK <"};
  char txt_gateway[] = {"GATEWAY <"};
  char txt_fim[] = {">\n"};

  FILE *fp;
  char *buffer;
  char ip[16];
  char mask[16];
  char gateway[16];
  char bcast[16];
  int i = 0;
  long lSize;
  char buffer_toradex[100];

  unsigned int ip_ini = 0, ip_fim = 0;
  unsigned int mask_ini = 0, mask_fim = 0;
  unsigned int gateway_ini = 0, gateway_fim = 0;


  int contabitmask;

  memset(&ip[0], 0x00, sizeof(ip));
  memset(&mask[0], 0x00, sizeof(mask));
  memset(&gateway[0], 0x00, sizeof(gateway));

  lSize = 0;

  switch (rw)
  {
  case 1: //READ
  {
    printf( "\nLeitura no arquivo IP\n");
    fp = fopen("/opt/SDM/Execute/ip.txt","r");

    if(!fp)
    {
      printf( "Erro na leitura do ip.txt");
      return 0;
    }
    else
    {
      // obtain file size:
      fseek(fp, 0, SEEK_END);
      lSize = ftell(fp);
      rewind(fp);

      /* If file is OK, create a copy */
      if (lSize == 0)
      {
        fclose(fp);
        printf("Use ip backup file\n");
        system("cp /opt/SDM/Execute/ip_copy.txt /opt/SDM/Execute/ip.txt");

        fp = fopen("/opt/SDM/Execute/ip.txt","r");
      }
      buffer = malloc(lSize+1);
      buffer[lSize-1] = 0;
      fread(buffer,lSize,1,fp);
      fclose(fp);

      for(i=0; i<lSize; i++)
      {
        if (buffer[i] == '<')
        {
          ip_ini = i;
        }
        if (buffer[i] == '>')
        {
          ip_fim = i;
          break;
        }
      }
      (void)memcpy(&ip[0], &buffer[ip_ini+1], ((ip_fim - ip_ini)-1));

      for(i=ip_fim+1; i<lSize; i++)
      {
        if (buffer[i] == '<')
        {
          mask_ini = i;
        }
        if (buffer[i] == '>')
        {
          mask_fim = i;
          break;
        }
      }
      memcpy(&mask[0], &buffer[mask_ini+1], ((mask_fim - mask_ini)-1));

      for(i=mask_fim+1; i<lSize; i++)
      {
        if (buffer[i] == '<')
        {
          gateway_ini = i;
        }
        if (buffer[i] == '>')
        {
          gateway_fim = i;
          break;
        }
      }
      memcpy(&gateway[0], &buffer[gateway_ini+1], ((gateway_fim - gateway_ini)-1));

      // Buffer is no longer needed
      free(buffer);

      printf("Ip : %s", ip);
      printf("\nMask: : %s", mask);
      printf("\nGateway : %s", gateway);

      LogIP = fopen("/opt/SDM/Execute/logip.txt","w");
      fprintf(LogIP, "\nPreset\n");

      if (ip[0] == '0')
      {
        fprintf(LogIP, "ERRO IP\n");
        break;
      }

      fprintf(LogIP, "ifconfig eth0 %s netmask %s \n", ip, mask);

      fprintf(LogIP, "route add default gw %s\n\n", gateway);

      fclose(LogIP);
    }
  }
  break;

  case 2: //WRITE
  {
    printf( "\nEscrita no arquivo IP\n");
    if (wip[0] == 0)
    {
      break;
    }

    sprintf(ip, "%d.%d.%d.%d", wip[0], wip[1], wip[2], wip[3]);
    sprintf(mask, "%d.%d.%d.%d", wmask[0], wmask[1], wmask[2], wmask[3]);
    sprintf(gateway, "%d.%d.%d.%d", wgateway[0], wgateway[1], wgateway[2], wgateway[3]);

    fp = fopen("/opt/SDM/Execute/ip.txt","w");

    if(!fp)
    {
      fclose(fp);
      printf( "Erro na escrita do ip.txt");
      return 0;
    }

    fprintf(fp, "%s",txt_ip);
    fprintf(fp, "%s",ip);
    fprintf(fp, "%s",txt_fim);

    fprintf(fp, "%s",txt_mask);
    fprintf(fp, "%s",mask);
    fprintf(fp, "%s",txt_fim);

    fprintf(fp, "%s",txt_gateway);
    fprintf(fp, "%s",gateway);
    fprintf(fp, "%s",txt_fim);

    fseek(fp, 0, SEEK_END);
    lSize = ftell(fp);
    rewind(fp);

    fclose(fp);

    /* If file is OK, create a copy */
    if (lSize > 0)
    {
      system("cp /opt/SDM/Execute/ip.txt /opt/SDM/Execute/ip_copy.txt");
      printf("Create ip backup file\n");
    }
  }
  break;
  }

  //------------------------------------------------------
  //Escrita no arquivo: /etc/systemd/network/wired.network (toradex)
  sscanf(ip,"%d.%d.%d.%d",&ip_toradex[0],&ip_toradex[1],&ip_toradex[2],&ip_toradex[3]);
  sscanf(gateway,"%d.%d.%d.%d",&gateway_toradex[0],&gateway_toradex[1],&gateway_toradex[2],&gateway_toradex[3]);
  sscanf(mask,"%d.%d.%d.%d",&mask_toradex[0],&mask_toradex[1],&mask_toradex[2],&mask_toradex[3]);
  contabitmask  = conta_bit (mask_toradex[0], 1);
  contabitmask += conta_bit (mask_toradex[1], 1);
  contabitmask += conta_bit (mask_toradex[2], 1);
  contabitmask += conta_bit (mask_toradex[3], 1);

  fp = fopen("/etc/systemd/network/wired.network","w");
  if(!fp)
  {
    fclose(fp);
    printf( "Erro na escrita no /etc/systemd/network/wired.network");
    return 0;
  }
  fprintf(fp, "[Match]\n");
  fprintf(fp, "Name=eth0\n");

  fprintf(fp, "\n[Network]\n");

  fprintf(fp, "Address=");
  fprintf(fp, "%s",ip);
  fprintf(fp, "/%d\n",contabitmask);

  fprintf(fp, "Gateway=");
  fprintf(fp, "%s\n",gateway);

  fprintf(fp, "DNS=8.8.8.8\n");

  fclose(fp);

  system("ip addr flush dev eth0 && ip route flush dev eth0");
  system("ip link set eth0 up");

  CalBcast(ip, mask, bcast);
  sprintf(buffer_toradex, "ip addr add %s/%d broadcast %s dev eth0", ip,contabitmask,bcast);
  system (buffer_toradex);
  sprintf(buffer_toradex, "ip route add default via %s", gateway);
  system (buffer_toradex);

  //------------------------------------------------------
  return 0;
}


void CriaArquivoHistorico(unsigned int tipo, unsigned int limpa_datalog)
{
  FILE *ArqHistJS, *ArqHistTXT, *ArqHistTMP, *ArqHistTXT_TMP;
  char buffer_temp[30];
  int t;
  unsigned short temp_u16;
  double *ptr_double;
  long long int *ptr_u64;
  long long int llu64;
  BYTE4 TempData;
  BYTE8 TempData2;
  static unsigned int first_time;
  static unsigned int reg_count;
  long long int llu64Tmp=0;

  /* Força a limpar o datalog */
  if (limpa_datalog == 1)
  {
    first_time = 0;
  }
  else
  {
    /*
     * - Escreve a posição que quer ler da memória
     */
    TempData.WData[0] = PointerDLogWRITE;
    buffer_temp[1]=TempData.BData[0];
    buffer_temp[0]=TempData.BData[1];
    (void)PressetMultipleRegisters(SerialPort,1,400,2, (unsigned char *)&buffer_temp[0]);

    usleep(500);

    /*
     * - Lê a memória
     */
    (void)ReadInputRegister(SerialPort,1,2000,48,&ReceiveAuxBuffer[0]);

    usleep(1000);


    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // UNIT. LOGs

    UnidLogFlow = (ReceiveAuxBuffer[91]<<8) | ReceiveAuxBuffer[92];
    UnidLogTemp = (ReceiveAuxBuffer[93]<<8) | ReceiveAuxBuffer[94];
    UnidLogDens = (ReceiveAuxBuffer[95]<<8) | ReceiveAuxBuffer[96];
    UnidLogVol  = (ReceiveAuxBuffer[97]<<8) | ReceiveAuxBuffer[98];
    /////////////////////////////////////////////////////////////////////////////////////////////////////////

    if (tipo == UPDATE)
    {
      /* Validação da Data, para inserir no arquivo */
      TempData.BData[0]=ReceiveAuxBuffer[3];	//Dia
      TempData.BData[1]=ReceiveAuxBuffer[4];  //Mês

      if (((TempData.BData[0] > 0)&&(TempData.BData[0]<0x32))||
          ((TempData.BData[1] > 0)&&(TempData.BData[1]<0x13)))
      {
        ArqHistTMP = fopen("/tmp/VirtualRAM/arquivo-historico.tmp","w");
        ArqHistTXT_TMP = fopen("/tmp/VirtualRAM/arquivo-historico.txt-tmp","w");


        if (ArqHistTMP != NULL)
        {
          fprintf(ArqHistTXT_TMP, "%c%c%c",0xEF,0xBB, 0xBF);
          fprintf(ArqHistTXT_TMP,"Data Início\tData Término\tVeículo\tOperador\tLacre\tCód. Operação\tModo Operação\tVolume Bruto\tVolume Corrigido\tMédia Vazão\tMédia Temperatura\tMédia Densidade\tAlarme Erro #1\tAlarme Erro #2\tAlarme Warning\r\n");

          fprintf(ArqHistTMP, "%s", arqhist_ini);
          fprintf(ArqHistTMP, " { ");

          //Counter registro
          fprintf(ArqHistTMP, "%s", varhist_POSICAO);
          sprintf(buffer_temp, "%3d\",", PointerDLogWRITE);
          fprintf(ArqHistTMP, "%s", buffer_temp);

          // Data início
          fprintf(ArqHistTMP, "%s", varhist_DATAINICIO);
          sprintf(buffer_temp, "%02x/%02x/%02x%02x ", ReceiveAuxBuffer[3], ReceiveAuxBuffer[4], ReceiveAuxBuffer[5],ReceiveAuxBuffer[6]);
          fprintf(ArqHistTMP, "%s", buffer_temp);
          fprintf(ArqHistTXT_TMP, "%s", buffer_temp);
          sprintf(buffer_temp, "%02x:%02x:%02x\",", ReceiveAuxBuffer[7], ReceiveAuxBuffer[8], ReceiveAuxBuffer[9]);
          fprintf(ArqHistTMP, "%s", buffer_temp);
          fprintf(ArqHistTXT_TMP, "%02x:%02x:%02x\t", ReceiveAuxBuffer[7], ReceiveAuxBuffer[8], ReceiveAuxBuffer[9]);

          // Data término
          fprintf(ArqHistTMP, "%s", varhist_DATATERMINO);
          sprintf(buffer_temp, "%02x/%02x/%02x%02x ", ReceiveAuxBuffer[11], ReceiveAuxBuffer[12], ReceiveAuxBuffer[13],ReceiveAuxBuffer[14]);
          fprintf(ArqHistTMP, "%s", buffer_temp);
          fprintf(ArqHistTXT_TMP, "%s", buffer_temp);
          sprintf(buffer_temp, "%02x:%02x:%02x\",", ReceiveAuxBuffer[15], ReceiveAuxBuffer[16], ReceiveAuxBuffer[17]);
          fprintf(ArqHistTMP, "%s", buffer_temp);
          fprintf(ArqHistTXT_TMP, "%02x:%02x:%02x\t", ReceiveAuxBuffer[15], ReceiveAuxBuffer[16], ReceiveAuxBuffer[17]);

          //Veículo
          fprintf(ArqHistTMP, "%s", varhist_VEICULO);
          for(t=19; t<27; t++){ if(ReceiveAuxBuffer[t]<0x23) ReceiveAuxBuffer[t] = 0x20;}
          memcpy(buffer_temp,&ReceiveAuxBuffer[19],8);  buffer_temp[8]=0;
          fprintf(ArqHistTMP, "%s\",", buffer_temp);
          fprintf(ArqHistTXT_TMP, "%s\t", buffer_temp);

          //Operador
          fprintf(ArqHistTMP, "%s", varhist_OPERADOR);
          for(t=27; t<35; t++){ if(ReceiveAuxBuffer[t]<0x23) ReceiveAuxBuffer[t] = 0x20;}
          memcpy(buffer_temp,&ReceiveAuxBuffer[27],8);   buffer_temp[8]=0;
          fprintf(ArqHistTMP, "%s\",", buffer_temp);
          fprintf(ArqHistTXT_TMP, "%s\t", buffer_temp);

          //Lacre
          fprintf(ArqHistTMP, "%s", varhist_LACRE);
          for(t=35; t<43; t++){ if(ReceiveAuxBuffer[t]<0x23) ReceiveAuxBuffer[t] = 0x20;}
          memcpy(buffer_temp,&ReceiveAuxBuffer[35],8);   buffer_temp[8]=0;
          fprintf(ArqHistTMP, "%s\",", buffer_temp);
          fprintf(ArqHistTXT_TMP, "%s\t", buffer_temp);

          //Código operação
          fprintf(ArqHistTMP, "%s", varhist_COD_OP);
          for(t=43; t<51; t++){ if(ReceiveAuxBuffer[t]<0x23) ReceiveAuxBuffer[t] = 0x20;}
          memcpy(buffer_temp,&ReceiveAuxBuffer[43],8);   buffer_temp[8]=0;
          fprintf(ArqHistTMP, "%s\",", buffer_temp);
          fprintf(ArqHistTXT_TMP, "%s\t", buffer_temp);

          //Modo operação
          fprintf(ArqHistTMP, "%s", varhist_MODO_OP);
          temp_u16 = (ReceiveAuxBuffer[51]<<8) | ReceiveAuxBuffer[52];
          fprintf(ArqHistTMP, "%s\",", GetString_Tabela2(temp_u16));
          fprintf(ArqHistTXT_TMP, "%s\t", GetString_Tabela2(temp_u16));

          //Volume bruto
          fprintf(ArqHistTMP, "%s", varhist_VOL_BRUTO);
          for(t=0; t<8; t++){  TempData2.BData[(7-t)] = ReceiveAuxBuffer[53+t]; }
          fprintf(ArqHistTMP, "%.1lf", TempData2.DData);
          GetUnit(ArqHistTMP, UNIT_VOL_LOG);
          fprintf(ArqHistTMP, "\",");
          fprintf(ArqHistTXT_TMP, "%.1lf", TempData2.DData);
          GetUnit(ArqHistTXT_TMP, UNIT_VOL_LOG);
          fprintf(ArqHistTXT_TMP, " \t");

          //Volume Corrigido
          fprintf(ArqHistTMP, "%s", varhist_VOL_CORRIGIDO);
          for(t=0; t<8; t++){  TempData2.BData[(7-t)] = ReceiveAuxBuffer[61+t]; }
          fprintf(ArqHistTMP, "%.1lf", TempData2.DData);
          GetUnit(ArqHistTMP, UNIT_VOL_LOG);
          fprintf(ArqHistTMP, "\",");
          fprintf(ArqHistTXT_TMP, "%.1lf", TempData2.DData);
          GetUnit(ArqHistTXT_TMP, UNIT_VOL_LOG);
          fprintf(ArqHistTXT_TMP, " \t");

          //Média Vazao
          fprintf(ArqHistTMP, "%s", varhist_VAZAO);
          for(t=0; t<4; t++){  TempData.BData[(3-t)] = ReceiveAuxBuffer[69+t]; }
          fprintf(ArqHistTMP, "%.1f", TempData.FData);
          GetUnit(ArqHistTMP, UNIT_FLOW_LOG);
          fprintf(ArqHistTMP, "\",");
          fprintf(ArqHistTXT_TMP, "%.1f", TempData.FData);
          GetUnit(ArqHistTXT_TMP, UNIT_FLOW_LOG);
          fprintf(ArqHistTXT_TMP, " \t");

          //Média Temperatura
          fprintf(ArqHistTMP, "%s", varhist_TEMP);
          for(t=0; t<4; t++){  TempData.BData[(3-t)] = ReceiveAuxBuffer[73+t]; }
          fprintf(ArqHistTMP, "%.2f", TempData.FData);
          GetUnit(ArqHistTMP, UNIT_TEMP_LOG);
          fprintf(ArqHistTMP, "\",");
          fprintf(ArqHistTXT_TMP, "%.2f", TempData.FData);
          GetUnit(ArqHistTXT_TMP, UNIT_TEMP_LOG);
          fprintf(ArqHistTXT_TMP, " \t");

          //Média Densidade
          fprintf(ArqHistTMP, "%s", varhist_DENS);
          ptr_u64 = (long long int *)&ReceiveAuxBuffer[77];
          (void)memcpy(&llu64Tmp,ptr_u64,8);
          llu64 = bswap_64(llu64Tmp);
          ptr_double = (double *)&llu64;
          fprintf(ArqHistTMP, "%.1f", *ptr_double);
          GetUnit(ArqHistTMP, UNIT_DENS_LOG);
          fprintf(ArqHistTMP, "\",");
          fprintf(ArqHistTXT_TMP, "%.1f", *ptr_double);
          GetUnit(ArqHistTXT_TMP, UNIT_DENS_LOG);
          fprintf(ArqHistTXT_TMP, " \t");

          //Alarme Erro
          fprintf(ArqHistTMP, "%s", varhist_ERROR);
          temp_u16 = (ReceiveAuxBuffer[85]<<8) | ReceiveAuxBuffer[86];
          fprintf(ArqHistTMP, "%s\",", GetString_Tabela15(temp_u16));
          fprintf(ArqHistTXT_TMP, "%s \t", GetString_Tabela15(temp_u16));

          //Alarme Warning
          fprintf(ArqHistTMP, "%s", varhist_WARNING);
          temp_u16 = (ReceiveAuxBuffer[87]<<8) | ReceiveAuxBuffer[88];
          fprintf(ArqHistTMP, "%s\",", GetString_Tabela16(temp_u16));
          fprintf(ArqHistTXT_TMP, "%s \t", GetString_Tabela16(temp_u16));

          //Alarme Erro 2
          fprintf(ArqHistTMP, "%s", varhist_ERROR2);
          temp_u16 = (ReceiveAuxBuffer[89]<<8) | ReceiveAuxBuffer[90];
          fprintf(ArqHistTMP, "%s\"", GetString_Tabela17(temp_u16));
          fprintf(ArqHistTXT_TMP, "%s \r\n", GetString_Tabela17(temp_u16));

          fprintf(ArqHistTMP, " }\n");
        }
        fclose(ArqHistTMP);
        fclose(ArqHistTXT_TMP);


        /* Trata arquivo-historico.js */
        system("cat /tmp/VirtualRAM/arquivo-historico.js | grep -v \"historico\" > /tmp/VirtualRAM/arquivo-historico.tmp2");

        usleep(1000);

        /* Novo registro apos ler toda a memoria */
        ArqHistJS = fopen("/tmp/VirtualRAM/arquivo-historico.tmp2","r+");

        if (ArqHistJS != NULL)
        {
          fseek(ArqHistJS, 0, SEEK_SET);
          fprintf(ArqHistJS, ",{");
        }
        fclose(ArqHistJS);

        system("cat /tmp/VirtualRAM/arquivo-historico.tmp2 >> /tmp/VirtualRAM/arquivo-historico.tmp");
        system("mv /tmp/VirtualRAM/arquivo-historico.tmp /tmp/VirtualRAM/arquivo-historico.js");

        /* Trata arquivo TXT */
        system("cat /tmp/VirtualRAM/arquivo-historico.txt | grep -v \"Data\" > /tmp/VirtualRAM/arquivo-historico.txt2");
        system("mv /tmp/VirtualRAM/arquivo-historico.txt-tmp /tmp/VirtualRAM/arquivo-historico.txt");
        system("cat /tmp/VirtualRAM/arquivo-historico.txt2 >> /tmp/VirtualRAM/arquivo-historico.txt");

        reg_count++;

        if (reg_count > 500)
        {
          /* Remove ultimo registro do arquivo JS */
          system("sed -ie '$d' /tmp/VirtualRAM/arquivo-historico.js");
          system("sed -ie '$d' /tmp/VirtualRAM/arquivo-historico.js");

          /* Adiciona finalizador */
          ArqHistJS=fopen("/tmp/VirtualRAM/arquivo-historico.js","a");
          fprintf(ArqHistJS, "%s", "]}]");
          fclose(ArqHistJS);

          /* Remove ultimo registro do arquivo TXT*/
          system("sed -ie '$d' /tmp/VirtualRAM/arquivo-historico.txt");
        }
      }
    }


    /* Baixa todos os registros da memoria e cria
     * o arquivo pela primeira vez
     */
    if ((tipo == DOWNLOAD) && (first_time == 0))
    {
      /* Limpa o arquivo */
      ArqHistJS=fopen("/tmp/VirtualRAM/arquivo-historico.js","w");

      if (ArqHistJS != NULL)
      {
        fprintf(ArqHistJS, "%s", arqhist_ini);
      }
      fclose(ArqHistJS);

      ArqHistTXT=fopen("/tmp/VirtualRAM/arquivo-historico.txt","w");
      fprintf(ArqHistTXT, "%c%c%c",0xEF,0xBB, 0xBF);
      fprintf(ArqHistTXT,"Data Início\tData Término\tVeículo\tOperador\tLacre\tCód. Operação\tModo Operação\tVolume Bruto\tVolume Corrigido\tMédia Vazão\tMédia Temperatura\tMédia Densidade\tAlarme Erro #1\tAlarme Erro #2\tAlarme Warning\r\n");
      fclose(ArqHistTXT);

      first_time = 1;
    }


    if (tipo == DOWNLOAD)
    {
      /* Validação da Data, para inserir no arquivo */
      TempData.BData[0]=ReceiveAuxBuffer[3];	//Dia
      TempData.BData[1]=ReceiveAuxBuffer[4];  //Mês


      if (((TempData.BData[0] > 0)&&(TempData.BData[0]<0x32))||
          ((TempData.BData[1] > 0)&&(TempData.BData[1]<0x13)))
      {
        if (first_time == 2)
        {
          /* Limpa a ultima linha do arquivo antes de inserir um novo registro */
          system("/opt/SDM/Scripts/./DeleteLine.sh");
          usleep(1000);
        }

        ArqHistJS=fopen("/tmp/VirtualRAM/arquivo-historico.js","a");
        ArqHistTXT=fopen("/tmp/VirtualRAM/arquivo-historico.txt","a");


        if (ArqHistJS != NULL)
        {

          if (first_time == 1)
          {
            fprintf(ArqHistJS, " { ");
            first_time = 2;
          }
          else if (first_time == 2)
          {
            fprintf(ArqHistJS, ",{ ");
          }

          reg_count++;

          //Counter registro
          fprintf(ArqHistJS, "%s", varhist_POSICAO);
          sprintf(buffer_temp, "%3d\",", PointerDLogWRITE);
          fprintf(ArqHistJS, "%s", buffer_temp);

          // Data início
          fprintf(ArqHistJS, "%s", varhist_DATAINICIO);
          sprintf(buffer_temp, "%02x/%02x/%02x%02x ", ReceiveAuxBuffer[3], ReceiveAuxBuffer[4], ReceiveAuxBuffer[5],ReceiveAuxBuffer[6]);
          fprintf(ArqHistJS, "%s", buffer_temp);
          fprintf(ArqHistTXT, "%s", buffer_temp);
          sprintf(buffer_temp, "%02x:%02x:%02x\",", ReceiveAuxBuffer[7], ReceiveAuxBuffer[8], ReceiveAuxBuffer[9]);
          fprintf(ArqHistJS, "%s", buffer_temp);
          fprintf(ArqHistTXT, "%02x:%02x:%02x\t", ReceiveAuxBuffer[7], ReceiveAuxBuffer[8], ReceiveAuxBuffer[9]);


          // Data término
          fprintf(ArqHistJS, "%s", varhist_DATATERMINO);
          sprintf(buffer_temp, "%02x/%02x/%02x%02x ", ReceiveAuxBuffer[11], ReceiveAuxBuffer[12], ReceiveAuxBuffer[13],ReceiveAuxBuffer[14]);
          fprintf(ArqHistJS, "%s", buffer_temp);
          fprintf(ArqHistTXT, "%s", buffer_temp);
          sprintf(buffer_temp, "%02x:%02x:%02x\",", ReceiveAuxBuffer[15], ReceiveAuxBuffer[16], ReceiveAuxBuffer[17]);
          fprintf(ArqHistJS, "%s", buffer_temp);
          fprintf(ArqHistTXT, "%02x:%02x:%02x\t", ReceiveAuxBuffer[15], ReceiveAuxBuffer[16], ReceiveAuxBuffer[17]);

          //Veículo
          fprintf(ArqHistJS, "%s", varhist_VEICULO);
          for(t=19; t<27; t++){ if(ReceiveAuxBuffer[t]<0x23) ReceiveAuxBuffer[t] = 0x20;}
          memcpy(buffer_temp,&ReceiveAuxBuffer[19],8);  buffer_temp[8]=0;
          fprintf(ArqHistJS, "%s\",", buffer_temp);
          fprintf(ArqHistTXT, "%s\t", buffer_temp);


          //Operador
          fprintf(ArqHistJS, "%s", varhist_OPERADOR);
          for(t=27; t<35; t++){ if(ReceiveAuxBuffer[t]<0x23) ReceiveAuxBuffer[t] = 0x20;}
          memcpy(buffer_temp,&ReceiveAuxBuffer[27],8);   buffer_temp[8]=0;
          fprintf(ArqHistJS, "%s\",", buffer_temp);
          fprintf(ArqHistTXT, "%s\t", buffer_temp);


          //Lacre
          fprintf(ArqHistJS, "%s", varhist_LACRE);
          for(t=35; t<43; t++){ if(ReceiveAuxBuffer[t]<0x23) ReceiveAuxBuffer[t] = 0x20;}
          memcpy(buffer_temp,&ReceiveAuxBuffer[35],8);   buffer_temp[8]=0;
          fprintf(ArqHistJS, "%s\",", buffer_temp);
          fprintf(ArqHistTXT, "%s\t", buffer_temp);

          //Código operação
          fprintf(ArqHistJS, "%s", varhist_COD_OP);
          for(t=43; t<51; t++){ if(ReceiveAuxBuffer[t]<0x23) ReceiveAuxBuffer[t] = 0x20;}
          memcpy(buffer_temp,&ReceiveAuxBuffer[43],8);   buffer_temp[8]=0;
          fprintf(ArqHistJS, "%s\",", buffer_temp);
          fprintf(ArqHistTXT, "%s\t", buffer_temp);

          //Modo operação
          fprintf(ArqHistJS, "%s", varhist_MODO_OP);
          temp_u16 = (ReceiveAuxBuffer[51]<<8) | ReceiveAuxBuffer[52];
          fprintf(ArqHistJS, "%s\",", GetString_Tabela2(temp_u16));
          fprintf(ArqHistTXT, "%s\t", GetString_Tabela2(temp_u16));

          //Volume bruto
          fprintf(ArqHistJS, "%s", varhist_VOL_BRUTO);
          for(t=0; t<8; t++){  TempData2.BData[(7-t)] = ReceiveAuxBuffer[53+t]; }
          fprintf(ArqHistJS, "%.1lf", TempData2.DData);
          GetUnit(ArqHistJS, UNIT_VOL_LOG);
          fprintf(ArqHistJS, "\",");
          fprintf(ArqHistTXT, "%.1lf", TempData2.DData);
          GetUnit(ArqHistTXT, UNIT_VOL_LOG);
          fprintf(ArqHistTXT, " \t");

          //Volume Corrigido
          fprintf(ArqHistJS, "%s", varhist_VOL_CORRIGIDO);
          for(t=0; t<8; t++){  TempData2.BData[(7-t)] = ReceiveAuxBuffer[61+t]; }
          fprintf(ArqHistJS, "%.1lf", TempData2.DData);
          GetUnit(ArqHistJS, UNIT_VOL_LOG);
          fprintf(ArqHistJS, "\",");
          fprintf(ArqHistTXT, "%.1lf", TempData2.DData);
          GetUnit(ArqHistTXT, UNIT_VOL_LOG);
          fprintf(ArqHistTXT, " \t");

          //Média Vazao
          fprintf(ArqHistJS, "%s", varhist_VAZAO);
          for(t=0; t<4; t++){  TempData.BData[(3-t)] = ReceiveAuxBuffer[69+t]; }
          fprintf(ArqHistJS, "%.1f", TempData.FData);
          GetUnit(ArqHistJS, UNIT_FLOW_LOG);
          fprintf(ArqHistJS, "\",");
          fprintf(ArqHistTXT, "%.1f", TempData.FData);
          GetUnit(ArqHistTXT, UNIT_FLOW_LOG);
          fprintf(ArqHistTXT, " \t");

          //Média Temperatura
          fprintf(ArqHistJS, "%s", varhist_TEMP);
          for(t=0; t<4; t++){  TempData.BData[(3-t)] = ReceiveAuxBuffer[73+t]; }
          fprintf(ArqHistJS, "%.2f", TempData.FData);
          GetUnit(ArqHistJS, UNIT_TEMP_LOG);
          fprintf(ArqHistJS, "\",");
          fprintf(ArqHistTXT, "%.2f", TempData.FData);
          GetUnit(ArqHistTXT, UNIT_TEMP_LOG);
          fprintf(ArqHistTXT, " \t");

          //Média Densidade
          fprintf(ArqHistJS, "%s", varhist_DENS);
          ptr_u64 = (long long int *)&ReceiveAuxBuffer[77];
          (void)memcpy(&llu64Tmp,ptr_u64,8);
          llu64 = bswap_64(llu64Tmp);
          ptr_double = (double *)&llu64;
          fprintf(ArqHistJS, "%.1f", *ptr_double);
          GetUnit(ArqHistJS, UNIT_DENS_LOG);
          fprintf(ArqHistJS, "\",");
          fprintf(ArqHistTXT, "%.1f", *ptr_double);
          GetUnit(ArqHistTXT, UNIT_DENS_LOG);
          fprintf(ArqHistTXT, " \t");

          //Alarme Erro
          fprintf(ArqHistJS, "%s", varhist_ERROR);
          temp_u16 = (ReceiveAuxBuffer[85]<<8) | ReceiveAuxBuffer[86];
          fprintf(ArqHistJS, "%s\",", GetString_Tabela15(temp_u16));
          fprintf(ArqHistTXT, "%s \t", GetString_Tabela15(temp_u16));

          //Alarme Warning
          fprintf(ArqHistJS, "%s", varhist_WARNING);
          temp_u16 = (ReceiveAuxBuffer[87]<<8) | ReceiveAuxBuffer[88];
          fprintf(ArqHistJS, "%s\",", GetString_Tabela16(temp_u16));
          fprintf(ArqHistTXT, "%s \t", GetString_Tabela16(temp_u16));

          //Alarme Erro 2
          fprintf(ArqHistJS, "%s", varhist_ERROR2);
          temp_u16 = (ReceiveAuxBuffer[89]<<8) | ReceiveAuxBuffer[90];
          fprintf(ArqHistJS, "%s\"", GetString_Tabela17(temp_u16));
          fprintf(ArqHistTXT, "%s \r\n", GetString_Tabela17(temp_u16));

          fprintf(ArqHistJS, " }");
          fprintf(ArqHistJS, "%s", arqhist_fim);

          fclose(ArqHistJS);
          fclose(ArqHistTXT);
        }
      }
    }
  }
}

void LeArquivoEscrita(void)
{
  FILE *fd, *fd2;
  static int i_temp, j_temp;
  static char buffer_esc[50];
  static char *ptr;
  static char *ptr_fim;
  static int pos;
  static int size;
  static int size_frame_dados;
  static char addr_ascii[10];
  static int addr_int;
  static char hexa_ascii[50];
  static char tipo[12];
  static unsigned char byte_valor[30];
  unsigned char flag_ok = 0;

  memset(byte_valor,0,sizeof(byte_valor));
  memset(buffer_esc,0,sizeof(buffer_esc));

  fd = fopen("/tmp/VirtualRAM/arquivo-escrita.js", "r");

  i_temp = 0;
  j_temp = 0;

  if (fd)
  {
    while(1)
    {
      buffer_esc[i_temp] = (char)fgetc(fd);
      if (feof(fd) || (buffer_esc[i_temp] == '\n'))
      {
        break;
      }
      i_temp++;
    }

    fclose(fd);

    if (buffer_esc[0] == '{')
    {
      ptr = strchr(buffer_esc, ',');
      pos = ptr - &buffer_esc[0];

      strncpy(addr_ascii, &buffer_esc[1], pos-1);
      addr_int = atoi(addr_ascii);

      ptr_fim = strchr(&buffer_esc[pos+1], ',');
      size = (ptr_fim - &buffer_esc[pos])-1;

      size_frame_dados = size/2;

      strncpy(hexa_ascii, &buffer_esc[pos+1], size);

      strncpy(tipo, ++ptr_fim, sizeof(tipo));


      if ((strncmp(tipo, "u16", 3) == 0 ) || (strncmp(tipo, "u32", 3) == 0 ))
      {
        /* Converte Hex Ascii para byte */
        for (i_temp=0; i_temp<size_frame_dados; i_temp++)
        {
          j_temp = i_temp*2;

          if ((hexa_ascii[j_temp] >= '0') && (hexa_ascii[j_temp] <= '9'))
          {
            byte_valor[i_temp] = (hexa_ascii[j_temp] - 0x30)<<4;
          }
          else if ((hexa_ascii[j_temp] >= 'A') && (hexa_ascii[j_temp] <= 'F'))
          {
            byte_valor[i_temp] = (hexa_ascii[j_temp] - 0x37)<<4;
          }

          j_temp++;

          if ((hexa_ascii[j_temp] >= '0') && (hexa_ascii[j_temp] <= '9'))
          {
            byte_valor[i_temp] |= (hexa_ascii[j_temp] - 0x30);
          }
          else if ((hexa_ascii[j_temp] >= 'A') && (hexa_ascii[j_temp] <= 'F'))
          {
            byte_valor[i_temp] |= (hexa_ascii[j_temp] - 0x37);
          }
        }
        flag_ok = 1;
      }
      else if ((strncmp(tipo, "password", 8) == 0 ) ||
               (strncmp(tipo, "bytearray4", 10) == 0 ) ||
               (strncmp(tipo, "bytearray8", 10) == 0 ) ||
               (strncmp(tipo, "bytearray10", 11) == 0 )||
               (strncmp(tipo, "bytearray16", 11) == 0 )||
               (strncmp(tipo, "bitstring16", 11) == 0 ))
      {
        /* Converte Hex Ascii para byte */
        for (i_temp=0; i_temp<size_frame_dados; i_temp++)
        {
          j_temp = i_temp*2;

          if ((hexa_ascii[j_temp] >= '0') && (hexa_ascii[j_temp] <= '9'))
          {
            byte_valor[i_temp] = (hexa_ascii[j_temp] - 0x30)<<4;
          }
          else if ((hexa_ascii[j_temp] >= 'A') && (hexa_ascii[j_temp] <= 'F'))
          {
            byte_valor[i_temp] = (hexa_ascii[j_temp] - 0x37)<<4;
          }

          j_temp++;

          if ((hexa_ascii[j_temp] >= '0') && (hexa_ascii[j_temp] <= '9'))
          {
            byte_valor[i_temp] |= (hexa_ascii[j_temp] - 0x30);
          }
          else if ((hexa_ascii[j_temp] >= 'A') && (hexa_ascii[j_temp] <= 'F'))
          {
            byte_valor[i_temp] |= (hexa_ascii[j_temp] - 0x37);
          }
        }
        flag_ok = 1;
      }

      if ((flag_ok == 1) && (size_frame_dados != 0))
      {
        fd2 = fopen("/tmp/VirtualRAM/LogEscrita.log", "a");

        fprintf(fd2, "\n%02x/%02x", IRSerialBuffer[LISTA_50][3], IRSerialBuffer[LISTA_50][4]);
        fprintf(fd2, "- %02x:%02x:%02x - ", IRSerialBuffer[LISTA_50][7], IRSerialBuffer[LISTA_50][8], IRSerialBuffer[LISTA_50][9]);
        fprintf(fd2, "arq escrita Add: %d - Size: %d - V1:%x V2:%x V3:%x V4:%x V5:%x V6:%x", addr_int, size_frame_dados, byte_valor[0], byte_valor[1], \
            byte_valor[02], byte_valor[3], byte_valor[4], byte_valor[5]);
        fclose(fd2);

        (void)PressetMultipleRegisters(SerialPort,1,addr_int,size_frame_dados,byte_valor);
        usleep(1000);
      }
      system("/opt/SDM/Scripts/./DeleteFirstLine.sh");
    }

  }
}


void CriaArquivoGEMAP(unsigned char erro_id)
{
  FILE *ArqJS, *fpDebug;
  FILE *fpPIPE;
  time_t t_atual, t_res;
  struct tm tm;
  unsigned char i;
  char buffer_temp[40];
  unsigned short temp_u16;
  double *ptr_double;
  long long int *ptr_u64;
  long long int llu64;
  long long int llu64Tmp=0;
  unsigned int *ptr_u32;
  unsigned int temp_u32;
  unsigned int llu32Tmp=0;
  float *ptr_float;
  static int count_version = 30;
  static char buffer_version[3][40];
  unsigned int dbg_addr, dbg_count;
  unsigned short ListaOperadoresSenhas;

  if (erro_id == SERIAL_NOT_FOUND)
  {
    if((ArqJS = 1 ? fopen("/tmp/VirtualRAM/arquivo.js","w") : stdin) == NULL)
    {
      printf("Erro ao abrir /tmp/VirtualRAM/arquivo.js");
    }
    else
    {
#ifdef SET_DEBUG_TXT
      // Abre arquivo
      fprintf(ArqJS, "%s", _ini);

      fprintf(ArqJS, "\n\nSERIAL %s NAO ENCONTRADO -> %d minutos sem comunicacao \n\n", &ComPath[0], Fault_COMM);

      // Serial RX Counter
      fprintf(ArqJS, "Serial RX Counter: %ld \n\n", SerialRXCounter);

      // Erros de comunicação e CRC
      fprintf(ArqJS, "Erro COMM: %d \n\n", ErrCommCounter);

      // MainServer Lock
      fprintf(ArqJS, "MainServer Lock: %d \n\n", MainServerLock);

      // Fecha arquivo
      fprintf(ArqJS, "%s", _fim);

      fclose(ArqJS);
#else
      // Abre arquivo
      fprintf(ArqJS, "%s", arq_ini);
      fprintf(ArqJS, "%s%s", tit_erroinit, submenu_erroinit);

      fprintf(ArqJS, "%s", var_Erro1);
      fprintf(ArqJS, "<b>SERIAL %s NÃO ENCONTRADO</b><br>%d minutos sem comunicação", &ComPath[0], Fault_COMM);
      fprintf(ArqJS, "%s,", var_Erro2);

      // Serial RX Counter
      fprintf(ArqJS, "%s", var_erro_RX1);
      fprintf(ArqJS, "%ld %s,", SerialRXCounter, var_erro_RX2);

      // Erros de comunicação e CRC
      fprintf(ArqJS, "%s", var_erro_Comm1);
      fprintf(ArqJS, "%d %s,", ErrCommCounter, var_erro_Comm2);

      // MainServer Lock
      fprintf(ArqJS, "%s", var_MSLock1);
      fprintf(ArqJS, "%d %s", MainServerLock, var_MSLock2);

      // Fecha Submenu e Titulo
      fprintf(ArqJS, "%s%s", submenu_fim, tit_fim);

      // Fecha arquivo
      fprintf(ArqJS, "%s", arq_fim);

      fclose(ArqJS);
#endif
    }
  }
  else if (erro_id == NO_ERROR)
  {
    TrataASCIIChar((unsigned char *)&IRSerialBuffer[LISTA_100][3], 40);
    TrataASCIIChar((unsigned char *)&IRSerialBuffer[LISTA_200][3], 40);
    TrataASCIIChar((unsigned char *)&IRSerialBuffer[LISTA_500][13], 16);

    /************ ARQUIVO DEBUG ***********/
#ifdef SET_DEBUG_TXT
    if((fpDebug = 1 ? fopen("/tmp/VirtualRAM/debug_IR.txt","w") : stdin) == NULL)
    {
      printf("Erro ao abrir /tmp/VirtualRAM/debug_IR.txt");
    }
    else
    {
      fprintf(fpDebug, "%s", "\n\n *** Debug Input Register ***\n");
      fprintf(fpDebug,"%s %ld \n","TX:",SerialTXCounter);
      fprintf(fpDebug,"%s %ld \n\n","RX:",SerialRXCounter);

      for (dbg_count=0; dbg_count<MAX_IR_LIST_SIZE; dbg_count++){

        fprintf(fpDebug,"TX%d: %d \t",dbg_count,SerialTXListIR[dbg_count]);
        fprintf(fpDebug,"RX%d: %d \n",dbg_count,SerialRXListIR[dbg_count]);

      }
      fprintf(fpDebug,"\n");
      for (dbg_addr=0; dbg_addr<MAX_IR_LIST_SIZE; dbg_addr++)
      {
        if (IRStartAddr[dbg_addr] == 0)
        {
          break;
        }

        fprintf(fpDebug, "Lista %d - Count %d \n", IRStartAddr[dbg_addr], IRCount[dbg_addr]);

        for (dbg_count=0; dbg_count<((IRCount[dbg_addr]*2)+3); dbg_count=dbg_count+8)
        {
          fprintf(fpDebug, "%02X - ", IRSerialBuffer[dbg_addr][dbg_count]);
          fprintf(fpDebug, "%02X - ", IRSerialBuffer[dbg_addr][dbg_count+1]);
          fprintf(fpDebug, "%02X - ", IRSerialBuffer[dbg_addr][dbg_count+2]);
          fprintf(fpDebug, "%02X - ", IRSerialBuffer[dbg_addr][dbg_count+3]);
          fprintf(fpDebug, "%02X - ", IRSerialBuffer[dbg_addr][dbg_count+4]);
          fprintf(fpDebug, "%02X - ", IRSerialBuffer[dbg_addr][dbg_count+5]);
          fprintf(fpDebug, "%02X - ", IRSerialBuffer[dbg_addr][dbg_count+6]);
          fprintf(fpDebug, "%02X \n ", IRSerialBuffer[dbg_addr][dbg_count+7]);
        }

        fprintf(fpDebug, "%s", "\n\n");
      }

      fprintf(fpDebug, "%s", "Fim - arquivo\n\n");
    }
#else
    if((fpDebug = 1 ? fopen("/tmp/VirtualRAM/debug.htm","w") : stdin) == NULL)
    {
      printf("Erro ao abrir /tmp/VirtualRAM/debug.htm");
    }
    else
    {
      fprintf(fpDebug, "%s", "<html><body>");

      for (dbg_addr=0; dbg_addr<MAX_IR_LIST_SIZE; dbg_addr++)
      {
        if (IRStartAddr[dbg_addr] == 0)
        {
          break;
        }

        fprintf(fpDebug, "<b>Lista %d - Count %d</b><br><table cellpadding=""3"">", IRStartAddr[dbg_addr], IRCount[dbg_addr]);

        for (dbg_count=0; dbg_count<((IRCount[dbg_addr]*2)+3); dbg_count=dbg_count+8)
        {
          fprintf(fpDebug, "%s", "<tr>");
          fprintf(fpDebug, "<td> %02X</td>", IRSerialBuffer[dbg_addr][dbg_count]);
          fprintf(fpDebug, "<td> %02X</td>", IRSerialBuffer[dbg_addr][dbg_count+1]);
          fprintf(fpDebug, "<td> %02X</td>", IRSerialBuffer[dbg_addr][dbg_count+2]);
          fprintf(fpDebug, "<td> %02X</td>", IRSerialBuffer[dbg_addr][dbg_count+3]);
          fprintf(fpDebug, "<td> %02X</td>", IRSerialBuffer[dbg_addr][dbg_count+4]);
          fprintf(fpDebug, "<td> %02X</td>", IRSerialBuffer[dbg_addr][dbg_count+5]);
          fprintf(fpDebug, "<td> %02X</td>", IRSerialBuffer[dbg_addr][dbg_count+6]);
          fprintf(fpDebug, "<td> %02X</td>", IRSerialBuffer[dbg_addr][dbg_count+7]);
          fprintf(fpDebug, "%s", "</tr>");
        }

        fprintf(fpDebug, "%s", "</table><br>");
      }

      fprintf(fpDebug, "%s", "</body></html>");
    }

#endif
    fclose(fpDebug);

    /************ FIM ARQUIVO DEBUG ***********/

    /************ ARQUIVO DEBUG_HR ***********/
#ifdef SET_DEBUG_TXT
    if((fpDebug = 1 ? fopen("/tmp/VirtualRAM/debug_HR.txt","w") : stdin) == NULL)
    {
      printf("Erro ao abrir /tmp/VirtualRAM/debug_HR.txt");
    }
    else
    {
      fprintf(fpDebug, "%s", "\n\n *** Debug Holding Register ***\n");
      fprintf(fpDebug,"%s %ld \n","TX:",SerialTXCounter);
      fprintf(fpDebug,"%s %ld \n\n","RX:",SerialRXCounter);

      for (dbg_addr=0; dbg_addr<MAX_HR_LIST_SIZE; dbg_addr++)
      {
        if (HRStartAddr[dbg_addr] == 0)
        {
          break;
        }

        fprintf(fpDebug, "Lista %d - Count %d \n", HRStartAddr[dbg_addr], HRCount[dbg_addr]);

        for (dbg_count=0; dbg_count<((HRCount[dbg_addr]*2)+3); dbg_count=dbg_count+8)
        {
          fprintf(fpDebug, "%02X - ", HRSerialBuffer[dbg_addr][dbg_count]);
          fprintf(fpDebug, "%02X - ", HRSerialBuffer[dbg_addr][dbg_count+1]);
          fprintf(fpDebug, "%02X - ", HRSerialBuffer[dbg_addr][dbg_count+2]);
          fprintf(fpDebug, "%02X - ", HRSerialBuffer[dbg_addr][dbg_count+3]);
          fprintf(fpDebug, "%02X - ", HRSerialBuffer[dbg_addr][dbg_count+4]);
          fprintf(fpDebug, "%02X - ", HRSerialBuffer[dbg_addr][dbg_count+5]);
          fprintf(fpDebug, "%02X - ", HRSerialBuffer[dbg_addr][dbg_count+6]);
          fprintf(fpDebug, "%02X \n", HRSerialBuffer[dbg_addr][dbg_count+7]);
        }

        fprintf(fpDebug, "%s", "\n\n");
      }

      fprintf(fpDebug, "%s", "Fim - arquivo\n\n");
    }
#else
    if((fpDebug = 1 ? fopen("/tmp/VirtualRAM/debug_HR.htm","w") : stdin) == NULL)
    {
      printf("Erro ao abrir /tmp/VirtualRAM/debug_HR.htm");
    }
    else
    {
      fprintf(fpDebug, "%s", "<html><body>");

      for (dbg_addr=0; dbg_addr<MAX_HR_LIST_SIZE; dbg_addr++)
      {
        if (HRStartAddr[dbg_addr] == 0)
        {
          break;
        }

        fprintf(fpDebug, "<b>Lista %d - Count %d</b><br><table cellpadding=""3"">", HRStartAddr[dbg_addr], HRCount[dbg_addr]);

        for (dbg_count=0; dbg_count<((HRCount[dbg_addr]*2)+3); dbg_count=dbg_count+8)
        {
          fprintf(fpDebug, "%s", "<tr>");
          fprintf(fpDebug, "<td> %02X</td>", HRSerialBuffer[dbg_addr][dbg_count]);
          fprintf(fpDebug, "<td> %02X</td>", HRSerialBuffer[dbg_addr][dbg_count+1]);
          fprintf(fpDebug, "<td> %02X</td>", HRSerialBuffer[dbg_addr][dbg_count+2]);
          fprintf(fpDebug, "<td> %02X</td>", HRSerialBuffer[dbg_addr][dbg_count+3]);
          fprintf(fpDebug, "<td> %02X</td>", HRSerialBuffer[dbg_addr][dbg_count+4]);
          fprintf(fpDebug, "<td> %02X</td>", HRSerialBuffer[dbg_addr][dbg_count+5]);
          fprintf(fpDebug, "<td> %02X</td>", HRSerialBuffer[dbg_addr][dbg_count+6]);
          fprintf(fpDebug, "<td> %02X</td>", HRSerialBuffer[dbg_addr][dbg_count+7]);
          fprintf(fpDebug, "%s", "</tr>");
        }

        fprintf(fpDebug, "%s", "</table><br>");
      }

      fprintf(fpDebug, "%s", "</body></html>");
    }
#endif
    fclose(fpDebug);
    /************ FIM ARQUIVO DEBUG ***********/





    if((ArqJS = 1 ? fopen("/tmp/VirtualRAM/arquivo.js","w") : stdin) == NULL)
    {
      printf("Erro ao abrir /tmp/VirtualRAM/arquivo.js");
    }
    else
    {
#if 1
      printf("\n Cria Arquivo GEMAP \n");
      printf("Tx: %ld \n",SerialTXCounter);
      printf("Rx: %ld \n\n",SerialRXCounter);

      printf("\n ******************************** Listas IR *****************************************\n");
      for(i=0;i<MAX_IR_LIST_SIZE;i++)
      {
        printf("Tx%d: %d \t",i,SerialTXListIR[i]);
        printf("Rx%d: %d \n",i,SerialRXListIR[i]);
      }
      printf("\n ******************************** Listas HR *****************************************\n");
      for(i=0;i<MAX_HR_LIST_SIZE;i++)
      {
        printf("Tx%d: %d \t",i,SerialTXListHR[i]);
        printf("Rx%d: %d \n",i,SerialRXListHR[i]);
      }
#endif
      // Abre arquivo
      fprintf(ArqJS, "%s", arq_ini);

      // Cria Titulo e Submenu (Visão Geral)


      fprintf(ArqJS, "%s%s", tit_visaogeral, submenu_visaogeral);

      // Data
      fprintf(ArqJS, "%s", var_Data1);
      sprintf(buffer_temp, "%02x/%02x/%02x%02x", IRSerialBuffer[LISTA_50][3], IRSerialBuffer[LISTA_50][4], IRSerialBuffer[LISTA_50][5], IRSerialBuffer[LISTA_50][6]);
      fprintf(ArqJS, "%s", buffer_temp);
      fprintf(ArqJS, "%s,", var_Data2);		/* adiciona virgula para cada proximo elemento do array */

      // Hora
      fprintf(ArqJS, "%s", var_Hora1);
      sprintf(buffer_temp, "%02x:%02x:%02x", IRSerialBuffer[LISTA_50][7], IRSerialBuffer[LISTA_50][8], IRSerialBuffer[LISTA_50][9]);
      fprintf(ArqJS, "%s", buffer_temp);
      fprintf(ArqJS, "%s,", var_Hora2);

      // Status do Sistema - Braço 1
      fprintf(ArqJS, "%s", var_Stat1_B1);
      temp_u16 = (IRSerialBuffer[LISTA_50][13]<<8) | IRSerialBuffer[LISTA_50][14];
      fprintf(ArqJS, "%s %s,", GetString_Tabela1(temp_u16), var_Stat2_B1);

#if defined ( SDM_F120 )
      // Status do Sistema - Braço 2
      fprintf(ArqJS, "%s", var_Stat1_B2);
      temp_u16 = (IRSerialBuffer[LISTA_50][69]<<8) | IRSerialBuffer[LISTA_50][70];
      fprintf(ArqJS, "%s %s,", GetString_Tabela1(temp_u16), var_Stat2_B2);
#endif
      // Volume Corrigido
      fprintf(ArqJS, "%s", var_VolCor1);
      ptr_u64 = (long long int *)&IRSerialBuffer[LISTA_50][47];
      (void)memcpy(&llu64Tmp,ptr_u64,8);
      llu64 = bswap_64(llu64Tmp);
      ptr_double = (double *)&llu64;
      fprintf(ArqJS, "%.1lf", *ptr_double);
      GetUnit(ArqJS, UNIT_VOL);
      fprintf(ArqJS, "%s,", var_VolCor2);

      // Volume bruto
      fprintf(ArqJS, "%s", var_VolBruto1);
      ptr_u64 = (long long int *)&IRSerialBuffer[LISTA_50][39];
      (void)memcpy(&llu64Tmp,ptr_u64,8);
      llu64 = bswap_64(llu64Tmp);
      ptr_double = (double *)&llu64;
      fprintf(ArqJS, "%.1lf", *ptr_double);
      GetUnit(ArqJS, UNIT_VOL);
      fprintf(ArqJS, "%s,", var_VolBruto2);

      // Vazao
      fprintf(ArqJS, "%s", var_Vazao1);
      ptr_u64 = (long long int *)&IRSerialBuffer[LISTA_50][15];
      (void)memcpy(&llu64Tmp,ptr_u64,8);
      llu64 = bswap_64(llu64Tmp);
      ptr_double = (double *)&llu64;
      fprintf(ArqJS, "%.1lf", *ptr_double);
      GetUnit(ArqJS, UNIT_FLOW);
      fprintf(ArqJS, "%s,", var_Vazao2);

      // Densidade
      fprintf(ArqJS, "%s", var_Dens1);
      ptr_u64 = (long long int *)&IRSerialBuffer[LISTA_50][31];
      (void)memcpy(&llu64Tmp,ptr_u64,8);
      llu64 = bswap_64(llu64Tmp);
      ptr_double = (double *)&llu64;
      fprintf(ArqJS, "%.1lf", *ptr_double);
      GetUnit(ArqJS, UNIT_DENS);
      fprintf(ArqJS, "%s,", var_Dens2);

      // Temperatura
      fprintf(ArqJS, "%s", var_Temp1);
      ptr_u64= (long long int *)&IRSerialBuffer[LISTA_50][23];
      (void)memcpy(&llu64Tmp,ptr_u64,8);
      llu64 = bswap_64(llu64Tmp);
      ptr_double = (double *)&llu64;
      fprintf(ArqJS, "%.2lf", *ptr_double);
      GetUnit(ArqJS, UNIT_TEMP);
      fprintf(ArqJS, "%s", var_Temp2);		/* ultimo elemento, não adiciona virgula */

      // Fecha Submenu e Titulo
      fprintf(ArqJS, "%s%s", submenu_fim, tit_fim);


      // Cria Titulo e Submenu (Sistema>>Principal)
      fprintf(ArqJS, "%s%s", tit_sistema, submenu_principal);


      // Data
      fprintf(ArqJS, "%s", var_Data1);
      sprintf(buffer_temp, "%02x/%02x/%02x%02x", IRSerialBuffer[LISTA_50][3], IRSerialBuffer[LISTA_50][4], IRSerialBuffer[LISTA_50][5], IRSerialBuffer[LISTA_50][6]);
      fprintf(ArqJS, "%s", buffer_temp);
      fprintf(ArqJS, "%s,", var_Data2);		/* adiciona virgula para cada proximo elemento do array */

      // Hora
      fprintf(ArqJS, "%s", var_Hora1);
      sprintf(buffer_temp, "%02x:%02x:%02x", IRSerialBuffer[LISTA_50][7], IRSerialBuffer[LISTA_50][8], IRSerialBuffer[LISTA_50][9]);
      fprintf(ArqJS, "%s", buffer_temp);
      fprintf(ArqJS, "%s,", var_Hora2);

      // Vazao
      fprintf(ArqJS, "%s", var_Vazao1);
      ptr_u64 = (long long int *)&IRSerialBuffer[LISTA_50][15];
      (void)memcpy(&llu64Tmp,ptr_u64,8);
      llu64 = bswap_64(llu64Tmp);
      ptr_double = (double *)&llu64;
      fprintf(ArqJS, "%.1lf", *ptr_double);
      GetUnit(ArqJS, UNIT_FLOW);
      fprintf(ArqJS, "%s,", var_Vazao2);

      // Temperatura
      fprintf(ArqJS, "%s", var_Temp1);
      ptr_u64 = (long long int *)&IRSerialBuffer[LISTA_50][23];
      (void)memcpy(&llu64Tmp,ptr_u64,8);
      llu64 = bswap_64(llu64Tmp);
      ptr_double = (double *)&llu64;
      fprintf(ArqJS, "%.2lf", *ptr_double);
      GetUnit(ArqJS, UNIT_TEMP);
      fprintf(ArqJS, "%s,", var_Temp2);

      // Densidade
      fprintf(ArqJS, "%s", var_Dens1);
      ptr_u64 = (long long int *)&IRSerialBuffer[LISTA_50][31];
      (void)memcpy(&llu64Tmp,ptr_u64,8);
      llu64 = bswap_64(llu64Tmp);
      ptr_double = (double *)&llu64;
      fprintf(ArqJS, "%.1lf", *ptr_double);
      GetUnit(ArqJS, UNIT_DENS);
      fprintf(ArqJS, "%s,", var_Dens2);

      // Volume bruto
      fprintf(ArqJS, "%s", var_VolBruto1);
      ptr_u64 = (long long int *)&IRSerialBuffer[LISTA_50][39];
      (void)memcpy(&llu64Tmp,ptr_u64,8);
      llu64 = bswap_64(llu64Tmp);
      ptr_double = (double *)&llu64;
      fprintf(ArqJS, "%.1lf", *ptr_double);
      GetUnit(ArqJS, UNIT_VOL);
      fprintf(ArqJS, "%s,", var_VolBruto2);

      // Volume Corrigido
      fprintf(ArqJS, "%s", var_VolCor1);
      ptr_u64 = (long long int *)&IRSerialBuffer[LISTA_50][47];
      (void)memcpy(&llu64Tmp,ptr_u64,8);
      llu64 = bswap_64(llu64Tmp);
      ptr_double = (double *)&llu64;
      fprintf(ArqJS, "%.1lf", *ptr_double);
      GetUnit(ArqJS, UNIT_VOL);
      fprintf(ArqJS, "%s,", var_VolCor2);

      // Status do Sistema - Braço 1
      fprintf(ArqJS, "%s", var_Stat1_B1);
      temp_u16 = (IRSerialBuffer[LISTA_50][13]<<8) | IRSerialBuffer[LISTA_50][14];
      fprintf(ArqJS, "%s %s,", GetString_Tabela1(temp_u16), var_Stat2_B1);

#if defined ( SDM_F120 )
      // Status do Sistema - Braço 2
      fprintf(ArqJS, "%s", var_Stat1_B2);
      temp_u16 = (IRSerialBuffer[LISTA_50][69]<<8) | IRSerialBuffer[LISTA_50][70];
      fprintf(ArqJS, "%s %s,", GetString_Tabela1(temp_u16), var_Stat2_B2);
#endif
      // Liberação Modo Remoto
      fprintf(ArqJS, "%s", var_ModoRem1);
      temp_u16 = (HRSerialBuffer[LISTA_HR_45][3]<<8) | HRSerialBuffer[LISTA_HR_45][4];
      fprintf(ArqJS, "%d %s,", temp_u16, var_ModoRem2);

      // Enable and disable list Operators and password
      fprintf(ArqJS, "%s", var_LtOpPass1);
      //Essa variável libera a lista de operadores e senha no ethernet
      ListaOperadoresSenhas = (HRSerialBuffer[LISTA_HR_1600][3]<<8) | HRSerialBuffer[LISTA_HR_1600][4];
      fprintf(ArqJS, "%d %s", ListaOperadoresSenhas, var_LtOpPass2);


      // Fecha Submenu
      fprintf(ArqJS, "%s,", submenu_fim);


      // Cria Submenu (Sistema>>Operação)
      fprintf(ArqJS, "%s", submenu_operacao);

      // Cód da Operação
      fprintf(ArqJS, "%s", var_CodOp1);
      memset(buffer_temp,0,sizeof(buffer_temp));
      strncpy(buffer_temp, (char *)&IRSerialBuffer[LISTA_100][3], 8);
      fprintf(ArqJS, "%s", buffer_temp);
      fprintf(ArqJS, "%s,", var_CodOp2);

      // Lacre
      fprintf(ArqJS, "%s", var_Lacre1);
      memset(buffer_temp,0,sizeof(buffer_temp));
      strncpy(buffer_temp, (char *)&IRSerialBuffer[LISTA_100][13], 8);
      fprintf(ArqJS, "%s", buffer_temp);
      fprintf(ArqJS, "%s,", var_Lacre2);

      // Operador
      fprintf(ArqJS, "%s", var_Operad1);
      memset(buffer_temp,0,sizeof(buffer_temp));
      strncpy(buffer_temp, (char *)&IRSerialBuffer[LISTA_100][23], 8);
      fprintf(ArqJS, "%s", buffer_temp);
      fprintf(ArqJS, "%s,", var_Operad2);

      // Veículo
      fprintf(ArqJS, "%s", var_IdVeic1);
      memset(buffer_temp,0,sizeof(buffer_temp));
      strncpy(buffer_temp, (char *)&IRSerialBuffer[LISTA_100][33], 8);
      fprintf(ArqJS, "%s", buffer_temp);
      fprintf(ArqJS, "%s,", var_IdVeic2);

      // Volume de Referencia
      fprintf(ArqJS, "%s", var_VolRef1);
      ptr_u32 = (unsigned int *)&IRSerialBuffer[LISTA_100][43];
      (void)memcpy(&llu32Tmp,ptr_u32,4);
      temp_u32 = bswap_32(llu32Tmp);
      fprintf(ArqJS, "%d", (temp_u32)/10);
      fprintf(ArqJS, "%s,", var_VolRef2);

      // Produto
      fprintf(ArqJS, "%s", var_Produto1);
      memset(buffer_temp,0,sizeof(buffer_temp));
      strncpy(buffer_temp, (char *)&IRSerialBuffer[LISTA_100][47], 10);
      fprintf(ArqJS, "%s", buffer_temp);
      fprintf(ArqJS, "%s,", var_Produto2);

      // Modo de operação
      fprintf(ArqJS, "%s", var_ModoOp1);
      temp_u16 = (IRSerialBuffer[LISTA_100][57]<<8) | IRSerialBuffer[LISTA_100][58];
      fprintf(ArqJS, "%s", GetString_Tabela2(temp_u16));
      fprintf(ArqJS, "%s,", var_ModoOp2);

      // Data de inicio da operação
      fprintf(ArqJS, "%s", var_DataIniOp1);
      sprintf(buffer_temp, "%02x/%02x/%02x%02x ", IRSerialBuffer[LISTA_130][3], IRSerialBuffer[LISTA_130][4], IRSerialBuffer[LISTA_130][5], IRSerialBuffer[LISTA_130][6]);	// Data
      fprintf(ArqJS, "%s", buffer_temp);
      sprintf(buffer_temp, "%02x:%02x:%02x", IRSerialBuffer[LISTA_130][7], IRSerialBuffer[LISTA_130][8], IRSerialBuffer[LISTA_130][9]);
      fprintf(ArqJS, "%s", buffer_temp);
      fprintf(ArqJS, "%s,", var_DataIniOp2);

      // Data do sistema
      fprintf(ArqJS, "%s", var_DataSist1);
      sprintf(buffer_temp, "%02x/%02x/%02x%02x ", IRSerialBuffer[LISTA_130][11], IRSerialBuffer[LISTA_130][12], IRSerialBuffer[LISTA_130][13], IRSerialBuffer[LISTA_130][14]);	// Data
      fprintf(ArqJS, "%s", buffer_temp);
      sprintf(buffer_temp, "%02x:%02x:%02x", IRSerialBuffer[LISTA_130][15], IRSerialBuffer[LISTA_130][16], IRSerialBuffer[LISTA_130][17]);
      fprintf(ArqJS, "%s", buffer_temp);
      fprintf(ArqJS, "%s,", var_DataSist2);

      // Volume bruto descarregado
      fprintf(ArqJS, "%s", var_VolBDesc1);
      ptr_u64 = (long long int *)&IRSerialBuffer[LISTA_130][19];
      (void)memcpy(&llu64Tmp,ptr_u64,8);
      llu64 = bswap_64(llu64Tmp);
      ptr_double = (double *)&llu64;
      fprintf(ArqJS, "%.1lf", *ptr_double);
      GetUnit(ArqJS, UNIT_VOL);
      fprintf(ArqJS, "%s,", var_VolBDesc2);

      // Volume corrigido descarregado
      fprintf(ArqJS, "%s", var_VolCDesc1);
      ptr_u64 = (long long int *)&IRSerialBuffer[LISTA_130][27];
      (void)memcpy(&llu64Tmp,ptr_u64,8);
      llu64 = bswap_64(llu64Tmp);
      ptr_double = (double *)&llu64;
      fprintf(ArqJS, "%.1lf", *ptr_double);
      GetUnit(ArqJS, UNIT_VOL);
      fprintf(ArqJS, "%s,", var_VolCDesc2);

      // Média da vazão (c/ temperatura compensada)
      fprintf(ArqJS, "%s", var_MedVazao1);
      ptr_u32 = (unsigned int *)&IRSerialBuffer[LISTA_130][35];		/* usar ponteiro int para byte desalinhado */
      (void)memcpy(&llu32Tmp,ptr_u32,4);
      temp_u32 = bswap_32(llu32Tmp);		     		/* swap para corrigir byte-order */
      ptr_float = (float *)&temp_u32;					/* cast para exibir em float */
      fprintf(ArqJS, "%.2f", *ptr_float);
      GetUnit(ArqJS, UNIT_FLOW);
      fprintf(ArqJS, "%s,", var_MedVazao2);

      // Média da temperatura
      fprintf(ArqJS, "%s", var_MedTemp1);
      ptr_u32 = (unsigned int *)&IRSerialBuffer[LISTA_130][39];		/* usar ponteiro int para byte desalinhado */
      (void)memcpy(&llu32Tmp,ptr_u32,4);
      temp_u32 = bswap_32(llu32Tmp);     				/* swap para corrigir byte-order */
      ptr_float = (float *)&temp_u32;					/* cast para exibir em float */
      fprintf(ArqJS, "%.2f", *ptr_float);
      GetUnit(ArqJS, UNIT_TEMP);
      fprintf(ArqJS, "%s,", var_MedTemp2);

      // Média da densidade (c/ temperatura compensada)
      fprintf(ArqJS, "%s", var_MedDens1);
      ptr_u64 = (long long int *)&IRSerialBuffer[LISTA_130][43];
      (void)memcpy(&llu64Tmp,ptr_u64,8);
      llu64 = bswap_64(llu64Tmp);
      ptr_double = (double *)&llu64;
      fprintf(ArqJS, "%.1lf", *ptr_double);
      GetUnit(ArqJS, UNIT_DENS);
      fprintf(ArqJS, "%s,", var_MedDens2);

      // Vazão máxima (c/ temperatura compensada)
      fprintf(ArqJS, "%s", var_VazaoMax1);
      ptr_u32 = (unsigned int *)&IRSerialBuffer[LISTA_130][51];		/* usar ponteiro int para byte desalinhado */
      (void)memcpy(&llu32Tmp,ptr_u32,4);
      temp_u32 = bswap_32(llu32Tmp);   				/* swap para corrigir byte-order */
      ptr_float = (float *)&temp_u32;					/* cast para exibir em float */
      fprintf(ArqJS, "%.1f", *ptr_float);
      GetUnit(ArqJS, UNIT_FLOW);
      fprintf(ArqJS, "%s,", var_VazaoMax2);

      // Temperatura máxima
      fprintf(ArqJS, "%s", var_TempMax1);
      ptr_u32 = (unsigned int *)&IRSerialBuffer[LISTA_130][55];		/* usar ponteiro int para byte desalinhado */
      (void)memcpy(&llu32Tmp,ptr_u32,4);
      temp_u32 = bswap_32(llu32Tmp);   				/* swap para corrigir byte-order */
      ptr_float = (float *)&temp_u32;					/* cast para exibir em float */
      fprintf(ArqJS, "%.2f", *ptr_float);
      GetUnit(ArqJS, UNIT_TEMP);
      fprintf(ArqJS, "%s,", var_TempMax2);

      // Temperatura minima
      fprintf(ArqJS, "%s", var_TempMin1);
      ptr_u32 = (unsigned int *)&IRSerialBuffer[LISTA_130][59];		/* usar ponteiro int para byte desalinhado */
      (void)memcpy(&llu32Tmp,ptr_u32,4);
      temp_u32 = bswap_32(llu32Tmp);  				/* swap para corrigir byte-order */
      ptr_float = (float *)&temp_u32;					/* cast para exibir em float */
      fprintf(ArqJS, "%.2f", *ptr_float);
      GetUnit(ArqJS, UNIT_TEMP);
      fprintf(ArqJS, "%s,", var_TempMin2);


      /* 160 */
      // Densidade máxima (c/ temperatura compensada)
      fprintf(ArqJS, "%s", var_DensMax1);
      ptr_u64 = (long long int *)&IRSerialBuffer[LISTA_160][3];
      (void)memcpy(&llu64Tmp,ptr_u64,8);
      llu64 = bswap_64(llu64Tmp);
      ptr_double = (double *)&llu64;
      fprintf(ArqJS, "%.1lf", *ptr_double);
      GetUnit(ArqJS, UNIT_DENS);
      fprintf(ArqJS, "%s,", var_DensMax2);

      // Densidade minima (c/ temperatura compensada)
      fprintf(ArqJS, "%s", var_DensMin1);
      ptr_u64 = (long long int *)&IRSerialBuffer[LISTA_160][11];
      (void)memcpy(&llu64Tmp,ptr_u64,8);
      llu64 = bswap_64(llu64Tmp);
      ptr_double = (double *)&llu64;
      fprintf(ArqJS, "%.1lf", *ptr_double);
      GetUnit(ArqJS, UNIT_DENS);
      fprintf(ArqJS, "%s,", var_DensMin2);

      // Inventário inicial do volume bruto
      fprintf(ArqJS, "%s", var_InvVolBrutoIni1);
      ptr_u64 = (long long int *)&IRSerialBuffer[LISTA_160][19];
      (void)memcpy(&llu64Tmp,ptr_u64,8);
      llu64 = bswap_64(llu64Tmp);
      ptr_double = (double *)&llu64;
      fprintf(ArqJS, "%.1lf", *ptr_double);
      GetUnit(ArqJS, UNIT_VOL);
      fprintf(ArqJS, "%s,", var_InvVolBrutoIni2);

      // Inventário inicial do volume corrigido
      fprintf(ArqJS, "%s", var_InvVolCorIni1);
      ptr_u64 = (long long int *)&IRSerialBuffer[LISTA_160][27];
      (void)memcpy(&llu64Tmp,ptr_u64,8);
      llu64 = bswap_64(llu64Tmp);
      ptr_double = (double *)&llu64;
      fprintf(ArqJS, "%.1lf", *ptr_double);
      GetUnit(ArqJS, UNIT_VOL);
      fprintf(ArqJS, "%s,", var_InvVolCorIni2);

      // Inventário do volume bruto
      fprintf(ArqJS, "%s", var_InvVolBruto1);
      ptr_u64 = (long long int *)&IRSerialBuffer[LISTA_160][35];
      (void)memcpy(&llu64Tmp,ptr_u64,8);
      llu64 = bswap_64(llu64Tmp);
      ptr_double = (double *)&llu64;
      fprintf(ArqJS, "%.1lf", *ptr_double);
      GetUnit(ArqJS, UNIT_VOL);
      fprintf(ArqJS, "%s,", var_InvVolBruto2);

      // Inventário do volume corrigido
      fprintf(ArqJS, "%s", var_InvVolCor1);
      ptr_u64 = (long long int *)&IRSerialBuffer[LISTA_160][43];
      (void)memcpy(&llu64Tmp,ptr_u64,8);
      llu64 = bswap_64(llu64Tmp);
      ptr_double = (double *)&llu64;
      fprintf(ArqJS, "%.1lf", *ptr_double);
      GetUnit(ArqJS, UNIT_VOL);
      fprintf(ArqJS, "%s", var_InvVolCor2);

      // Fecha Submenu
      fprintf(ArqJS, "%s,", submenu_fim);


      /* ****************************  ÚLTIMA OPERAÇÃO ***************************************/
      // Cria Submenu (Sistema>>Última Operação)
      fprintf(ArqJS, "%s", submenu_operacao_old);

      /* 200 */
      // Cód da Operação
      fprintf(ArqJS, "%s", var_CodOp1_ul);
      memset(buffer_temp,0,sizeof(buffer_temp));
      strncpy(buffer_temp, (char *)&IRSerialBuffer[LISTA_200][3], 8);
      fprintf(ArqJS, "%s", buffer_temp);
      fprintf(ArqJS, "%s,", var_CodOp2_ul);

      // Lacre
      fprintf(ArqJS, "%s", var_Lacre1_ul);
      memset(buffer_temp,0,sizeof(buffer_temp));
      strncpy(buffer_temp, (char *)&IRSerialBuffer[LISTA_200][13], 8);
      fprintf(ArqJS, "%s", buffer_temp);
      fprintf(ArqJS, "%s,", var_Lacre2_ul);

      // Operador
      fprintf(ArqJS, "%s", var_Operad1_ul);
      memset(buffer_temp,0,sizeof(buffer_temp));
      strncpy(buffer_temp, (char *)&IRSerialBuffer[LISTA_200][23], 8);
      fprintf(ArqJS, "%s", buffer_temp);
      fprintf(ArqJS, "%s,", var_Operad2_ul);

      // Veículo
      fprintf(ArqJS, "%s", var_IdVeic1_ul);
      memset(buffer_temp,0,sizeof(buffer_temp));
      strncpy(buffer_temp, (char *)&IRSerialBuffer[LISTA_200][33], 8);
      fprintf(ArqJS, "%s", buffer_temp);
      fprintf(ArqJS, "%s,", var_IdVeic2_ul);

      // Volume de Referencia
      fprintf(ArqJS, "%s", var_VolRef1_ul);
      ptr_u32 = (unsigned int *)&IRSerialBuffer[LISTA_200][43];
      (void)memcpy(&llu32Tmp,ptr_u32,4);
      temp_u32 = bswap_32(llu32Tmp);
      fprintf(ArqJS, "%d", (temp_u32)/10);
      fprintf(ArqJS, "%s,", var_VolRef2_ul);

      // Produto
      fprintf(ArqJS, "%s", var_Produto1);
      memset(buffer_temp,0,sizeof(buffer_temp));
      strncpy(buffer_temp, (char *)&IRSerialBuffer[LISTA_200][47], 10);
      fprintf(ArqJS, "%s", buffer_temp);
      fprintf(ArqJS, "%s,", var_Produto2);

      // Modo de operação
      fprintf(ArqJS, "%s", var_ModoOp1_ul);
      temp_u16 = (IRSerialBuffer[LISTA_200][57]<<8) | IRSerialBuffer[LISTA_200][58];
      fprintf(ArqJS, "%s", GetString_Tabela2(temp_u16));
      fprintf(ArqJS, "%s,", var_ModoOp2_ul);

      /* 230 */
      // Data de inicio da operação
      fprintf(ArqJS, "%s", var_DataIniOp1);
      sprintf(buffer_temp, "%02x/%02x/%02x%02x ", IRSerialBuffer[LISTA_230][3], IRSerialBuffer[LISTA_230][4], IRSerialBuffer[LISTA_230][5], IRSerialBuffer[LISTA_230][6]);	// Data
      fprintf(ArqJS, "%s", buffer_temp);
      sprintf(buffer_temp, "%02x:%02x:%02x", IRSerialBuffer[LISTA_230][7], IRSerialBuffer[LISTA_230][8], IRSerialBuffer[LISTA_230][9]);
      fprintf(ArqJS, "%s", buffer_temp);
      fprintf(ArqJS, "%s,", var_DataIniOp2);

      // Data do sistema
      fprintf(ArqJS, "%s", var_DataSist1);
      sprintf(buffer_temp, "%02x/%02x/%02x%02x ", IRSerialBuffer[LISTA_230][11], IRSerialBuffer[LISTA_230][12], IRSerialBuffer[LISTA_230][13], IRSerialBuffer[LISTA_230][14]);	// Data
      fprintf(ArqJS, "%s", buffer_temp);
      sprintf(buffer_temp, "%02x:%02x:%02x", IRSerialBuffer[LISTA_230][15], IRSerialBuffer[LISTA_230][16], IRSerialBuffer[LISTA_230][17]);
      fprintf(ArqJS, "%s", buffer_temp);
      fprintf(ArqJS, "%s,", var_DataSist2);

      // Volume bruto descarregado
      fprintf(ArqJS, "%s", var_VolBDesc1);
      ptr_u64 = (long long int *)&IRSerialBuffer[LISTA_230][19];
      (void)memcpy(&llu64Tmp,ptr_u64,8);
      llu64 = bswap_64(llu64Tmp);
      ptr_double = (double *)&llu64;
      fprintf(ArqJS, "%.1lf", *ptr_double);
      GetUnit(ArqJS, UNIT_VOL);
      fprintf(ArqJS, "%s,", var_VolBDesc2);

      // Volume corrigido descarregado
      fprintf(ArqJS, "%s", var_VolCDesc1);
      ptr_u64 = (long long int *)&IRSerialBuffer[LISTA_230][27];
      (void)memcpy(&llu64Tmp,ptr_u64,8);
      llu64 = bswap_64(llu64Tmp);
      ptr_double = (double *)&llu64;
      fprintf(ArqJS, "%.1lf", *ptr_double);
      GetUnit(ArqJS, UNIT_VOL);
      fprintf(ArqJS, "%s,", var_VolCDesc2);

      // Média da vazão (c/ temperatura compensada)
      fprintf(ArqJS, "%s", var_MedVazao1);
      ptr_u32 = (unsigned int *)&IRSerialBuffer[LISTA_230][35];		/* usar ponteiro int para byte desalinhado */
      (void)memcpy(&llu32Tmp,ptr_u32,4);
      temp_u32 = bswap_32(llu32Tmp);  				/* swap para corrigir byte-order */
      ptr_float = (float *)&temp_u32;					/* cast para exibir em float */
      fprintf(ArqJS, "%.1f", *ptr_float);
      GetUnit(ArqJS, UNIT_FLOW);
      fprintf(ArqJS, "%s,", var_MedVazao2);

      // Média da temperatura
      fprintf(ArqJS, "%s", var_MedTemp1);
      ptr_u32 = (unsigned int *)&IRSerialBuffer[LISTA_230][39];		/* usar ponteiro int para byte desalinhado */
      (void)memcpy(&llu32Tmp,ptr_u32,4);
      temp_u32 = bswap_32(llu32Tmp);  				/* swap para corrigir byte-order */
      ptr_float = (float *)&temp_u32;					/* cast para exibir em float */
      fprintf(ArqJS, "%.2f", *ptr_float);
      GetUnit(ArqJS, UNIT_TEMP);
      fprintf(ArqJS, "%s,", var_MedTemp2);

      // Média da densidade (c/ temperatura compensada)
      fprintf(ArqJS, "%s", var_MedDens1);
      ptr_u64 = (long long int *)&IRSerialBuffer[LISTA_230][43];
      (void)memcpy(&llu64Tmp,ptr_u64,8);
      llu64 = bswap_64(llu64Tmp);
      ptr_double = (double *)&llu64;
      fprintf(ArqJS, "%.1lf", *ptr_double);
      GetUnit(ArqJS, UNIT_DENS);
      fprintf(ArqJS, "%s,", var_MedDens2);

      // Vazão máxima (c/ temperatura compensada)
      fprintf(ArqJS, "%s", var_VazaoMax1);
      ptr_u32 = (unsigned int *)&IRSerialBuffer[LISTA_230][51];		/* usar ponteiro int para byte desalinhado */
      (void)memcpy(&llu32Tmp,ptr_u32,4);
      temp_u32 = bswap_32(llu32Tmp);  				/* swap para corrigir byte-order */
      ptr_float = (float *)&temp_u32;					/* cast para exibir em float */
      fprintf(ArqJS, "%.1f", *ptr_float);
      GetUnit(ArqJS, UNIT_FLOW);
      fprintf(ArqJS, "%s,", var_VazaoMax2);

      // Temperatura máxima
      fprintf(ArqJS, "%s", var_TempMax1);
      ptr_u32 = (unsigned int *)&IRSerialBuffer[LISTA_230][55];		/* usar ponteiro int para byte desalinhado */
      (void)memcpy(&llu32Tmp,ptr_u32,4);
      temp_u32 = bswap_32(llu32Tmp);  				/* swap para corrigir byte-order */
      ptr_float = (float *)&temp_u32;					/* cast para exibir em float */
      fprintf(ArqJS, "%.2f", *ptr_float);
      GetUnit(ArqJS, UNIT_TEMP);
      fprintf(ArqJS, "%s,", var_TempMax2);

      // Temperatura minima
      fprintf(ArqJS, "%s", var_TempMin1);
      ptr_u32 = (unsigned int *)&IRSerialBuffer[LISTA_230][59];		/* usar ponteiro int para byte desalinhado */
      (void)memcpy(&llu32Tmp,ptr_u32,4);
      temp_u32 = bswap_32(llu32Tmp);  				/* swap para corrigir byte-order */
      ptr_float = (float *)&temp_u32;					/* cast para exibir em float */
      fprintf(ArqJS, "%.2f", *ptr_float);
      GetUnit(ArqJS, UNIT_TEMP);
      fprintf(ArqJS, "%s,", var_TempMin2);

      /* 260 */
      // Densidade máxima (c/ temperatura compensada)
      fprintf(ArqJS, "%s", var_DensMax1);
      ptr_u64 = (long long int *)&IRSerialBuffer[LISTA_260][3];
      (void)memcpy(&llu64Tmp,ptr_u64,8);
      llu64 = bswap_64(llu64Tmp);
      ptr_double = (double *)&llu64;
      fprintf(ArqJS, "%.1lf", *ptr_double);
      GetUnit(ArqJS, UNIT_DENS);
      fprintf(ArqJS, "%s,", var_DensMax2);

      // Densidade minima (c/ temperatura compensada)
      fprintf(ArqJS, "%s", var_DensMin1);
      ptr_u64 = (long long int *)&IRSerialBuffer[LISTA_260][11];
      (void)memcpy(&llu64Tmp,ptr_u64,8);
      llu64 = bswap_64(llu64Tmp);
      ptr_double = (double *)&llu64;
      fprintf(ArqJS, "%.1lf", *ptr_double);
      GetUnit(ArqJS, UNIT_DENS);
      fprintf(ArqJS, "%s,", var_DensMin2);

      // Inventário inicial do volume bruto
      fprintf(ArqJS, "%s", var_InvVolBrutoIni1);
      ptr_u64 = (long long int *)&IRSerialBuffer[LISTA_260][19];
      (void)memcpy(&llu64Tmp,ptr_u64,8);
      llu64 = bswap_64(llu64Tmp);
      ptr_double = (double *)&llu64;
      fprintf(ArqJS, "%.1lf", *ptr_double);
      GetUnit(ArqJS, UNIT_VOL);
      fprintf(ArqJS, "%s,", var_InvVolBrutoIni2);

      // Inventário inicial do volume corrigido
      fprintf(ArqJS, "%s", var_InvVolCorIni1);
      ptr_u64 = (long long int *)&IRSerialBuffer[LISTA_260][27];
      (void)memcpy(&llu64Tmp,ptr_u64,8);
      llu64 = bswap_64(llu64Tmp);
      ptr_double = (double *)&llu64;
      fprintf(ArqJS, "%.1lf", *ptr_double);
      GetUnit(ArqJS, UNIT_VOL);
      fprintf(ArqJS, "%s,", var_InvVolCorIni2);

      // Inventário do volume bruto
      fprintf(ArqJS, "%s", var_InvVolBruto1);
      ptr_u64 = (long long int *)&IRSerialBuffer[LISTA_260][35];
      (void)memcpy(&llu64Tmp,ptr_u64,8);
      llu64 = bswap_64(llu64Tmp);
      ptr_double = (double *)&llu64;
      fprintf(ArqJS, "%.1lf", *ptr_double);
      GetUnit(ArqJS, UNIT_VOL);
      fprintf(ArqJS, "%s,", var_InvVolBruto2);

      // Inventário do volume corrigido
      fprintf(ArqJS, "%s", var_InvVolCor1);
      ptr_u64 = (long long int *)&IRSerialBuffer[LISTA_260][43];
      (void)memcpy(&llu64Tmp,ptr_u64,8);
      llu64 = bswap_64(llu64Tmp);
      ptr_double = (double *)&llu64;
      fprintf(ArqJS, "%.1lf", *ptr_double);
      GetUnit(ArqJS, UNIT_VOL);
      fprintf(ArqJS, "%s", var_InvVolCor2);


      // Fecha Submenu
      fprintf(ArqJS, "%s,", submenu_fim);


      /* ****************************  DIAGNÓSTICOS ***************************************/
      /* 300 */
      // Cria Submenu (Sistema>>Diagnósticos)
      fprintf(ArqJS, "%s", submenu_diag);

      for (i=0; i<12; i++)
      {
        /* (i*2) incremento a cada 16 bits
         * (+3) offset do inicio dos dados no frame modbus
         */
        fprintf(ArqJS, "%s", var_Sist_Diag_1[i]);
        temp_u16 = (IRSerialBuffer[LISTA_300][(i*2)+3]<<8) | IRSerialBuffer[LISTA_300][(i*2)+4];
        fprintf(ArqJS, "%d %s,", temp_u16, var_Sist_Diag_2[i]);
      }

      // Valor MC1 AI-1 [Reserva]
      fprintf(ArqJS, "%s", var_Sist_Diag_1[12]);
      ptr_u32 = (unsigned int *)&IRSerialBuffer[LISTA_300][27];		/* usar ponteiro int para byte desalinhado */
      (void)memcpy(&llu32Tmp,ptr_u32,4);
      temp_u32 = bswap_32(llu32Tmp);  						/* swap para corrigir byte-order */
      ptr_float = (float *)&temp_u32;							/* cast para exibir em float */
      fprintf(ArqJS, "%.0f %s,", *ptr_float, var_Sist_Diag_2[12]);

      // Status MC1 DI-7 [Monitor de Aterramento B1]
      fprintf(ArqJS, "%s", var_Sist_Diag_1[13]);
      ptr_u32 = (unsigned int *)&IRSerialBuffer[LISTA_300][31];		/* usar ponteiro int para byte desalinhado */
      (void)memcpy(&llu32Tmp,ptr_u32,4);
      temp_u32 = bswap_32(llu32Tmp);  						/* swap para corrigir byte-order */
      ptr_float = (float *)&temp_u32;							/* cast para exibir em float */
      fprintf(ArqJS, "%.0f %s,", *ptr_float, var_Sist_Diag_2[13]);

      // Status MC1 DI-8 [Botão de Emergência]
      fprintf(ArqJS, "%s", var_Sist_Diag_1[14]);
      ptr_u32 = (unsigned int *)&IRSerialBuffer[LISTA_300][35];		/* usar ponteiro int para byte desalinhado */
      (void)memcpy(&llu32Tmp,ptr_u32,4);
      temp_u32 = bswap_32(llu32Tmp);  						/* swap para corrigir byte-order */
      ptr_float = (float *)&temp_u32;							/* cast para exibir em float */
#if defined ( SDM_F120 )
      fprintf(ArqJS, "%.0f %s,", *ptr_float, var_Sist_Diag_2[14]);
#else
      fprintf(ArqJS, "%.0f %s", *ptr_float, var_Sist_Diag_2[14]);
#endif


#if defined ( SDM_F120 )
      for (i=15; i<27; i++)
      {
        fprintf(ArqJS, "%s", var_Sist_Diag_1[i]);
        temp_u16 = (IRSerialBuffer[LISTA_300][(i*2)+9]<<8) | IRSerialBuffer[LISTA_300][(i*2)+10];	/* (+9) Considerar offset dos 3 floats anteriores */
        fprintf(ArqJS, "%d %s,", temp_u16, var_Sist_Diag_2[i]);
      }

      // Valor MC2 AI-1 [Tensão de Alimentação Vac]
      fprintf(ArqJS, "%s", var_Sist_Diag_1[27]);
      ptr_u32 = (unsigned int *)&IRSerialBuffer[LISTA_300][63];		/* usar ponteiro int para byte desalinhado */
      (void)memcpy(&llu32Tmp,ptr_u32,4);
      temp_u32 = bswap_32(llu32Tmp);  				/* swap para corrigir byte-order */
      ptr_float = (float *)&temp_u32;					/* cast para exibir em float */
      fprintf(ArqJS, "%.2f %s,", *ptr_float, var_Sist_Diag_2[27]);

      // Status MC2 DI-7 [Monitor de Aterramento B1]
      fprintf(ArqJS, "%s", var_Sist_Diag_1[28]);
      ptr_u32 = (unsigned int *)&IRSerialBuffer[LISTA_300][67];		/* usar ponteiro int para byte desalinhado */
      (void)memcpy(&llu32Tmp,ptr_u32,4);
      temp_u32 = bswap_32(llu32Tmp);  				/* swap para corrigir byte-order */
      ptr_float = (float *)&temp_u32;					/* cast para exibir em float */
      fprintf(ArqJS, "%.0f %s,", *ptr_float, var_Sist_Diag_2[28]);

      // Status MC2 DI-8 [Botão de Emergência]
      fprintf(ArqJS, "%s", var_Sist_Diag_1[29]);
      ptr_u32 = (unsigned int *)&IRSerialBuffer[LISTA_300][71];		/* usar ponteiro int para byte desalinhado */
      (void)memcpy(&llu32Tmp,ptr_u32,4);
      temp_u32 = bswap_32(llu32Tmp);  				/* swap para corrigir byte-order */
      ptr_float = (float *)&temp_u32;					/* cast para exibir em float */
      fprintf(ArqJS, "%.0f %s", *ptr_float, var_Sist_Diag_2[29]);
#endif

      // Fecha Submenu
      fprintf(ArqJS, "%s,", submenu_fim);



      /* ****************************  ALARMES  ***************************************/
      /* 50 */
      // Cria Submenu (Sistema>>Alarmes)
      fprintf(ArqJS, "%s", submenu_alarmes);

      // Alarme Erro
      fprintf(ArqJS, "%s", var_Alm_Err_1);
      temp_u16 = (IRSerialBuffer[LISTA_50][55]<<8) | IRSerialBuffer[LISTA_50][56];
      fprintf(ArqJS, "0x%04X %s,", temp_u16, var_Alm_Err_2);
#if defined ( SDM_F120 )
      // Alarme Erro 2
      fprintf(ArqJS, "%s", var_Alm_Inf_1);
      temp_u16 = (IRSerialBuffer[LISTA_50][59]<<8) | IRSerialBuffer[LISTA_50][60];
      fprintf(ArqJS, "0x%04X %s,", temp_u16, var_Alm_Inf_2);
#endif
      // Alarme Warning
      fprintf(ArqJS, "%s", var_Alm_War_1);
      temp_u16 = (IRSerialBuffer[LISTA_50][57]<<8) | IRSerialBuffer[LISTA_50][58];
      fprintf(ArqJS, "0x%04X %s", temp_u16, var_Alm_War_2);


      // Fecha Submenu
      fprintf(ArqJS, "%s,", submenu_fim);



      /* ****************************  INFORMAÇÕES  ***************************************/
      /* 500 */
      // Cria Submenu (Sistema>>Informações)
      fprintf(ArqJS, "%s", submenu_info);

      // TAG
      fprintf(ArqJS, "%s", var_TAGSys_1);
      memset(buffer_temp,0,sizeof(buffer_temp));
      strncpy(buffer_temp, (char *)&IRSerialBuffer[LISTA_500][13], 16);
      fprintf(ArqJS, "%s", buffer_temp);
      fprintf(ArqJS, "%s,", var_TAGSys_2);

      // Número de série
      fprintf(ArqJS, "%s", var_NumSer_1);
      ptr_u32 = (unsigned int *)&IRSerialBuffer[LISTA_500][29];
      (void)memcpy(&llu32Tmp,ptr_u32,4);
      temp_u32 = bswap_32(llu32Tmp);
      fprintf(ArqJS, "%d %s,", temp_u32, var_NumSer_2);

      // Data da última inicialização
      fprintf(ArqJS, "%s", var_Data_ult_ini_1);
      sprintf(buffer_temp, "%02x/%02x/%02x%02x ", IRSerialBuffer[LISTA_500][5], IRSerialBuffer[LISTA_500][6], IRSerialBuffer[LISTA_500][7], IRSerialBuffer[LISTA_500][8]);	// Data
      fprintf(ArqJS, "%s", buffer_temp);
      sprintf(buffer_temp, "%02x:%02x:%02x", IRSerialBuffer[LISTA_500][9], IRSerialBuffer[LISTA_500][10], IRSerialBuffer[LISTA_500][11]);
      fprintf(ArqJS, "%s%s,", buffer_temp, var_Data_ult_ini_2);

      // Número de ciclos de inicialização
      fprintf(ArqJS, "%s", var_Ciclos_1);
      temp_u16 = (IRSerialBuffer[LISTA_500][3]<<8) | IRSerialBuffer[LISTA_500][4];
      fprintf(ArqJS, "%d %s,", temp_u16, var_Ciclos_2);

      // Contador de descarregamento
      fprintf(ArqJS, "%s", var_CntDesc_1);
      temp_u16 = (IRSerialBuffer[LISTA_500][33]<<8) | IRSerialBuffer[LISTA_500][34];
      fprintf(ArqJS, "%d %s", temp_u16, var_CntDesc_2);

      // Fecha Submenu e titulo
      fprintf(ArqJS, "%s%s", submenu_fim, tit_fim);



      /* ****************************  MODULO RTU >> DIAGNOSTICOS  ***************************************/
      // Cria Titulo e Submenu (Módulo RTU>>Diagnosticos de hardware)
      fprintf(ArqJS, "%s%s", tit_mod_RTU, submenu_diag_hw);

      // Tensão de rede
      fprintf(ArqJS, "%s", var_Tensao_rede_1);
      ptr_u32 = (unsigned int *)&IRSerialBuffer[LISTA_400][3];		/* usar ponteiro int para byte desalinhado */
      (void)memcpy(&llu32Tmp,ptr_u32,4);
      temp_u32 = bswap_32(llu32Tmp);  				/* swap para corrigir byte-order */
      ptr_float = (float *)&temp_u32;					/* cast para exibir em float */
      fprintf(ArqJS, "%.2f %s,", *ptr_float, var_Tensao_rede_2);

      // Tensão VCC
      fprintf(ArqJS, "%s", var_Tensao_vcc_1);
      ptr_u32 = (unsigned int *)&IRSerialBuffer[LISTA_400][7];		/* usar ponteiro int para byte desalinhado */
      (void)memcpy(&llu32Tmp,ptr_u32,4);
      temp_u32 = bswap_32(llu32Tmp);   				/* swap para corrigir byte-order */
      ptr_float = (float *)&temp_u32;					/* cast para exibir em float */
      fprintf(ArqJS, "%.2f %s,", *ptr_float, var_Tensao_vcc_2);

      // Temperatura da placa
      fprintf(ArqJS, "%s", var_Temp_placa_1);
      ptr_u32 = (unsigned int *)&IRSerialBuffer[LISTA_400][11];		/* usar ponteiro int para byte desalinhado */
      (void)memcpy(&llu32Tmp,ptr_u32,4);
      temp_u32 = bswap_32(llu32Tmp);  				/* swap para corrigir byte-order */
      ptr_float = (float *)&temp_u32;					/* cast para exibir em float */
      fprintf(ArqJS, "%.1f %s,", *ptr_float, var_Temp_placa_2);

      // Tensão da placa
      fprintf(ArqJS, "%s", var_Tensao_placa_1);
      ptr_u32 = (unsigned int *)&IRSerialBuffer[LISTA_400][15];		/* usar ponteiro int para byte desalinhado */
      (void)memcpy(&llu32Tmp,ptr_u32,4);
      temp_u32 = bswap_32(llu32Tmp);  				/* swap para corrigir byte-order */
      ptr_float = (float *)&temp_u32;					/* cast para exibir em float */
      fprintf(ArqJS, "%.2f %s,", *ptr_float, var_Tensao_placa_2);

      // Tensão da bateria
      fprintf(ArqJS, "%s", var_Tensao_bat_1);
      ptr_u32 = (unsigned int *)&IRSerialBuffer[LISTA_400][19];		/* usar ponteiro int para byte desalinhado */
      (void)memcpy(&llu32Tmp,ptr_u32,4);
      temp_u32 = bswap_32(llu32Tmp);  				/* swap para corrigir byte-order */
      ptr_float = (float *)&temp_u32;					/* cast para exibir em float */
      fprintf(ArqJS, "%.2f %s,", *ptr_float, var_Tensao_bat_2);

      // Tensão de entrada
      fprintf(ArqJS, "%s", var_Tensao_ent_1);
      ptr_u32 = (unsigned int *)&IRSerialBuffer[LISTA_400][23];		/* usar ponteiro int para byte desalinhado */
      (void)memcpy(&llu32Tmp,ptr_u32,4);
      temp_u32 = bswap_32(llu32Tmp);  				/* swap para corrigir byte-order */
      ptr_float = (float *)&temp_u32;					/* cast para exibir em float */
      fprintf(ArqJS, "%.2f %s", *ptr_float, var_Tensao_ent_2);

      // Fecha Submenu
      fprintf(ArqJS, "%s,", submenu_fim);


      /* ****************************  MODULO RTU >> CONFIGURAÇÃO  ***************************************/
      // Cria Titulo e Submenu (Módulo RTU>>Configurações DI)
      fprintf(ArqJS, "%s", submenu_config_DI);

      // 6 DI's MC1 + 6 DI's MC2
#if defined ( SDM_F120 )
      for (i=0; i<12; i++)
#else
      for (i=0; i<6; i++)
#endif
      {
        // Configuração DI
        fprintf(ArqJS, "%s", var_ConfigDI_1[i*3]);
        temp_u16 = (HRSerialBuffer[LISTA_HR_1200][(i*6)+3]<<8) | HRSerialBuffer[LISTA_HR_1200][(i*6)+4];
        fprintf(ArqJS, "0x%04X %s,", temp_u16, var_ConfigDI_2[i*3]);

        // Parâmetro 1 DI
        fprintf(ArqJS, "%s", var_ConfigDI_1[(i*3)+1]);
        temp_u16 = (HRSerialBuffer[LISTA_HR_1200][(i*6)+5]<<8) | HRSerialBuffer[LISTA_HR_1200][(i*6)+6];
        fprintf(ArqJS, "%d %s,", temp_u16, var_ConfigDI_2[(i*3)+1]);

        // Parâmetro 2 DI
        fprintf(ArqJS, "%s", var_ConfigDI_1[(i*3)+2]);
        temp_u16 = (HRSerialBuffer[LISTA_HR_1200][(i*6)+7]<<8) | HRSerialBuffer[LISTA_HR_1200][(i*6)+8];

#if defined ( SDM_F120 )
        if (i == 11) // Não coloca virgula no ultimo elemento
#else
        if (i == 5)
#endif
        {
          fprintf(ArqJS, "%d %s", temp_u16, var_ConfigDI_2[(i*3)+2]);
        }
        else
        {
          fprintf(ArqJS, "%d %s,", temp_u16, var_ConfigDI_2[(i*3)+2]);
        }
      }

      // Fecha Submenu
      fprintf(ArqJS, "%s,", submenu_fim);


      /* ****************************  MODULO RTU >> CONFIGURAÇÃO  ***************************************/
      // Cria Titulo e Submenu (Módulo RTU>>Configurações DO)
      fprintf(ArqJS, "%s", submenu_config_DO);

#if defined ( SDM_F120 )
      for (i=0; i<12; i++)
#else
      for (i=0; i<6; i++)
#endif
      {
        // Configuração DO
        fprintf(ArqJS, "%s", var_ConfigDO_1[i*2]);
        temp_u16 = (HRSerialBuffer[LISTA_HR_1218][(i*4)+3]<<8) | HRSerialBuffer[LISTA_HR_1218][(i*4)+4];
        fprintf(ArqJS, "0x%04X %s,", temp_u16, var_ConfigDO_2[i*2]);

        // Parâmetro DO
        fprintf(ArqJS, "%s", var_ConfigDO_1[(i*2)+1]);
        temp_u16 = (HRSerialBuffer[LISTA_HR_1218][(i*4)+5]<<8) | HRSerialBuffer[LISTA_HR_1218][(i*4)+6];

#if defined ( SDM_F120 )
        if (i == 11) // Não coloca virgula no ultimo elemento
#else
        if (i == 5)
#endif
        {
          fprintf(ArqJS, "%d %s", temp_u16, var_ConfigDO_2[(i*2)+1]);
        }
        else
        {
          fprintf(ArqJS, "%d %s,", temp_u16, var_ConfigDO_2[(i*2)+1]);
        }
      }

      // Fecha Submenu
      fprintf(ArqJS, "%s,", submenu_fim);


      /* ****************************  MODULO RTU >> CONFIGURAÇÃO  ***************************************/
      // Cria Titulo e Submenu (Módulo RTU>>Configurações AI)
      fprintf(ArqJS, "%s", submenu_config_AI);
#if defined ( SDM_F120 )
      for (i=0; i<6; i++)
#else
      for (i=0; i<3; i++)
#endif
      {
        // Configuração AI
        fprintf(ArqJS, "%s", var_ConfigAI_1[i*3]);
        temp_u16 = (HRSerialBuffer[LISTA_HR_1230][(i*6)+3]<<8) | HRSerialBuffer[LISTA_HR_1230][(i*6)+4];
        fprintf(ArqJS, "0x%04X %s,", temp_u16, var_ConfigAI_2[i*3]);

        // Parâmetro 1 AI
        fprintf(ArqJS, "%s", var_ConfigAI_1[(i*3)+1]);
        temp_u16 = (HRSerialBuffer[LISTA_HR_1230][(i*6)+5]<<8) | HRSerialBuffer[LISTA_HR_1230][(i*6)+6];
        fprintf(ArqJS, "%d %s,", temp_u16, var_ConfigAI_2[(i*3)+1]);

        // Parâmetro 2 AI
        fprintf(ArqJS, "%s", var_ConfigAI_1[(i*3)+2]);
        temp_u16 = (HRSerialBuffer[LISTA_HR_1230][(i*6)+7]<<8) | HRSerialBuffer[LISTA_HR_1230][(i*6)+8];

#if defined ( SDM_F120 )
        if (i == 5) // Não coloca virgula no ultimo elemento
#else
        if (i == 2)
#endif
        {
          fprintf(ArqJS, "%d %s", temp_u16, var_ConfigAI_2[(i*3)+2]);
        }
        else
        {
          fprintf(ArqJS, "%d %s,", temp_u16, var_ConfigAI_2[(i*3)+2]);
        }
      }

      // Fecha Submenu
      fprintf(ArqJS, "%s,", submenu_fim);



      /* ****************************  MODULO RTU >> DIAGNÓSTICOS  ***************************************/
      // Submenu (Módulo RTU>>Comunicação)
      fprintf(ArqJS, "%s", submenu_comm);

      // IQC [Vega II]
      fprintf(ArqJS, "%s", var_IQC2_1);
      temp_u16 = (IRSerialBuffer[LISTA_800][13]<<8) | IRSerialBuffer[LISTA_800][14];
      fprintf(ArqJS, "%d %s,", temp_u16, var_IQC2_2);

      // TX [Vega II]
      fprintf(ArqJS, "%s", var_TX2_1);
      ptr_u32 = (unsigned int *)&IRSerialBuffer[LISTA_800][15];
      (void)memcpy(&llu32Tmp,ptr_u32,4);
      temp_u32 = bswap_32(llu32Tmp);
      fprintf(ArqJS, "%d %s,", temp_u32, var_TX2_2);

      // RX [Vega II]
      fprintf(ArqJS, "%s", var_RX2_1);
      ptr_u32 = (unsigned int *)&IRSerialBuffer[LISTA_800][19];
      (void)memcpy(&llu32Tmp,ptr_u32,4);
      temp_u32 = bswap_32(llu32Tmp);
      fprintf(ArqJS, "%d %s,", temp_u32, var_RX2_2);

      // IQC [IHM]
      fprintf(ArqJS, "%s", var_IQC3_1);
      temp_u16 = (IRSerialBuffer[LISTA_800][23]<<8) | IRSerialBuffer[LISTA_800][24];
      fprintf(ArqJS, "%d %s,", temp_u16, var_IQC3_2);

      // TX [IHM]
      fprintf(ArqJS, "%s", var_TX3_1);
      ptr_u32 = (unsigned int *)&IRSerialBuffer[LISTA_800][25];
      (void)memcpy(&llu32Tmp,ptr_u32,4);
      temp_u32 = bswap_32(llu32Tmp);
      fprintf(ArqJS, "%d %s,", temp_u32, var_TX3_2);

      // RX [IHM]
      fprintf(ArqJS, "%s", var_RX3_1);
      ptr_u32 = (unsigned int *)&IRSerialBuffer[LISTA_800][29];
      (void)memcpy(&llu32Tmp,ptr_u32,4);
      temp_u32 = bswap_32(llu32Tmp);
#if defined ( SDM_F30_60 )
      fprintf(ArqJS, "%d %s", temp_u32, var_RX3_2);
#else
      fprintf(ArqJS, "%d %s,", temp_u32, var_RX3_2);
#endif

#if defined ( SDM_F120 )
      // IQC
      fprintf(ArqJS, "%s", var_IQC1_1);
      temp_u16 = (IRSerialBuffer[LISTA_800][3]<<8) | IRSerialBuffer[LISTA_800][4];
      fprintf(ArqJS, "%d %s,", temp_u16, var_IQC1_2);

      // TX
      fprintf(ArqJS, "%s", var_TX1_1);
      ptr_u32 = (unsigned int *)&IRSerialBuffer[LISTA_800][5];
      (void)memcpy(&llu32Tmp,ptr_u32,4);
      temp_u32 = bswap_32(llu32Tmp);
      fprintf(ArqJS, "%d %s,", temp_u32, var_TX1_2);

      // RX
      fprintf(ArqJS, "%s", var_RX1_1);
      ptr_u32 = (unsigned int *)&IRSerialBuffer[LISTA_800][9];
      (void)memcpy(&llu32Tmp,ptr_u32,4);
      temp_u32 = bswap_32(llu32Tmp);
      fprintf(ArqJS, "%d %s", temp_u32, var_RX1_2);
#endif

      // Fecha Submenu
      fprintf(ArqJS, "%s,", submenu_fim);


      /* ****************************  MODULO RTU >> INFORMAÇÕES  ***************************************/
      // Submenu (Módulo RTU>>Informações)
      fprintf(ArqJS, "%s", submenu_info);

      // Versão de software
      fprintf(ArqJS, "%s", var_ver_sw_1);
      fprintf(ArqJS, "%d.%d.%d.%d.", IRSerialBuffer[LISTA_65500][5], IRSerialBuffer[LISTA_65500][6], IRSerialBuffer[LISTA_65500][7], IRSerialBuffer[LISTA_65500][8]);
      temp_u16 = (IRSerialBuffer[LISTA_65500][9]<<8) | IRSerialBuffer[LISTA_65500][10];
      fprintf(ArqJS, "%d %s,", temp_u16, var_ver_sw_2);

      // Data de fabricação
      fprintf(ArqJS, "%s", var_data_fabr_1);
      sprintf(buffer_temp, "%02x/%02x/%02x%02x ", IRSerialBuffer[LISTA_65500][15], IRSerialBuffer[LISTA_65500][16], IRSerialBuffer[LISTA_65500][17], IRSerialBuffer[LISTA_65500][18]);	// Data
      fprintf(ArqJS, "%s", buffer_temp);
      sprintf(buffer_temp, "%02x:%02x:%02x", IRSerialBuffer[LISTA_65500][19], IRSerialBuffer[LISTA_65500][20], IRSerialBuffer[LISTA_65500][21]);
      fprintf(ArqJS, "%s%s,", buffer_temp, var_data_fabr_2);

      // Identificação do microcontrolador
      fprintf(ArqJS, "%s", var_uc_id_1);
      temp_u16 = (IRSerialBuffer[LISTA_65500][23]<<8) | IRSerialBuffer[LISTA_65500][24];
      fprintf(ArqJS, "%d %s,", temp_u16, var_uc_id_2);

      // Número de série
      fprintf(ArqJS, "%s", var_num_ser_1);
      ptr_u32 = (unsigned int *)&IRSerialBuffer[LISTA_65500][35];
      (void)memcpy(&llu32Tmp,ptr_u32,4);
      temp_u32 = bswap_32(llu32Tmp);
      fprintf(ArqJS, "%d %s", temp_u32, var_num_ser_2);

      // Fecha Submenu e titulo
      fprintf(ArqJS, "%s%s", submenu_fim, tit_fim);


      /* ****************************  MODULO IHM >> DIAGNOSTICOS  ***************************************/
      // Cria Titulo e Submenu (Módulo IHM>>Diagnosticos)
      fprintf(ArqJS, "%s%s", tit_mod_IHM, submenu_diag);

      // Temperatura
#if 0
      fprintf(ArqJS, "%s", var_Temp1);
      ptr_u32 = (unsigned int *)&IRSerialBuffer[LISTA_400][27];
      (void)memcpy(&llu32Tmp,ptr_u32,4);
      temp_u32 = bswap_32(llu32Tmp);
      ptr_float = (float *)&temp_u32;
      fprintf(ArqJS, "%.0f %s,", *ptr_float, var_Temp2);
#endif
#warning "Temperature Ixora Core, no IHM!"
      fprintf(ArqJS, "%s", var_Temp1);
      //ptr_float = (float *)&GetTemperatureGateway();
      fprintf(ArqJS, "%.3f %s,", /**ptr_float*/GetTemperatureGateway(), var_Temp2);


      // Contador de inicialização
      fprintf(ArqJS, "%s", var_cnt_init_1);
      temp_u16 = (IRSerialBuffer[LISTA_400][31]<<8) | IRSerialBuffer[LISTA_400][32];
      fprintf(ArqJS, "%d %s", temp_u16, var_cnt_init_2);

      // Fecha Submenu
      fprintf(ArqJS, "%s,", submenu_fim);


      /* ****************************  MODULO IHM >> INFORMAÇÕES  ***************************************/
      // Submenu (Módulo IHM>>Informações)
      fprintf(ArqJS, "%s", submenu_info);

      // Versão de software
      fprintf(ArqJS, "%s", var_ver_sw_1);
      fprintf(ArqJS, "%d.%d.%d.%d.", IRSerialBuffer[LISTA_65500][43], IRSerialBuffer[LISTA_65500][44], IRSerialBuffer[LISTA_65500][45], IRSerialBuffer[LISTA_65500][46]);
      temp_u16 = (IRSerialBuffer[LISTA_65500][47]<<8) | IRSerialBuffer[LISTA_65500][48];
      fprintf(ArqJS, "%d %s,", temp_u16, var_ver_sw_2);

      // Número de série
      fprintf(ArqJS, "%s", var_num_ser_1);
      ptr_u32 = (unsigned int *)&IRSerialBuffer[LISTA_65500][49];
      (void)memcpy(&llu32Tmp,ptr_u32,4);
      temp_u32 = bswap_32(llu32Tmp);
      fprintf(ArqJS, "%d %s", temp_u32, var_num_ser_2);

      // Fecha Submenu e titulo
      fprintf(ArqJS, "%s%s", submenu_fim, tit_fim);



      /* ****************************  MODULO GPRS >> DIAGNOSTICOS  ***************************************/
      // Cria Titulo e Submenu (Módulo GPRS>>Diagnosticos)
      fprintf(ArqJS, "%s%s", tit_mod_GPRS, submenu_diag);

      // Nível de sinal
      fprintf(ArqJS, "%s", var_RSSI_1);
      temp_u16 = (IRSerialBuffer[LISTA_800][33]<<8) | IRSerialBuffer[LISTA_800][34];
      fprintf(ArqJS, "%d RSSI %s,", temp_u16, var_RSSI_2);

      // Tempo decrescente pare reset do modem
      fprintf(ArqJS, "%s", var_Tmodem1);
      temp_u16 = (IRSerialBuffer[LISTA_50][71]<<8) | IRSerialBuffer[LISTA_50][72];
      fprintf(ArqJS, "%d %s,", temp_u16, var_Tmodem2);

      // Número de reset do modem
      fprintf(ArqJS, "%s", var_NResetModem1);
      temp_u16 = (IRSerialBuffer[LISTA_50][73]<<8) | IRSerialBuffer[LISTA_50][74];
      fprintf(ArqJS, "%d %s", temp_u16, var_NResetModem2);

      // Fecha Submenu
      fprintf(ArqJS, "%s,", submenu_fim);


      /* ****************************  MODULO GPRS >> INFORMAÇÕES  ***************************************/
      // Submenu (Módulo GPRS>>Informações)
      fprintf(ArqJS, "%s", submenu_info);

      // Tipo de modem
      fprintf(ArqJS, "%s", var_modem_1);
      temp_u16 = (IRSerialBuffer[LISTA_65500][53]<<8) | IRSerialBuffer[LISTA_65500][54];
      if (temp_u16 == 1)
      {
        fprintf(ArqJS, "Informat");
      }
      else if (temp_u16 == 2)
      {
        fprintf(ArqJS, "Robustel/Duodigit");
      }
      fprintf(ArqJS, "%s,", var_modem_2);

      // IMEI
      fprintf(ArqJS, "%s", var_IMEI_1);
      fprintf(ArqJS, "null %s", var_IMEI_2);

      // Fecha Submenu e titulo
      fprintf(ArqJS, "%s%s", submenu_fim, tit_fim);


      /* ****************************  Computador de vazão >> CONFIGURAÇÕES  ***************************************/
      // Cria Titulo e Submenu (Computador de vazão>>Configurações)
      fprintf(ArqJS, "%s%s", tit_comp_vazao, submenu_config);

      // Unidade de vazão
      fprintf(ArqJS, "%s", var_vazao_unid_1);
      temp_u16 = (HRSerialBuffer[1][3]<<8) | HRSerialBuffer[1][4];
      fprintf(ArqJS, "%s", GetString_Tabela7(temp_u16));
      fprintf(ArqJS, "%s,", var_vazao_unid_2);
      UnidFlow = temp_u16;

      // Unidade de temperatura
      fprintf(ArqJS, "%s", var_temp_unid_1);
      temp_u16 = (HRSerialBuffer[1][5]<<8) | HRSerialBuffer[1][6];
      fprintf(ArqJS, "%s", GetString_Tabela8(temp_u16));
      fprintf(ArqJS, "%s,", var_temp_unid_2);
      UnidTemp= temp_u16;

      // Unidade de densidade para cálculo do volume corrigido
      fprintf(ArqJS, "%s", var_densid_unid_1);
      temp_u16 = (HRSerialBuffer[1][7]<<8) | HRSerialBuffer[1][8];
      fprintf(ArqJS, "%s", GetString_Tabela9(temp_u16));
      fprintf(ArqJS, "%s,", var_densid_unid_2);
      UnidDens = temp_u16;

      // Unidade de volume
      fprintf(ArqJS, "%s", var_vol_unid_1);
      temp_u16 = (HRSerialBuffer[1][9]<<8) | HRSerialBuffer[1][10];
      fprintf(ArqJS, "%s", GetString_Tabela10(temp_u16));
      fprintf(ArqJS, "%s", var_vol_unid_2);
      UnidVol = temp_u16;

      // Fecha Submenu
      fprintf(ArqJS, "%s,", submenu_fim);


      /* ****************************  Computador de vazão >> INFORMAÇÕES  ***************************************/
      // Submenu (Computador de vazão>>Informações)
      fprintf(ArqJS, "%s", submenu_info);

      // Versão de firmware
      fprintf(ArqJS, "%s", var_ver_firm_1);
      memset(buffer_temp,0,sizeof(buffer_temp));
      strncpy(buffer_temp, (char *)&IRSerialBuffer[LISTA_600][3], 12);
      fprintf(ArqJS, "%s", buffer_temp);
      fprintf(ArqJS, "%s,", var_ver_firm_2);

      // Versão modbus
      memset(buffer_temp,0,sizeof(buffer_temp));
      fprintf(ArqJS, "%s", var_ver_modb_1);
      strncpy(buffer_temp, (char *)&IRSerialBuffer[LISTA_600][15], 8);
      fprintf(ArqJS, "%s", buffer_temp);
      fprintf(ArqJS, "%s,", var_ver_modb_2);

      // Status bateria
      fprintf(ArqJS, "%s", var_sts_bat_1);
      temp_u16 = (IRSerialBuffer[LISTA_600][23]<<8) | IRSerialBuffer[LISTA_600][24];
      fprintf(ArqJS, "%s", GetString_TabelaBateria(temp_u16));
      fprintf(ArqJS, "%s,", var_sts_bat_2);


      // Modo de operação
      fprintf(ArqJS, "%s", var_modo_op_1);
      temp_u16 = (IRSerialBuffer[LISTA_600][25]<<8) | IRSerialBuffer[LISTA_600][26];
      if (temp_u16 == 0)
      {
        fprintf(ArqJS, "Local");
      }
      else if (temp_u16 == 1)
      {
        fprintf(ArqJS, "Remoto");
      }
      fprintf(ArqJS, "%s,", var_modo_op_2);


      // Contador de operações
      fprintf(ArqJS, "%s", var_cnt_op_1);
      ptr_u32 = (unsigned int *)&IRSerialBuffer[LISTA_600][27];
      (void)memcpy(&llu32Tmp,ptr_u32,4);
      temp_u32 = bswap_32(llu32Tmp);
      fprintf(ArqJS, "%d %s", temp_u32, var_cnt_op_2);

      // Fecha Submenu e titulo
      fprintf(ArqJS, "%s%s", submenu_fim, tit_fim);


      /* ****************************  GATEWAY >> INFORMAÇÕES  ***************************************/
      // Cria Titulo e Submenu (GATEWAY>>Diagnosticos)
      fprintf(ArqJS, "%s%s", tit_gateway, submenu_info);

      // IP
      fprintf(ArqJS, "%s", var_ip_1);
      fprintf(ArqJS, "%d.%d.%d.%d", IRSerialBuffer[LISTA_700][3], IRSerialBuffer[LISTA_700][4], IRSerialBuffer[LISTA_700][5], IRSerialBuffer[LISTA_700][6]);
      fprintf(ArqJS, "%s,", var_ip_2);

      // Mask
      fprintf(ArqJS, "%s", var_mask_1);
      fprintf(ArqJS, "%d.%d.%d.%d", IRSerialBuffer[LISTA_700][19], IRSerialBuffer[LISTA_700][20], IRSerialBuffer[LISTA_700][21], IRSerialBuffer[LISTA_700][22]);
      fprintf(ArqJS, "%s,", var_mask_2);

      // Gateway
      fprintf(ArqJS, "%s", var_gateway_1);
      fprintf(ArqJS, "%d.%d.%d.%d", IRSerialBuffer[LISTA_700][35], IRSerialBuffer[LISTA_700][36], IRSerialBuffer[LISTA_700][37], IRSerialBuffer[LISTA_700][38]);
      fprintf(ArqJS, "%s,", var_gateway_2);

      // Versão Main Server
      fprintf(ArqJS, "%s", var_ver_main_1);
      memset(buffer_temp,0,sizeof(buffer_temp));
      sprintf(buffer_temp, "%s - %s %s", swVersion, swData, swTime);
      //strncpy(buffer_temp, swTime, 16);
      fprintf(ArqJS, "%s", buffer_temp);
      fprintf(ArqJS, "%s,", var_ver_main_2);

      if (count_version > 20)
      {
        count_version = 0;

        fpPIPE = popen("/opt/SDM/Execute/UpdateSW -v", "r");
        memset(buffer_version[0],0,sizeof(buffer_version[0]));
        fgets(buffer_version[0], 40, fpPIPE);
        pclose(fpPIPE);

        fpPIPE = popen("/opt/SDM/Execute/ModbusTCPServer -v", "r");
        memset(buffer_version[1],0,sizeof(buffer_version[1]));
        fgets(buffer_version[1], 40, fpPIPE);
        pclose(fpPIPE);

        fpPIPE = popen("/opt/SDM/Execute/ModbusClient -v", "r");
        memset(buffer_version[2],0,sizeof(buffer_version[2]));
        fgets(buffer_version[2], 40, fpPIPE);
        pclose(fpPIPE);

      }
      count_version++;

      // Versão Update SW
      fprintf(ArqJS, "%s", var_ver_updatesw_1);
      fprintf(ArqJS, "%s%s,", buffer_version[0], var_ver_updatesw_2);

      // Versão Modbus TCP Server
      fprintf(ArqJS, "%s", var_ver_modbs_1);
      fprintf(ArqJS, "%s%s,", buffer_version[1], var_ver_modbs_2);

      // Versão Modbus Client
      fprintf(ArqJS, "%s", var_ver_modbclient_1);
      fprintf(ArqJS, "%s%s,", buffer_version[2], var_ver_modbclient_2);

      // Online
      t_atual = time(NULL);
      t_res = t_atual - t_inicio;
      tm = *localtime(&t_res);
      fprintf(ArqJS, "%s", var_online_1);
      fprintf(ArqJS, "%d dias - %d:%02d:%02d %s,", tm.tm_yday, tm.tm_hour, tm.tm_min, tm.tm_sec, var_online_2);

      // Serial RX Counter
      fprintf(ArqJS, "%s", var_erro_RX1);
      fprintf(ArqJS, "%ld %s,", SerialRXCounter, var_erro_RX2);

      // Erros de comunicação e CRC
      fprintf(ArqJS, "%s", var_erro_Comm1);
      fprintf(ArqJS, "%d %s,", ErrCommCounter, var_erro_Comm2);

      // MainServer Lock
      fprintf(ArqJS, "%s", var_MSLock1);
      fprintf(ArqJS, "%d %s,", MainServerLock, var_MSLock2);

      // Temperatura Core
#warning "Temperature Core"
      // Porcentagem Datalog baixado
      fprintf(ArqJS, "%s", var_StatDatalog1);
      fprintf(ArqJS, "%d %s", (unsigned int)((PointerDLog*1000)/5000), var_StatDatalog2);


      // Fecha Submenu e titulo
      fprintf(ArqJS, "%s%s", submenu_fim, tit_fim);


      /* ****************************  Operador >> INFORMAÇÕES  ***************************************/
      if(ListaOperadoresSenhas > 0){
        // Cria Titulo e Submenu (Operadores)
        fprintf(ArqJS, "%s%s", tit_bd_Operador, submenu_bd_Operador);

        // Operador 1
        fprintf(ArqJS, "%s", var_Op1i);
        memset(buffer_temp,0,sizeof(buffer_temp));
        strncpy(buffer_temp, (char *)&IRSerialBuffer[LISTA_1500][3], 8);
        fprintf(ArqJS, "%s", buffer_temp);
        fprintf(ArqJS, "%s,", var_Op1f);
        // Senha 1
        fprintf(ArqJS, "%s", var_Pass1i);
        if(IRSerialBuffer[LISTA_1500][3] != 0x20) fprintf(ArqJS, "%s", PassWord_Simbol);
        fprintf(ArqJS, "%s,", var_Pass1f);

        // Operador 2
        fprintf(ArqJS, "%s", var_Op2i);
        memset(buffer_temp,0,sizeof(buffer_temp));
        strncpy(buffer_temp, (char *)&IRSerialBuffer[LISTA_1500][11], 8);
        fprintf(ArqJS, "%s", buffer_temp);
        fprintf(ArqJS, "%s,", var_Op2f);
        // Senha 2
        fprintf(ArqJS, "%s", var_Pass2i);
        if(IRSerialBuffer[LISTA_1500][11] != 0x20) fprintf(ArqJS, "%s", PassWord_Simbol);
        fprintf(ArqJS, "%s,", var_Pass2f);

        // Operador 3
        fprintf(ArqJS, "%s", var_Op3i);
        memset(buffer_temp,0,sizeof(buffer_temp));
        strncpy(buffer_temp, (char *)&IRSerialBuffer[LISTA_1500][19], 8);
        fprintf(ArqJS, "%s", buffer_temp);
        fprintf(ArqJS, "%s,", var_Op3f);
        // Senha 3
        fprintf(ArqJS, "%s", var_Pass3i);
        if(IRSerialBuffer[LISTA_1500][19] != 0x20) fprintf(ArqJS, "%s", PassWord_Simbol);
        fprintf(ArqJS, "%s,", var_Pass3f);

        // Operador 4
        fprintf(ArqJS, "%s", var_Op4i);
        memset(buffer_temp,0,sizeof(buffer_temp));
        strncpy(buffer_temp, (char *)&IRSerialBuffer[LISTA_1500][27], 8);
        fprintf(ArqJS, "%s", buffer_temp);
        fprintf(ArqJS, "%s,", var_Op4f);
        // Senha 4
        fprintf(ArqJS, "%s", var_Pass4i);
        if(IRSerialBuffer[LISTA_1500][27] != 0x20) fprintf(ArqJS, "%s", PassWord_Simbol);
        fprintf(ArqJS, "%s,", var_Pass4f);

        // Operador 5
        fprintf(ArqJS, "%s", var_Op5i);
        memset(buffer_temp,0,sizeof(buffer_temp));
        strncpy(buffer_temp, (char *)&IRSerialBuffer[LISTA_1500][35], 8);
        fprintf(ArqJS, "%s", buffer_temp);
        fprintf(ArqJS, "%s,", var_Op5f);
        // Senha 5
        fprintf(ArqJS, "%s", var_Pass5i);
        if(IRSerialBuffer[LISTA_1500][35] != 0x20) fprintf(ArqJS, "%s", PassWord_Simbol);
        fprintf(ArqJS, "%s,", var_Pass5f);

        // Operador 6
        fprintf(ArqJS, "%s", var_Op6i);
        memset(buffer_temp,0,sizeof(buffer_temp));
        strncpy(buffer_temp, (char *)&IRSerialBuffer[LISTA_1500][43], 8);
        fprintf(ArqJS, "%s", buffer_temp);
        fprintf(ArqJS, "%s,", var_Op6f);
        // Senha 6
        fprintf(ArqJS, "%s", var_Pass6i);
        if(IRSerialBuffer[LISTA_1500][43] != 0x20) fprintf(ArqJS, "%s", PassWord_Simbol);
        fprintf(ArqJS, "%s,", var_Pass6f);

        // Operador 7
        fprintf(ArqJS, "%s", var_Op7i);
        memset(buffer_temp,0,sizeof(buffer_temp));
        strncpy(buffer_temp, (char *)&IRSerialBuffer[LISTA_1500][51], 8);
        fprintf(ArqJS, "%s", buffer_temp);
        fprintf(ArqJS, "%s,", var_Op7f);
        // Senha 7
        fprintf(ArqJS, "%s", var_Pass7i);
        if(IRSerialBuffer[LISTA_1500][51] != 0x20) fprintf(ArqJS, "%s", PassWord_Simbol);
        fprintf(ArqJS, "%s,", var_Pass7f);

        // Operador 8
        fprintf(ArqJS, "%s", var_Op8i);
        memset(buffer_temp,0,sizeof(buffer_temp));
        strncpy(buffer_temp, (char *)&IRSerialBuffer[LISTA_1500][59], 8);
        fprintf(ArqJS, "%s", buffer_temp);
        fprintf(ArqJS, "%s,", var_Op8f);
        // Senha 8
        fprintf(ArqJS, "%s", var_Pass8i);
        if(IRSerialBuffer[LISTA_1500][59] != 0x20) fprintf(ArqJS, "%s", PassWord_Simbol);
        fprintf(ArqJS, "%s,", var_Pass8f);

        // Operador 9
        fprintf(ArqJS, "%s", var_Op9i);
        memset(buffer_temp,0,sizeof(buffer_temp));
        strncpy(buffer_temp, (char *)&IRSerialBuffer[LISTA_1500][67], 8);
        fprintf(ArqJS, "%s", buffer_temp);
        fprintf(ArqJS, "%s,", var_Op9f);
        // Senha 9
        fprintf(ArqJS, "%s", var_Pass9i);
        if(IRSerialBuffer[LISTA_1500][67] != 0x20) fprintf(ArqJS, "%s", PassWord_Simbol);
        fprintf(ArqJS, "%s,", var_Pass9f);

        // Operador 10
        fprintf(ArqJS, "%s", var_Op10i);
        memset(buffer_temp,0,sizeof(buffer_temp));
        strncpy(buffer_temp, (char *)&IRSerialBuffer[LISTA_1500][75], 8);
        fprintf(ArqJS, "%s", buffer_temp);
        fprintf(ArqJS, "%s,", var_Op10f);
        // Senha 10
        fprintf(ArqJS, "%s", var_Pass10i);
        if(IRSerialBuffer[LISTA_1500][75] != 0x20) fprintf(ArqJS, "%s", PassWord_Simbol);
        fprintf(ArqJS, "%s", var_Pass10f);


        // Fecha Submenu e titulo
        fprintf(ArqJS, "%s%s", submenu_fim, tit_fim);
      }//if(ListaOperadoresSenhas > 1)



      // Fecha arquivo
      fprintf(ArqJS, "%s", arq_fim);

      fclose(ArqJS);
    }
  }
}

void TrataASCIIChar(unsigned char *ptr_table, unsigned int size_tab)
{
  const unsigned char *end_char = ptr_table + size_tab;
  while(ptr_table < end_char)
  {
    if (*ptr_table < 0x20)
    {
      *ptr_table = 0x20;
    }
    ptr_table++;
  }
}
#if 0
inline
uint32_t swap_uint32( uint32_t val )
{
  val = ((val << 8) & 0xFF00FF00 ) | ((val >> 8) & 0xFF00FF );
  return (val << 16) | (val >> 16);
}
#endif

void StartTimeout(unsigned int timer_id, unsigned int timer_val)
{
  struct timeval tv_actualtime;

  gettimeofday(&tv_actualtime,NULL);
  timer_end[timer_id] = (unsigned int)((1000 * tv_actualtime.tv_sec) + (tv_actualtime.tv_usec)/1000);
  timer_end[timer_id] += timer_val;
}

unsigned int CheckTimeout(unsigned int timer_id, unsigned int restart_val)
{
  struct timeval tv_actualtime;
  unsigned int actualtime_milisec;

  gettimeofday(&tv_actualtime,NULL);
  actualtime_milisec = (unsigned int)((1000 * tv_actualtime.tv_sec) + (tv_actualtime.tv_usec)/1000);

  if (actualtime_milisec >= timer_end[timer_id])
  {
    /* Rearma timeout */
    StartTimeout(timer_id, restart_val);
    return 1;
  }
  else
  {
    return 0;
  }
}

float GetTemperatureGateway(void)
{
  FILE *fpTemp;
  float temperature = 0.0;

  fpTemp = fopen(TEMPERATURE_FILE, "r");
  fscanf(fpTemp, "%f\n", &temperature);
  fclose(fpTemp);

  return temperature / 1000.0;
}

void GetUnit(FILE *ArqTemp, UnitTypes tipo)
{
  switch (tipo)
  {
  // UNITs On-Line
  case UNIT_FLOW:
    fprintf(ArqTemp, "%s", GetString_Tabela7(UnidFlow));
  break;
  case UNIT_TEMP:
    fprintf(ArqTemp, "%s", GetString_Tabela8(UnidTemp));
  break;
  case UNIT_DENS:
    fprintf(ArqTemp, "%s", GetString_Tabela9(UnidDens));
  break;
  case UNIT_VOL:
    fprintf(ArqTemp, "%s", GetString_Tabela10(UnidVol));
  break;

    // UNITs LOGs
  case UNIT_FLOW_LOG:
    fprintf(ArqTemp, "%s", GetString_Tabela7(UnidLogFlow));
  break;
  case UNIT_TEMP_LOG:
    fprintf(ArqTemp, "%s", GetString_Tabela8(UnidLogTemp));
  break;
  case UNIT_DENS_LOG:
    fprintf(ArqTemp, "%s", GetString_Tabela9(UnidLogDens));
  break;
  case UNIT_VOL_LOG:
    fprintf(ArqTemp, "%s", GetString_Tabela10(UnidLogVol));
  break;

  default:
  break;
  }
}

//Usado para conta bit do Netmask
char conta_bit (unsigned char num, int bit) {

  int i, flag = 1;
  char conta1=0, conta0=0;

  for(i=0; i<8; i++) {
    if(num&flag)         // binario da variavel num
      conta1++;
    else
      conta0++;
    flag = flag << 1; // Movimenta um BIT de flag para direita
  }
  if(bit)
    return(conta1);
  return(conta0);
}

void CalBcast(char *wip, char *wmask, char *bcast)
{
  unsigned int ip,mask,res, cntzero,numhosts;
  unsigned int aux[4];


  sscanf(wip,"%d.%d.%d.%d",&aux[0],&aux[1],&aux[2],&aux[3]);
  ip = (aux[0] << 24) & 0xFF000000;
  ip += (aux[1] << 16) & 0x00FF0000;
  ip += (aux[2] << 8) & 0x0000FF00;
  ip += (aux[3]) & 0x000000FF;

  sscanf(wmask,"%d.%d.%d.%d",&aux[0],&aux[1],&aux[2],&aux[3]);
  mask = (aux[0] << 24) & 0xFF000000;
  mask += (aux[1] << 16) & 0x00FF0000;
  mask += (aux[2] << 8) & 0x0000FF00;
  mask += (aux[3]) & 0x000000FF;

  cntzero = conta_bit (aux[0],0);
  cntzero += conta_bit (aux[1],0);
  cntzero += conta_bit (aux[2],0);
  cntzero += conta_bit (aux[3],0);

  numhosts = pow(2,cntzero);

  res = ip & mask;

  res += (numhosts-1);

  aux[0] = (res >> 24) & 0x000000FF;
  aux[1] = (res >> 16) & 0x000000FF;
  aux[2] = (res >> 8) & 0x000000FF;
  aux[3] = (res) & 0x000000FF;

  sprintf(bcast,"%d.%d.%d.%d",aux[0],aux[1],aux[2],aux[3]);
}


void CriaArquivoGEMAP_MTM(unsigned char erro_id)
{
  FILE *ArqJS;
  time_t t_atual, t_res;

  if (erro_id == NO_ERROR)
  {
    TrataASCIIChar(&IRSerialBuffer[LISTA_100][3], 40);
  }
  else if (erro_id == SERIAL_NOT_FOUND)
  {
    ArqJS = fopen("/tmp/VirtualRAM/arquivo.js", "w");
    if (ArqJS)
    {
#ifdef SET_DEBUG_TXT
      fprintf(ArqJS, "%s", _ini);
      fprintf(ArqJS, "\n\nSERIAL %s NAO ENCONTRADO -> %d minutos sem comunicacao \n\n", &ComPath[0], Fault_COMM);
      fprintf(ArqJS, "Serial RX Counter: %ld \n\n", SerialRXCounter);
      fprintf(ArqJS, "Erro COMM: %d \n\n", ErrCommCounter);
      fprintf(ArqJS, "MainServer Lock: %d \n\n", MainServerLock);
      fprintf(ArqJS, "%s", _fim);
      fclose(ArqJS);
#else
      fprintf(ArqJS, "%s", arq_ini);
      fprintf(ArqJS, "%s%s", tit_erroinit, submenu_erroinit);
      fprintf(ArqJS, "%s", var_Erro1);
      fprintf(ArqJS, "<b>SERIAL %s NÃO ENCONTRADO</b><br>%d minutos sem comunicação", &ComPath[0], Fault_COMM);
      fprintf(ArqJS, "%s,", var_Erro2);
      fprintf(ArqJS, "%s", var_erro_RX1);
      fprintf(ArqJS, "%ld %s,", SerialRXCounter, var_erro_RX2);
      fprintf(ArqJS, "%s", var_erro_Comm1);
      fprintf(ArqJS, "%d %s,", ErrCommCounter, var_erro_Comm2);
      fprintf(ArqJS, "%s", var_MSLock1);
      fprintf(ArqJS, "%d %s", MainServerLock, var_MSLock2);
      fprintf(ArqJS, "%s%s", submenu_fim, tit_fim);
      fprintf(ArqJS, "%s", arq_fim);
      fclose(ArqJS);
#endif
    }
    else
    {
      printf("Error ao abrir arquivo.js");
    }
  }
}
