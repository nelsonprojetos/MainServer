/*
 * Tabelas_GEMAP.h
 *
 *  Created on: May 26, 2014
 *      Author: denis
 */

#ifndef TABELAS_GEMAP_H_
#define TABELAS_GEMAP_H_

char * GetString_Tabela1(unsigned int idn);
char * GetString_Tabela2(unsigned int idn);
char * GetString_Tabela7(unsigned int idn);
char * GetString_Tabela8(unsigned int idn);
char * GetString_Tabela9(unsigned int idn);
char * GetString_Tabela10(unsigned int idn);
char * GetString_Tabela15(unsigned int idn);
char * GetString_Tabela16(unsigned int idn);
char * GetString_Tabela17(unsigned int idn);

char * GetString_TabelaBateria(unsigned int idn);
char * GetPos_Tabela1(unsigned int idn);
char * GetString_Braco(unsigned int idn);


enum  Unit_Types
    {
      UNIT_FLOW = 1,
      UNIT_TEMP,
      UNIT_DENS,
      UNIT_VOL,

      UNIT_FLOW_LOG,
      UNIT_TEMP_LOG,
      UNIT_DENS_LOG,
      UNIT_VOL_LOG

     };
typedef enum Unit_Types UnitTypes ;


#endif /* TABELAS_GEMAP_H_ */
