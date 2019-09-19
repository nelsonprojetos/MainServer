/*
 * Tabelas_GEMAP.c
 *
 *  Created on: May 26, 2014
 *      Author: denis
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>

#include "Tabelas_GEMAP.h"

char *string_tab1[16] = {
		/* 0x0001 */ "Aguardando Operador",
		/* 0x0002 */ "Parado por nível",
		/* 0x0004 */ "Descarregamento Terminado",
		/* 0x0008 */ "Sistema Parado",
		/* 0x0010 */ "Sistema Livre",
		/* 0x0020 */ "Aguardando Braço",
		/* 0x0040 */ "[Reservado]",
		/* 0x0080 */ "Descarregando",
		/* 0x0100 */ "[Reservado]",
		/* 0x0200 */ "[Reservado]",
		/* 0x0400 */ "Checagem Pré-operação",
		/* 0x0800 */ "Modo Manutenção",
		/* 0x1000 */ "[Reservado]",
		/* 0x2000 */ "Sistema Calibrando",
		/* 0x4000 */ "Desabilitado/Inativo",
		/* 0x8000 */ "Sistema Bloqueado"
};

char *string_tab2[12] = {
		/* 0x0001 */ "Local Braço 1",
		/* 0x0002 */ "Local Braço 2",
		/* 0x0003 */ "Local Braço 1 e 2",
		/* 0x0011 */ "Remoto Braço 1",
		/* 0x0012 */ "Remoto Braço 2",
		/* 0x0013 */ "Remoto Braço 1 e 2",
		/* 0x0101 */ "Calibração Local Braço 1",
		/* 0x0102 */ "Calibração Local Braço 2",
		/* 0x0103 */ "Calibração Local Braço 1 e 2",
		/* 0x0111 */ "Calibração Remoto Braço 1",
		/* 0x0112 */ "Calibração Remoto Braço 2",
		/* 0x0113 */ "Calibração Remoto Braço 1 e 2"
};


char *string_tab7[6] = {
		/* 0 */ " l/min",
		/* 1 */ " l/h",
		/* 2 */ " m³/min",
		/* 3 */ " m³/h",
		/* 4 */ " cm³/min",
		/* 5 */ " cm³/h"
};

char *string_tab8[2] = {
		/* 0 */ " °C",
		/* 1 */ " °F"
};

char *string_tab9[4] = {
		/* 0 */ " kg/m³",
		/* 1 */ " °API",
		/* 2 */ " Sem Unidade",
		/* 3 */ " % Grau Alcoólico"
};

char *string_tab10[3] = {
		/* 0 */ " L",
		/* 1 */ " m³",
		/* 2 */ " cm³"
};

char *string_tab15[17] = {
	/* 0x0000 */ " ",
	/* 0x0001 */ "Botão de emergência",
	/* 0x0002 */ "Aterramento do braço 1",
	/* 0x0004 */ "Densidade fora da faixa",
	/* 0x0008 */ "Temperatura fora da faixa",
	/* 0x0010 */ "Válvula de calibração 1 aberta",
	/* 0x0020 */ "Válvula de calibração 2 aberta",
	/* 0x0040 */ "Válvula de linha aberta",
	/* 0x0080 */ "Erro Vega(CV) não resetado",
	/* 0x0100 */ "Válvula de calibração 1 fechada",
	/* 0x0200 */ "Válvula de calibração 2 fechada",
	/* 0x0400 */ "Válvula de linha fechada",
	/* 0x0800 */ "Sem vazão",
	/* 0x1000 */ "Bateria desconectada",
	/* 0x2000 */ "Alta temperatura na MC",
	/* 0x4000 */ "Comunicação com Ethernet",
	/* 0x8000 */ "Computador de vazão"
	};

char *string_tab16[17] = {
	/* 0x0000 */ " ",
	/* 0x0001 */ "Reservado",
	/* 0x0002 */ "Reservado",
	/* 0x0004 */ "Operação modo manual",
	/* 0x0008 */ "Valor batelada inválido",
	/* 0x0010 */ "Sem sinal do modem",
	/* 0x0020 */ "Configuração de data",
	/* 0x0040 */ "Configuração de hora",
	/* 0x0080 */ "Falha na liberação remota",
	/* 0x0100 */ "Baixa densidade",
	/* 0x0200 */ "Alta densidade",
	/* 0x0400 */ "Baixa temperatura",
	/* 0x0800 */ "Alta temperatura",
	/* 0x1000 */ "Baixa vazão",
	/* 0x2000 */ "Alta vazão",
	/* 0x4000 */ "Falha sensor do tanque",
	/* 0x8000 */ "Parada remota"
	};

char *string_tab17[17] = {
	/* 0x0000 */ " ",
	/* 0x0001 */ "S1 não acionado",
	/* 0x0002 */ "S2 não acionado",
	/* 0x0004 */ "S3 não acionado",
	/* 0x0008 */ "S4 não acionado",
	/* 0x0010 */ "S5 não acionado",
	/* 0x0020 */ "S6 não acionado",
	/* 0x0040 */ "Baixo nível TQ pulmão",
	/* 0x0080 */ "Alto nível TQ pulmão",
	/* 0x0100 */ "Reservado",
	/* 0x0200 */ "Reservado",
	/* 0x0400 */ "Reservado",
	/* 0x0800 */ "Reservado",
	/* 0x1000 */ "Reservado",
	/* 0x2000 */ "Reservado",
	/* 0x4000 */ "Comunicação com MC2",
	/* 0x8000 */ "Aterramento do braço 2"
	};

char *string_bateria[4] = {
		/* 0 */ "Não Presente",
		/* 1 */ "Carregando",
		/* 2 */ "Presente",
		/* 3 */ "Descarregada"
};

char *string_braco[3] = {
		/* 0 */ "1",
		/* 1 */ "2",
		/* 2 */ "1 e 2"
};


char * GetString_Tabela1(unsigned int idn)
{
	switch (idn)
	{
		case 0x0001:
			return string_tab1[0];
		break;

		case 0x0002:
			return string_tab1[1];
		break;

		case 0x0004:
			return string_tab1[2];
		break;

		case 0x0008:
			return string_tab1[3];
		break;

		case 0x0010:
			return string_tab1[4];
		break;

		case 0x0020:
			return string_tab1[5];
		break;

		case 0x0040:
			return string_tab1[6];
		break;

		case 0x0080:
			return string_tab1[7];
		break;

		case 0x0100:
			return string_tab1[8];
		break;

		case 0x0200:
			return string_tab1[9];
		break;

		case 0x0400:
			return string_tab1[10];
		break;

		case 0x0800:
			return string_tab1[11];
		break;

		case 0x1000:
			return string_tab1[12];
		break;

		case 0x2000:
			return string_tab1[13];
		break;

		case 0x4000:
			return string_tab1[14];
		break;

		case 0x8000:
			return string_tab1[15];
		break;
	}

	return string_tab1[0];
}

char * GetString_Tabela2(unsigned int idn)
{
	switch (idn)
	{
		case 0x0001:
			return string_tab2[0];
		break;

		case 0x0002:
			return string_tab2[1];
		break;

		case 0x0003:
			return string_tab2[2];
		break;

		case 0x0011:
			return string_tab2[3];
		break;

		case 0x0012:
			return string_tab2[4];
		break;

		case 0x0013:
			return string_tab2[5];
		break;

		case 0x0101:
			return string_tab2[6];
		break;

		case 0x0102:
			return string_tab2[7];
		break;

		case 0x0103:
			return string_tab2[8];
		break;

		case 0x0111:
			return string_tab2[9];
		break;

		case 0x0112:
			return string_tab2[10];
		break;

		case 0x0113:
			return string_tab2[11];
		break;

	}

	return string_tab2[0];
}


char * GetString_Tabela7(unsigned int idn)
{
	switch (idn)
	{
		case 0:
			return string_tab7[0];
		break;

		case 1:
			return string_tab7[1];
		break;

		case 2:
			return string_tab7[2];
		break;

		case 3:
			return string_tab7[3];
		break;

		case 4:
			return string_tab7[4];
		break;

		case 5:
			return string_tab7[5];
		break;
	}

	return string_tab7[0];
}


char * GetString_Tabela8(unsigned int idn)
{
	switch (idn)
	{
		case 0:
			return string_tab8[0];
		break;

		case 1:
			return string_tab8[1];
		break;
	}

	return string_tab8[0];
}



char * GetString_Tabela9(unsigned int idn)
{
	switch (idn)
	{
		case 0:
			return string_tab9[0];
		break;

		case 1:
			return string_tab9[1];
		break;

		case 2:
			return string_tab9[2];
		break;

		case 3:
			return string_tab9[3];
		break;
	}

	return string_tab9[0];
}


char * GetString_Tabela10(unsigned int idn)
{
	switch (idn)
	{
		case 0:
			return string_tab10[0];
		break;

		case 1:
			return string_tab10[1];
		break;

		case 2:
			return string_tab10[2];
		break;
	}

	return string_tab10[0];
}

char * GetString_Tabela15(unsigned int idn)
{
	unsigned int bit_shift = 1;
	unsigned int i;
	static char buffer_ret[150];

	memset(&buffer_ret[0],0,150);

	for (i=0; i<17; i++)
	{
		if (idn & bit_shift)
		{
			if (buffer_ret[0] == 0)
			{
				sprintf(buffer_ret, "%s", string_tab15[i+1]);
			}
			else
			{
				sprintf(buffer_ret, "%s / %s", buffer_ret, string_tab15[i+1]);
			}
		}
		bit_shift = bit_shift<<1;
	}

	return buffer_ret;
}


char * GetString_Tabela16(unsigned int idn)
{
	unsigned int bit_shift = 1;
	unsigned int i;
	static char buffer_ret[150];

	memset(&buffer_ret[0],0,150);

	for (i=0; i<17; i++)
	{
		if (idn & bit_shift)
		{
			if (buffer_ret[0] == 0)
			{
				sprintf(buffer_ret, "%s", string_tab16[i+1]);
			}
			else
			{
				sprintf(buffer_ret, "%s / %s", buffer_ret, string_tab16[i+1]);
			}
		}
		bit_shift = bit_shift<<1;
	}

	return buffer_ret;
}

char * GetString_Tabela17(unsigned int idn)
{
	unsigned int bit_shift = 1;
	unsigned int i;
	static char buffer_ret[150];

	memset(&buffer_ret[0],0,150);

	for (i=0; i<17; i++)
	{
		if (idn & bit_shift)
		{
			if (buffer_ret[0] == 0)
			{
				sprintf(buffer_ret, "%s", string_tab17[i+1]);
			}
			else
			{
				sprintf(buffer_ret, "%s / %s", buffer_ret, string_tab17[i+1]);
			}
		}
		bit_shift = bit_shift<<1;
	}

	return buffer_ret;
}

char * GetString_TabelaBateria(unsigned int idn)
{
	switch (idn)
	{
		case 0:
			return string_bateria[0];
		break;

		case 1:
			return string_bateria[1];
		break;

		case 2:
			return string_bateria[2];
		break;

		case 3:
			return string_bateria[3];
		break;
	}

	return string_bateria[0];

}


char * GetPos_Tabela1(unsigned int idn)
{
	return string_tab1[idn];
}


char * GetString_Braco(unsigned int idn)
{
	switch (idn)
	{
		case 0:
			return string_braco[0];
		break;

		case 1:
			return string_braco[1];
		break;

		case 2:
			return string_braco[2];
		break;

		default:
		break;
	}

	return string_braco[0];

}



