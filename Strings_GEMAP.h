/*
 * Strings_GEMAP.h
 *
 *  Created on: May 25, 2014
 *      Author: Denis
 */

#ifndef STRINGS_GEMAP_H_
#define STRINGS_GEMAP_H_

/*
 * Strings Arquivo Exibição
 *
 */

char arq_ini[] = {"[\n"};
char arq_fim[] = {"]\n"};
char _ini[]      				= {"\n;----------   Arquivo.js   ----------\n"};
char _fim[]      				= {"\n;--------   FIM Arquivo.js   --------\n"};

char PassWord_Simbol[] = {"****"};

/* Titulos */
char tit_visaogeral[] = {"{\n \"titulo\" : \"Visão Geral\", \"submenus\" : [\n"};
char tit_erroinit[] = {"{\n \"titulo\" : \"Inicialização\", \"submenus\" : [\n"};
char tit_sistema[] = {",{\n \"titulo\" : \"Sistema\", \"submenus\" : [\n"};
char tit_mod_RTU[] = {",{\n \"titulo\" : \"Módulo RTU\", \"submenus\" : [\n"};
char tit_mod_IHM[] = {",{\n \"titulo\" : \"Módulo IHM\", \"submenus\" : [\n"};
char tit_mod_GPRS[] = {",{\n \"titulo\" : \"Módulo GPRS\", \"submenus\" : [\n"};
char tit_comp_vazao[] = {",{\n \"titulo\" : \"Computador de vazão\", \"submenus\" : [\n"};
char tit_gateway[] = {",{\n \"titulo\" : \"Gateway\", \"submenus\" : [\n"};
char tit_bd_Operador[] = {",{\n \"titulo\" : \"Operador\", \"submenus\" : [\n"};
char tit_fim[] = {"]\n}"};


/* Submenus */
char submenu_visaogeral[] = {"\n { \"titulo\" : \"Visão Geral\",\n \"vars\" : [ \n"};
char submenu_erroinit[] = {"\n { \"titulo\" : \"Erro Encontrado\",\n \"vars\" : [ \n"};
char submenu_principal[] = {"\n { \"titulo\" : \"Principal\",\n \"vars\" : [ \n"};
char submenu_operacao[] = {"\n { \"titulo\" : \"Operação\",\n \"vars\" : [ \n"};
char submenu_operacao_old[] = {"\n { \"titulo\" : \"Última Operação\",\n \"vars\" : [ \n"};
char submenu_diag[] = {"\n { \"titulo\" : \"Diagnósticos\",\n \"vars\" : [ \n"};
char submenu_diag_hw[] = {"\n { \"titulo\" : \"Diagnósticos de hardware\",\n \"vars\" : [ \n"};
char submenu_alarmes[] = {"\n { \"titulo\" : \"Alarmes\",\n \"vars\" : [ \n"};
char submenu_info[] = {"\n { \"titulo\" : \"Informações\",\n \"vars\" : [ \n"};
char submenu_comm[] = {"\n { \"titulo\" : \"Comunicação\",\n \"vars\" : [ \n"};
char submenu_config[] = {"\n { \"titulo\" : \"Configurações\",\n \"vars\" : [ \n"};
char submenu_config_DI[] = {"\n { \"titulo\" : \"Configuração de Entrada Digital\",\n \"vars\" : [ \n"};
char submenu_config_DO[] = {"\n { \"titulo\" : \"Configuração de Saída Digital\",\n \"vars\" : [ \n"};
char submenu_config_AI[] = {"\n { \"titulo\" : \"Configuração da Entrada Analógica\",\n \"vars\" : [ \n"};
char submenu_bd_Operador[] = {"\n { \"titulo\" : \"Operadores\",\n \"vars\" : [ \n"};
char submenu_fim[] = {"]\n}"};



/* Mensangens de Erro durante a inicialização */
char var_Erro1[] = {"{ \"titulo\" : \"Erro\", \"valor\": \" "};
char var_Erro2[] = {"\", \"addr\" : \"\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};

/* Variaveis - Visao Geral > Principal */
/* 45 */
char var_ModoRem1[] = {"{ \"titulo\" : \"Liberação Modo Remoto\", \"valor\": \" "};
char var_ModoRem2[] = {"\", \"addr\" : \"400045\", \"tipo\" : \"u16\", \"acesso\" : \"\", \"fmt\": \"9\"} \n"};

/* 1600 */
char var_LtOpPass1[] = {"{ \"titulo\" : \"Lista Operadores e senhas\", \"valor\": \" "};
char var_LtOpPass2[] = {"\", \"addr\" : \"401600\", \"tipo\" : \"u16\", \"acesso\" : \"\", \"fmt\": \"9\"} \n"};

/* 50 */
char var_TAG1[] = {"{ \"titulo\" : \"TAG\", \"valor\": \" "};
char var_TAG2[] = {"\", \"addr\" : \"300000\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};

char var_Desc1[] = {"{ \"titulo\" : \"Descrição\", \"valor\": \" "};
char var_Desc2[] = {"\", \"addr\" : \"300000\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};

char var_Data1[] = {"{ \"titulo\" : \"Data\", \"valor\": \" "};
char var_Data2[] = {"\", \"addr\" : \"400050\", \"tipo\" : \"bytearray4\", \"acesso\" : \"adm\", \"fmt\": \"99/99/9999\"} \n"};

char var_Hora1[] = {"{ \"titulo\" : \"Hora\", \"valor\": \" "};
char var_Hora2[] = {"\", \"addr\" : \"400052\", \"tipo\" : \"bytearray4\", \"acesso\" : \"adm\", \"fmt\": \"99:99:99\"} \n"};

char var_Stat1_B1[] = {"{ \"titulo\" : \"Status do Sistema - Braço 1\", \"valor\": \" "};
char var_Stat2_B1[] = {"\", \"addr\" : \"300052\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};

char var_Stat1_B2[] = {"{ \"titulo\" : \"Status do Sistema - Braço 2\", \"valor\": \" "};
char var_Stat2_B2[] = {"\", \"addr\" : \"300052\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};

char var_VolCor1[] = {"{ \"titulo\" : \"Volume Corrigido (Inventário)\", \"valor\": \" "};
char var_VolCor2[] = {" \", \"addr\" : \"300052\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};

char var_VolBruto1[] = {"{ \"titulo\" : \"Volume bruto (Inventário)\", \"valor\": \" "};
char var_VolBruto2[] = {" \", \"addr\" : \"300052\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};

char var_Vazao1[] = {"{ \"titulo\" : \"Vazão\", \"valor\": \" "};
char var_Vazao2[] = {" \", \"addr\" : \"300052\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};

char var_Dens1[] = {"{ \"titulo\" : \"Densidade\", \"valor\": \" "};
char var_Dens2[] = {" \", \"addr\" : \"300052\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};

char var_Temp1[] = {"{ \"titulo\" : \"Temperatura\", \"valor\": \" "};
char var_Temp2[] = {" \", \"addr\" : \"300052\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};

char var_Tmodem1[] = {"{ \"titulo\" : \"Tempo decrescente para reset do modem\", \"valor\": \" "};
char var_Tmodem2[] = {" min\", \"addr\" : \"300052\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};

char var_NResetModem1[] = {"{ \"titulo\" : \"Número de reset do modem\", \"valor\": \" "};
char var_NResetModem2[] = {"\", \"addr\" : \"300052\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};


/* Variaveis - Sistema > Operação */
/* 100 */
char var_CodOp1[] = {"{ \"titulo\" : \"Código da operação\", \"valor\": \" "};
char var_CodOp2[] = {"\", \"addr\" : \"400100\", \"tipo\" : \"bytearray10\", \"acesso\" : \"user\", \"fmt\": \"AAAAAAAA\"} \n"};

char var_Lacre1[] = {"{ \"titulo\" : \"Lacre\", \"valor\": \" "};
char var_Lacre2[] = {"\", \"addr\" : \"400105\", \"tipo\" : \"bytearray10\", \"acesso\" : \"user\", \"fmt\": \"AAAAAAAA\"} \n"};

char var_Operad1[] = {"{ \"titulo\" : \"Operador\", \"valor\": \" "};
char var_Operad2[] = {"\", \"addr\" : \"400110\", \"tipo\" : \"bytearray10\", \"acesso\" : \"user\", \"fmt\": \"AAAAAAAA\"} \n"};

char var_IdVeic1[] = {"{ \"titulo\" : \"Veículo\", \"valor\": \" "};
char var_IdVeic2[] = {"\", \"addr\" : \"400115\", \"tipo\" : \"bytearray10\", \"acesso\" : \"user\", \"fmt\": \"AAAAAAAA\"} \n"};

char var_VolRef1[] = {"{ \"titulo\" : \"Volume de Referência\", \"valor\": \" "};
char var_VolRef2[] = {"\", \"addr\" : \"400120\", \"tipo\" : \"u32\", \"acesso\" : \"user\", \"fmt\": \"9999999,9\"} \n"};

char var_Produto1[] = {"{ \"titulo\" : \"Produto\", \"valor\": \" "};
char var_Produto2[] = {"\", \"addr\" : \"300000\", \"tipo\" : \"bytearray4\", \"acesso\" : \"user\", \"fmt\": \" \"} \n"};

char var_ModoOp1[] = {"{ \"titulo\" : \"Modo de operação\", \"valor\": \" "};
char var_ModoOp2[] = {"\", \"addr\" : \"400127\", \"tipo\" : \"u16\", \"acesso\" : \"user\", \"fmt\": \"tabela2\"} \n"};

/* 130 */
char var_DataIniOp1[] = {"{ \"titulo\" : \"Data do início da operação\", \"valor\": \" "};
char var_DataIniOp2[] = {"\", \"addr\" : \"300000\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};

char var_DataSist1[] = {"{ \"titulo\" : \"Data do fim da operação\", \"valor\": \" "};
char var_DataSist2[] = {"\", \"addr\" : \"300000\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};

char var_VolBDesc1[] = {"{ \"titulo\" : \"Volume bruto descarregado\", \"valor\": \" "};
char var_VolBDesc2[] = {"\", \"addr\" : \"300000\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};

char var_VolCDesc1[] = {"{ \"titulo\" : \"Volume corrigido descarregado\", \"valor\": \" "};
char var_VolCDesc2[] = {" \", \"addr\" : \"300000\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};

char var_MedVazao1[] = {"{ \"titulo\" : \"Média da vazão (c/ temperatura compensada)\", \"valor\": \" "};
char var_MedVazao2[] = {" \", \"addr\" : \"300000\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};

char var_MedTemp1[] = {"{ \"titulo\" : \"Média da temperatura\", \"valor\": \" "};
char var_MedTemp2[] = {"\", \"addr\" : \"300000\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};

char var_MedDens1[] = {"{ \"titulo\" : \"Média da densidade (c/ temperatura compensada)\", \"valor\": \" "};
char var_MedDens2[] = {" \", \"addr\" : \"300000\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};

char var_VazaoMax1[] = {"{ \"titulo\" : \"Vazão máxima (c/ temperatura compensada)\", \"valor\": \" "};
char var_VazaoMax2[] = {" \", \"addr\" : \"300000\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};

char var_TempMax1[] = {"{ \"titulo\" : \"Temperatura máxima\", \"valor\": \" "};
char var_TempMax2[] = {" \", \"addr\" : \"300000\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};

char var_TempMin1[] = {"{ \"titulo\" : \"Temperatura mínima\", \"valor\": \" "};
char var_TempMin2[] = {" \", \"addr\" : \"300000\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};

/* 160 */
char var_DensMax1[] = {"{ \"titulo\" : \"Densidade máxima (c/ temperatura compensada)\", \"valor\": \" "};
char var_DensMax2[] = {" \", \"addr\" : \"300000\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};

char var_DensMin1[] = {"{ \"titulo\" : \"Densidade mínima (c/ temperatura compensada)\", \"valor\": \" "};
char var_DensMin2[] = {" \", \"addr\" : \"300000\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};

char var_InvVolBrutoIni1[] = {"{ \"titulo\" : \"Inventário inicial do volume bruto\", \"valor\": \" "};
char var_InvVolBrutoIni2[] = {"\", \"addr\" : \"300000\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};

char var_InvVolCorIni1[] = {"{ \"titulo\" : \"Inventário inicial do volume corrigido\", \"valor\": \" "};
char var_InvVolCorIni2[] = {" \", \"addr\" : \"300000\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};

char var_InvVolBruto1[] = {"{ \"titulo\" : \"Inventário do volume bruto\", \"valor\": \" "};
char var_InvVolBruto2[] = {" \", \"addr\" : \"300000\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};

char var_InvVolCor1[] = {"{ \"titulo\" : \"Inventário do volume corrigido\", \"valor\": \" "};
char var_InvVolCor2[] = {" \", \"addr\" : \"300000\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};


/* Variaveis - Sistema > Última Operação */
char var_CodOp1_ul[] = {"{ \"titulo\" : \"Código da operação\", \"valor\": \" "};
char var_CodOp2_ul[] = {"\", \"addr\" : \"300100\", \"tipo\" : \"bytearray10\", \"acesso\" : \"\", \"fmt\": \"\"} \n"};

char var_Lacre1_ul[] = {"{ \"titulo\" : \"Lacre\", \"valor\": \" "};
char var_Lacre2_ul[] = {"\", \"addr\" : \"300105\", \"tipo\" : \"bytearray10\", \"acesso\" : \"\", \"fmt\": \"\"} \n"};

char var_Operad1_ul[] = {"{ \"titulo\" : \"Operador\", \"valor\": \" "};
char var_Operad2_ul[] = {"\", \"addr\" : \"300110\", \"tipo\" : \"bytearray10\", \"acesso\" : \"\", \"fmt\": \"\"} \n"};

char var_IdVeic1_ul[] = {"{ \"titulo\" : \"Veículo\", \"valor\": \" "};
char var_IdVeic2_ul[] = {"\", \"addr\" : \"300115\", \"tipo\" : \"bytearray10\", \"acesso\" : \"\", \"fmt\": \"\"} \n"};

char var_VolRef1_ul[] = {"{ \"titulo\" : \"Volume de Referência\", \"valor\": \" "};
char var_VolRef2_ul[] = {"\", \"addr\" : \"300120\", \"tipo\" : \"u32\", \"acesso\" : \"\", \"fmt\": \"9999999,9\"} \n"};

char var_Produto1_ul[] = {"{ \"titulo\" : \"Produto\", \"valor\": \" "};
char var_Produto2_ul[] = {"\", \"addr\" : \"300000\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};

char var_ModoOp1_ul[] = {"{ \"titulo\" : \"Modo de operação\", \"valor\": \" "};
char var_ModoOp2_ul[] = {"\", \"addr\" : \"300127\", \"tipo\" : \"u16\", \"acesso\" : \"\", \"fmt\": \"tabela2\"} \n"};



/* Variaveis - Sistema > Diagnósticos */
/* 300 */
char var_Sist_Diag_1[30][80] = {
		{"{ \"titulo\" : \"Status MC1 DO-1 [LF-INV1]\", \"valor\": \" "},
		{"{ \"titulo\" : \"Status MC1 DO-2 [MF-INV1]\", \"valor\": \" "},
		{"{ \"titulo\" : \"Status MC1 DO-3 [HF-INV1]\", \"valor\": \" "},
		{"{ \"titulo\" : \"Status MC1 DO-4 [EN-INV1]\", \"valor\": \" "},
		{"{ \"titulo\" : \"Status MC1 DO-5 [Alarme do Vega II]\", \"valor\": \" "},
		{"{ \"titulo\" : \"Status MC1 DO-6 [SOV01-VENT]\", \"valor\": \" "},
		{"{ \"titulo\" : \"Status MC1 DI-1 [LSH-TQ]\", \"valor\": \" "},
		{"{ \"titulo\" : \"Status MC1 DI-2 [LSL_TQ]\", \"valor\": \" "},
		{"{ \"titulo\" : \"Status MC1 DI-3 [Monitor de Aterramento B2]\", \"valor\": \" "},
		{"{ \"titulo\" : \"Status MC1 DI-4 [S1 - Nível alto]\", \"valor\": \" "},
		{"{ \"titulo\" : \"Status MC1 DI-5 [S2 - Nível médio]\", \"valor\": \" "},
		{"{ \"titulo\" : \"Status MC1 DI-6 [S3 - Nível baixo]\", \"valor\": \" "},
		{"{ \"titulo\" : \"Valor MC1 AI-1 [Reserva]\", \"valor\": \" "},
		{"{ \"titulo\" : \"Status MC1 DI-7 [Monitor de Aterramento B1]\", \"valor\": \" "},
		{"{ \"titulo\" : \"Status MC1 DI-8 [Botão de Emergência]\", \"valor\": \" "},
		{"{ \"titulo\" : \"Status MC2 DO-1 [LF-INV2]\", \"valor\": \" "},
		{"{ \"titulo\" : \"Status MC2 DO-2 [MF-INV2]\", \"valor\": \" "},
		{"{ \"titulo\" : \"Status MC2 DO-3 [HF-INV2]\", \"valor\": \" "},
		{"{ \"titulo\" : \"Status MC2 DO-4 [EN-INV2]\", \"valor\": \" "},
		{"{ \"titulo\" : \"Status MC2 DO-5 [Reserva]\", \"valor\": \" "},
		{"{ \"titulo\" : \"Status MC2 DO-6 [Reserva]\", \"valor\": \" "},
		{"{ \"titulo\" : \"Status MC2 DI-1 [VOC-01]\", \"valor\": \" "},
		{"{ \"titulo\" : \"Status MC2 DI-2 [VOC-02]\", \"valor\": \" "},
		{"{ \"titulo\" : \"Status MC2 DI-3 [VOC-03]\", \"valor\": \" "},
		{"{ \"titulo\" : \"Status MC2 DI-4 [S4 - Nível alto]\", \"valor\": \" "},
		{"{ \"titulo\" : \"Status MC2 DI-5 [S5 - Nível médio]\", \"valor\": \" "},
		{"{ \"titulo\" : \"Status MC2 DI-6 [S6 - Nível baixo]\", \"valor\": \" "},
		{"{ \"titulo\" : \"Valor MC2 AI-1 [Tensão de Alimentação Vac]\", \"valor\": \" "},
		{"{ \"titulo\" : \"Status MC2 DI-7 [Reserva]\", \"valor\": \" "},
		{"{ \"titulo\" : \"Status MC2 DI-8 [Reserva]\", \"valor\": \" "}
};

char var_Sist_Diag_2[30][95] = {
		{"\", \"addr\" : \"300300\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"},
		{"\", \"addr\" : \"300301\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"},
		{"\", \"addr\" : \"300302\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"},
		{"\", \"addr\" : \"300303\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"},
		{"\", \"addr\" : \"300304\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"},
		{"\", \"addr\" : \"300305\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"},
		{"\", \"addr\" : \"300306\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"},
		{"\", \"addr\" : \"300307\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"},
		{"\", \"addr\" : \"300308\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"},
		{"\", \"addr\" : \"300309\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"},
		{"\", \"addr\" : \"300310\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"},
		{"\", \"addr\" : \"300311\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"},
		{"\", \"addr\" : \"300312\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"},
		{"\", \"addr\" : \"300314\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"},
		{"\", \"addr\" : \"300316\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"},
		{"\", \"addr\" : \"300318\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"},
		{"\", \"addr\" : \"300319\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"},
		{"\", \"addr\" : \"300320\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"},
		{"\", \"addr\" : \"300321\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"},
		{"\", \"addr\" : \"300322\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"},
		{"\", \"addr\" : \"300323\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"},
		{"\", \"addr\" : \"300324\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"},
		{"\", \"addr\" : \"300325\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"},
		{"\", \"addr\" : \"300326\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"},
		{"\", \"addr\" : \"300327\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"},
		{"\", \"addr\" : \"300328\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"},
		{"\", \"addr\" : \"300329\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"},
		{"V\", \"addr\" : \"300330\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"},
		{"\", \"addr\" : \"300332\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"},
		{"\", \"addr\" : \"300334\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"}
};


/* Variaveis - Sistema > Alarmes */
/* 50 */
char var_Alm_Err_1[] = {"{ \"titulo\" : \"Alarme de Erro \", \"valor\": \" "};
char var_Alm_Err_2[] = {"\", \"addr\" : \"300000\", \"tipo\" : \"alarmes\", \"acesso\" : \"\", \"fmt\": \"tabela 15\"} \n"};

char var_Alm_War_1[] = {"{ \"titulo\" : \"Alarme de Warning \", \"valor\": \" "};
char var_Alm_War_2[] = {"\", \"addr\" : \"300000\", \"tipo\" : \"alarmes\", \"acesso\" : \"\", \"fmt\": \"tabela 16\"} \n"};

char var_Alm_Inf_1[] = {"{ \"titulo\" : \"Alarme de Erro 2 \", \"valor\": \" "};
char var_Alm_Inf_2[] = {"\", \"addr\" : \"300000\", \"tipo\" : \"alarmes\", \"acesso\" : \"\", \"fmt\": \"tabela 17\"} \n"};


/* Variaveis - Sistema > Informações */
/* 500 */
char var_Ciclos_1[] = {"{ \"titulo\" : \"Número de ciclos de inicialização \", \"valor\": \" "};
char var_Ciclos_2[] = {"\", \"addr\" : \"300000\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};

char var_Data_ult_ini_1[] = {"{ \"titulo\" : \"Data da última inicialização \", \"valor\": \" "};
char var_Data_ult_ini_2[] = {"\", \"addr\" : \"300000\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};

char var_TAGSys_1[] = {"{ \"titulo\" : \"TAG de sistema \", \"valor\": \" "};
char var_TAGSys_2[] = {"\", \"addr\" : \"400505\", \"tipo\" : \"bytearray16\", \"acesso\" : \"adm\", \"fmt\": \"AAAAAAAAAAAAAAAA\"} \n"};

char var_NumSer_1[] = {"{ \"titulo\" : \"Número de série \", \"valor\": \" "};
char var_NumSer_2[] = {"\", \"addr\" : \"300000\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};

char var_CntDesc_1[] = {"{ \"titulo\" : \"Contador de descarregamento \", \"valor\": \" "};
char var_CntDesc_2[] = {"\", \"addr\" : \"300000\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};


/* Variaveis - Módulo RTU > Diagnóstico de hardware */
/* 400 */
char var_Tensao_rede_1[] = {"{ \"titulo\" : \"Tensão de rede \", \"valor\": \" "};
char var_Tensao_rede_2[] = {" V\", \"addr\" : \"300000\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};

char var_Tensao_vcc_1[] = {"{ \"titulo\" : \"Tensão VCC \", \"valor\": \" "};
char var_Tensao_vcc_2[] = {" V\", \"addr\" : \"300000\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};

char var_Temp_placa_1[] = {"{ \"titulo\" : \"Temperatura da placa \", \"valor\": \" "};
char var_Temp_placa_2[] = {" °C\", \"addr\" : \"300000\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};

char var_Tensao_placa_1[] = {"{ \"titulo\" : \"Tensão da placa \", \"valor\": \" "};
char var_Tensao_placa_2[] = {" V\", \"addr\" : \"300000\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};

char var_Tensao_bat_1[] = {"{ \"titulo\" : \"Tensão da bateria \", \"valor\": \" "};
char var_Tensao_bat_2[] = {" V\", \"addr\" : \"300000\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};

char var_Tensao_ent_1[] = {"{ \"titulo\" : \"Tensão de entrada \", \"valor\": \" "};
char var_Tensao_ent_2[] = {" V\", \"addr\" : \"300000\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};


/* Variaveis - Módulo RTU > Configurações de Entrada Digital */
/* 1200 */
char var_ConfigDI_1[36][58] = {
		{"{ \"titulo\" : \"Configuração MC1 DI1 \", \"valor\": \" "},
		{"{ \"titulo\" : \"Parâmetro 1 MC1 DI1 \", \"valor\": \" "},
		{"{ \"titulo\" : \"Parâmetro 2 MC1 DI1 \", \"valor\": \" "},
		{"{ \"titulo\" : \"Configuração MC1 DI2 \", \"valor\": \" "},
		{"{ \"titulo\" : \"Parâmetro 1 MC1 DI2 \", \"valor\": \" "},
		{"{ \"titulo\" : \"Parâmetro 2 MC1 DI2 \", \"valor\": \" "},
		{"{ \"titulo\" : \"Configuração MC1 DI3 \", \"valor\": \" "},
		{"{ \"titulo\" : \"Parâmetro 1 MC1 DI3 \", \"valor\": \" "},
		{"{ \"titulo\" : \"Parâmetro 2 MC1 DI3 \", \"valor\": \" "},
		{"{ \"titulo\" : \"Configuração MC1 DI4 \", \"valor\": \" "},
		{"{ \"titulo\" : \"Parâmetro 1 MC1 DI4 \", \"valor\": \" "},
		{"{ \"titulo\" : \"Parâmetro 2 MC1 DI4 \", \"valor\": \" "},
		{"{ \"titulo\" : \"Configuração MC1 DI5 \", \"valor\": \" "},
		{"{ \"titulo\" : \"Parâmetro 1 MC1 DI5 \", \"valor\": \" "},
		{"{ \"titulo\" : \"Parâmetro 2 MC1 DI5 \", \"valor\": \" "},
		{"{ \"titulo\" : \"Configuração MC1 DI6 \", \"valor\": \" "},
		{"{ \"titulo\" : \"Parâmetro 1 MC1 DI6 \", \"valor\": \" "},
		{"{ \"titulo\" : \"Parâmetro 2 MC1 DI6 \", \"valor\": \" "},
		{"{ \"titulo\" : \"Configuração MC2 DI1 \", \"valor\": \" "},
		{"{ \"titulo\" : \"Parâmetro 1 MC2 DI1 \", \"valor\": \" "},
		{"{ \"titulo\" : \"Parâmetro 2 MC2 DI1 \", \"valor\": \" "},
		{"{ \"titulo\" : \"Configuração MC2 DI2 \", \"valor\": \" "},
		{"{ \"titulo\" : \"Parâmetro 1 MC2 DI2 \", \"valor\": \" "},
		{"{ \"titulo\" : \"Parâmetro 2 MC2 DI2 \", \"valor\": \" "},
		{"{ \"titulo\" : \"Configuração MC2 DI3 \", \"valor\": \" "},
		{"{ \"titulo\" : \"Parâmetro 1 MC2 DI3 \", \"valor\": \" "},
		{"{ \"titulo\" : \"Parâmetro 2 MC2 DI3 \", \"valor\": \" "},
		{"{ \"titulo\" : \"Configuração MC2 DI4 \", \"valor\": \" "},
		{"{ \"titulo\" : \"Parâmetro 1 MC2 DI4 \", \"valor\": \" "},
		{"{ \"titulo\" : \"Parâmetro 2 MC2 DI4 \", \"valor\": \" "},
		{"{ \"titulo\" : \"Configuração MC2 DI5 \", \"valor\": \" "},
		{"{ \"titulo\" : \"Parâmetro 1 MC2 DI5 \", \"valor\": \" "},
		{"{ \"titulo\" : \"Parâmetro 2 MC2 DI5 \", \"valor\": \" "},
		{"{ \"titulo\" : \"Configuração MC2 DI6 \", \"valor\": \" "},
		{"{ \"titulo\" : \"Parâmetro 1 MC2 DI6 \", \"valor\": \" "},
		{"{ \"titulo\" : \"Parâmetro 2 MC2 DI6 \", \"valor\": \" "}
};

char var_ConfigDI_2[36][103] = {
		{" \", \"addr\" : \"401200\", \"tipo\" : \"bitstring16\", \"acesso\" : \"adm\", \"fmt\": \"Tabela 12\"} \n"},
		{" \", \"addr\" : \"401201\", \"tipo\" : \"u16\", \"acesso\" : \"adm\", \"fmt\": \"00000\"} \n"},
		{" \", \"addr\" : \"401202\", \"tipo\" : \"u16\", \"acesso\" : \"adm\", \"fmt\": \"00000\"} \n"},
		{" \", \"addr\" : \"401203\", \"tipo\" : \"bitstring16\", \"acesso\" : \"adm\", \"fmt\": \"Tabela 12\"} \n"},
		{" \", \"addr\" : \"401204\", \"tipo\" : \"u16\", \"acesso\" : \"adm\", \"fmt\": \"00000\"} \n"},
		{" \", \"addr\" : \"401205\", \"tipo\" : \"u16\", \"acesso\" : \"adm\", \"fmt\": \"00000\"} \n"},
		{" \", \"addr\" : \"401206\", \"tipo\" : \"bitstring16\", \"acesso\" : \"adm\", \"fmt\": \"Tabela 12\"} \n"},
		{" \", \"addr\" : \"401207\", \"tipo\" : \"u16\", \"acesso\" : \"adm\", \"fmt\": \"00000\"} \n"},
		{" \", \"addr\" : \"401208\", \"tipo\" : \"u16\", \"acesso\" : \"adm\", \"fmt\": \"00000\"} \n"},
		{" \", \"addr\" : \"401209\", \"tipo\" : \"bitstring16\", \"acesso\" : \"adm\", \"fmt\": \"Tabela 12\"} \n"},
		{" \", \"addr\" : \"401210\", \"tipo\" : \"u16\", \"acesso\" : \"adm\", \"fmt\": \"00000\"} \n"},
		{" \", \"addr\" : \"401211\", \"tipo\" : \"u16\", \"acesso\" : \"adm\", \"fmt\": \"00000\"} \n"},
		{" \", \"addr\" : \"401212\", \"tipo\" : \"bitstring16\", \"acesso\" : \"adm\", \"fmt\": \"Tabela 12\"} \n"},
		{" \", \"addr\" : \"401213\", \"tipo\" : \"u16\", \"acesso\" : \"adm\", \"fmt\": \"00000\"} \n"},
		{" \", \"addr\" : \"401214\", \"tipo\" : \"u16\", \"acesso\" : \"adm\", \"fmt\": \"00000\"} \n"},
		{" \", \"addr\" : \"401215\", \"tipo\" : \"bitstring16\", \"acesso\" : \"adm\", \"fmt\": \"Tabela 12\"} \n"},
		{" \", \"addr\" : \"401216\", \"tipo\" : \"u16\", \"acesso\" : \"adm\", \"fmt\": \"00000\"} \n"},
		{" \", \"addr\" : \"401217\", \"tipo\" : \"u16\", \"acesso\" : \"adm\", \"fmt\": \"00000\"} \n"},
		{" \", \"addr\" : \"401218\", \"tipo\" : \"bitstring16\", \"acesso\" : \"adm\", \"fmt\": \"Tabela 12\"} \n"},
		{" \", \"addr\" : \"401219\", \"tipo\" : \"u16\", \"acesso\" : \"adm\", \"fmt\": \"00000\"} \n"},
		{" \", \"addr\" : \"401220\", \"tipo\" : \"u16\", \"acesso\" : \"adm\", \"fmt\": \"00000\"} \n"},
		{" \", \"addr\" : \"401221\", \"tipo\" : \"bitstring16\", \"acesso\" : \"adm\", \"fmt\": \"Tabela 12\"} \n"},
		{" \", \"addr\" : \"401222\", \"tipo\" : \"u16\", \"acesso\" : \"adm\", \"fmt\": \"00000\"} \n"},
		{" \", \"addr\" : \"401223\", \"tipo\" : \"u16\", \"acesso\" : \"adm\", \"fmt\": \"00000\"} \n"},
		{" \", \"addr\" : \"401224\", \"tipo\" : \"bitstring16\", \"acesso\" : \"adm\", \"fmt\": \"Tabela 12\"} \n"},
		{" \", \"addr\" : \"401225\", \"tipo\" : \"u16\", \"acesso\" : \"adm\", \"fmt\": \"00000\"} \n"},
		{" \", \"addr\" : \"401226\", \"tipo\" : \"u16\", \"acesso\" : \"adm\", \"fmt\": \"00000\"} \n"},
		{" \", \"addr\" : \"401227\", \"tipo\" : \"bitstring16\", \"acesso\" : \"adm\", \"fmt\": \"Tabela 12\"} \n"},
		{" \", \"addr\" : \"401228\", \"tipo\" : \"u16\", \"acesso\" : \"adm\", \"fmt\": \"00000\"} \n"},
		{" \", \"addr\" : \"401229\", \"tipo\" : \"u16\", \"acesso\" : \"adm\", \"fmt\": \"00000\"} \n"},
		{" \", \"addr\" : \"401230\", \"tipo\" : \"bitstring16\", \"acesso\" : \"adm\", \"fmt\": \"Tabela 12\"} \n"},
		{" \", \"addr\" : \"401231\", \"tipo\" : \"u16\", \"acesso\" : \"adm\", \"fmt\": \"00000\"} \n"},
		{" \", \"addr\" : \"401232\", \"tipo\" : \"u16\", \"acesso\" : \"adm\", \"fmt\": \"00000\"} \n"},
		{" \", \"addr\" : \"401233\", \"tipo\" : \"bitstring16\", \"acesso\" : \"adm\", \"fmt\": \"Tabela 12\"} \n"},
		{" \", \"addr\" : \"401234\", \"tipo\" : \"u16\", \"acesso\" : \"adm\", \"fmt\": \"00000\"} \n"},
		{" \", \"addr\" : \"401235\", \"tipo\" : \"u16\", \"acesso\" : \"adm\", \"fmt\": \"00000\"} \n"}
};

/* Variaveis - Módulo RTU > Configurações de Saída Digital */
/* 1236 */
char var_ConfigDO_1[24][57] = {
		{"{ \"titulo\" : \"Configuração MC1 DO1\", \"valor\": \" "},
		{"{ \"titulo\" : \"Parâmetro MC1 DO1\", \"valor\": \" "},
		{"{ \"titulo\" : \"Configuração MC1 DO2\", \"valor\": \" "},
		{"{ \"titulo\" : \"Parâmetro MC1 DO2\", \"valor\": \" "},
		{"{ \"titulo\" : \"Configuração MC1 DO3\", \"valor\": \" "},
		{"{ \"titulo\" : \"Parâmetro MC1 DO3\", \"valor\": \" "},
		{"{ \"titulo\" : \"Configuração MC1 DO4\", \"valor\": \" "},
		{"{ \"titulo\" : \"Parâmetro MC1 DO4\", \"valor\": \" "},
		{"{ \"titulo\" : \"Configuração MC1 DO5\", \"valor\": \" "},
		{"{ \"titulo\" : \"Parâmetro MC1 DO5\", \"valor\": \" "},
		{"{ \"titulo\" : \"Configuração MC1 DO6\", \"valor\": \" "},
		{"{ \"titulo\" : \"Parâmetro MC1 DO6\", \"valor\": \" "},
		{"{ \"titulo\" : \"Configuração MC2 DO1\", \"valor\": \" "},
		{"{ \"titulo\" : \"Parâmetro MC2 DO1\", \"valor\": \" "},
		{"{ \"titulo\" : \"Configuração MC2 DO2\", \"valor\": \" "},
		{"{ \"titulo\" : \"Parâmetro MC2 DO2\", \"valor\": \" "},
		{"{ \"titulo\" : \"Configuração MC2 DO3\", \"valor\": \" "},
		{"{ \"titulo\" : \"Parâmetro MC2 DO3\", \"valor\": \" "},
		{"{ \"titulo\" : \"Configuração MC2 DO4\", \"valor\": \" "},
		{"{ \"titulo\" : \"Parâmetro MC2 DO4\", \"valor\": \" "},
		{"{ \"titulo\" : \"Configuração MC2 DO5\", \"valor\": \" "},
		{"{ \"titulo\" : \"Parâmetro MC2 DO5\", \"valor\": \" "},
		{"{ \"titulo\" : \"Configuração MC2 DO6\", \"valor\": \" "},
		{"{ \"titulo\" : \"Parâmetro MC2 DO6\", \"valor\": \" "}
};

char var_ConfigDO_2[24][103] = {
		{" \", \"addr\" : \"401236\", \"tipo\" : \"bitstring16\", \"acesso\" : \"adm\", \"fmt\": \"Tabela 13\"} \n"},
		{" \", \"addr\" : \"401237\", \"tipo\" : \"u16\", \"acesso\" : \"adm\", \"fmt\": \"00000\"} \n"},
		{" \", \"addr\" : \"401238\", \"tipo\" : \"bitstring16\", \"acesso\" : \"adm\", \"fmt\": \"Tabela 13\"} \n"},
		{" \", \"addr\" : \"401239\", \"tipo\" : \"u16\", \"acesso\" : \"adm\", \"fmt\": \"00000\"} \n"},
		{" \", \"addr\" : \"401240\", \"tipo\" : \"bitstring16\", \"acesso\" : \"adm\", \"fmt\": \"Tabela 13\"} \n"},
		{" \", \"addr\" : \"401241\", \"tipo\" : \"u16\", \"acesso\" : \"adm\", \"fmt\": \"00000\"} \n"},
		{" \", \"addr\" : \"401242\", \"tipo\" : \"bitstring16\", \"acesso\" : \"adm\", \"fmt\": \"Tabela 13\"} \n"},
		{" \", \"addr\" : \"401243\", \"tipo\" : \"u16\", \"acesso\" : \"adm\", \"fmt\": \"00000\"} \n"},
		{" \", \"addr\" : \"401244\", \"tipo\" : \"bitstring16\", \"acesso\" : \"adm\", \"fmt\": \"Tabela 13\"} \n"},
		{" \", \"addr\" : \"401245\", \"tipo\" : \"u16\", \"acesso\" : \"adm\", \"fmt\": \"00000\"} \n"},
		{" \", \"addr\" : \"401246\", \"tipo\" : \"bitstring16\", \"acesso\" : \"adm\", \"fmt\": \"Tabela 13\"} \n"},
		{" \", \"addr\" : \"401247\", \"tipo\" : \"u16\", \"acesso\" : \"adm\", \"fmt\": \"00000\"} \n"},
		{" \", \"addr\" : \"401248\", \"tipo\" : \"bitstring16\", \"acesso\" : \"adm\", \"fmt\": \"Tabela 13\"} \n"},
		{" \", \"addr\" : \"401249\", \"tipo\" : \"u16\", \"acesso\" : \"adm\", \"fmt\": \"00000\"} \n"},
		{" \", \"addr\" : \"401250\", \"tipo\" : \"bitstring16\", \"acesso\" : \"adm\", \"fmt\": \"Tabela 13\"} \n"},
		{" \", \"addr\" : \"401251\", \"tipo\" : \"u16\", \"acesso\" : \"adm\", \"fmt\": \"00000\"} \n"},
		{" \", \"addr\" : \"401252\", \"tipo\" : \"bitstring16\", \"acesso\" : \"adm\", \"fmt\": \"Tabela 13\"} \n"},
		{" \", \"addr\" : \"401253\", \"tipo\" : \"u16\", \"acesso\" : \"adm\", \"fmt\": \"00000\"} \n"},
		{" \", \"addr\" : \"401254\", \"tipo\" : \"bitstring16\", \"acesso\" : \"adm\", \"fmt\": \"Tabela 13\"} \n"},
		{" \", \"addr\" : \"401255\", \"tipo\" : \"u16\", \"acesso\" : \"adm\", \"fmt\": \"00000\"} \n"},
		{" \", \"addr\" : \"401256\", \"tipo\" : \"bitstring16\", \"acesso\" : \"adm\", \"fmt\": \"Tabela 13\"} \n"},
		{" \", \"addr\" : \"401257\", \"tipo\" : \"u16\", \"acesso\" : \"adm\", \"fmt\": \"00000\"} \n"},
		{" \", \"addr\" : \"401258\", \"tipo\" : \"bitstring16\", \"acesso\" : \"adm\", \"fmt\": \"Tabela 13\"} \n"},
		{" \", \"addr\" : \"401259\", \"tipo\" : \"u16\", \"acesso\" : \"adm\", \"fmt\": \"00000\"} \n"}
};

/* Variaveis - Módulo RTU > Configurações de Entrada Analógica */
/* 1260 */
char var_ConfigAI_1[18][57] = {
		{"{ \"titulo\" : \"Configuração MC1 AI1\", \"valor\": \" "},
		{"{ \"titulo\" : \"Parâmetro 1 MC1 AI1\", \"valor\": \" "},
		{"{ \"titulo\" : \"Parâmetro 2 MC1 AI1\", \"valor\": \" "},
		{"{ \"titulo\" : \"Configuração MC1 AI2\", \"valor\": \" "},
		{"{ \"titulo\" : \"Parâmetro 1 MC1 AI2\", \"valor\": \" "},
		{"{ \"titulo\" : \"Parâmetro 2 MC1 AI2\", \"valor\": \" "},
		{"{ \"titulo\" : \"Configuração MC1 AI3\", \"valor\": \" "},
		{"{ \"titulo\" : \"Parâmetro 1 MC1 AI3\", \"valor\": \" "},
		{"{ \"titulo\" : \"Parâmetro 2 MC1 AI3\", \"valor\": \" "},
		{"{ \"titulo\" : \"Configuração MC2 AI1\", \"valor\": \" "},
		{"{ \"titulo\" : \"Parâmetro 1 MC2 AI1\", \"valor\": \" "},
		{"{ \"titulo\" : \"Parâmetro 2 MC2 AI1\", \"valor\": \" "},
		{"{ \"titulo\" : \"Configuração MC2 AI2\", \"valor\": \" "},
		{"{ \"titulo\" : \"Parâmetro 1 MC2 AI2\", \"valor\": \" "},
		{"{ \"titulo\" : \"Parâmetro 2 MC2 AI2\", \"valor\": \" "},
		{"{ \"titulo\" : \"Configuração MC2 AI3\", \"valor\": \" "},
		{"{ \"titulo\" : \"Parâmetro 1 MC2 AI3\", \"valor\": \" "},
		{"{ \"titulo\" : \"Parâmetro 2 MC2 AI3\", \"valor\": \" "}
};

char var_ConfigAI_2[18][103] = {
		{" \", \"addr\" : \"401260\", \"tipo\" : \"bitstring16\", \"acesso\" : \"adm\", \"fmt\": \"Tabela 14\"} \n"},
		{" \", \"addr\" : \"401261\", \"tipo\" : \"u16\", \"acesso\" : \"adm\", \"fmt\": \"00000\"} \n"},
		{" \", \"addr\" : \"401262\", \"tipo\" : \"u16\", \"acesso\" : \"adm\", \"fmt\": \"00000\"} \n"},
		{" \", \"addr\" : \"401263\", \"tipo\" : \"bitstring16\", \"acesso\" : \"adm\", \"fmt\": \"Tabela 14\"} \n"},
		{" \", \"addr\" : \"401264\", \"tipo\" : \"u16\", \"acesso\" : \"adm\", \"fmt\": \"00000\"} \n"},
		{" \", \"addr\" : \"401265\", \"tipo\" : \"u16\", \"acesso\" : \"adm\", \"fmt\": \"00000\"} \n"},
		{" \", \"addr\" : \"401266\", \"tipo\" : \"bitstring16\", \"acesso\" : \"adm\", \"fmt\": \"Tabela 14\"} \n"},
		{" \", \"addr\" : \"401267\", \"tipo\" : \"u16\", \"acesso\" : \"adm\", \"fmt\": \"00000\"} \n"},
		{" \", \"addr\" : \"401268\", \"tipo\" : \"u16\", \"acesso\" : \"adm\", \"fmt\": \"00000\"} \n"},
		{" \", \"addr\" : \"401269\", \"tipo\" : \"bitstring16\", \"acesso\" : \"adm\", \"fmt\": \"Tabela 14\"} \n"},
		{" \", \"addr\" : \"401270\", \"tipo\" : \"u16\", \"acesso\" : \"adm\", \"fmt\": \"00000\"} \n"},
		{" \", \"addr\" : \"401271\", \"tipo\" : \"u16\", \"acesso\" : \"adm\", \"fmt\": \"00000\"} \n"},
		{" \", \"addr\" : \"401272\", \"tipo\" : \"bitstring16\", \"acesso\" : \"adm\", \"fmt\": \"Tabela 14\"} \n"},
		{" \", \"addr\" : \"401273\", \"tipo\" : \"u16\", \"acesso\" : \"adm\", \"fmt\": \"00000\"} \n"},
		{" \", \"addr\" : \"401274\", \"tipo\" : \"u16\", \"acesso\" : \"adm\", \"fmt\": \"00000\"} \n"},
		{" \", \"addr\" : \"401275\", \"tipo\" : \"bitstring16\", \"acesso\" : \"adm\", \"fmt\": \"Tabela 14\"} \n"},
		{" \", \"addr\" : \"401276\", \"tipo\" : \"u16\", \"acesso\" : \"adm\", \"fmt\": \"00000\"} \n"},
		{" \", \"addr\" : \"401277\", \"tipo\" : \"u16\", \"acesso\" : \"adm\", \"fmt\": \"00000\"} \n"}
};


/* Variaveis - Módulo RTU > Comunicação */
/* 800 */
char var_IQC1_1[] = {"{ \"titulo\" : \"IQC [MC2]\", \"valor\": \" "};
char var_IQC1_2[] = {"%\", \"addr\" : \"300000\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};

char var_TX1_1[] = {"{ \"titulo\" : \"TX [MC2]\", \"valor\": \" "};
char var_TX1_2[] = {"\", \"addr\" : \"300000\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};

char var_RX1_1[] = {"{ \"titulo\" : \"RX [MC2]\", \"valor\": \" "};
char var_RX1_2[] = {"\", \"addr\" : \"300000\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};

char var_IQC2_1[] = {"{ \"titulo\" : \"IQC [Vega II] \", \"valor\": \" "};
char var_IQC2_2[] = {"%\", \"addr\" : \"300000\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};

char var_TX2_1[] = {"{ \"titulo\" : \"TX [Vega II]\", \"valor\": \" "};
char var_TX2_2[] = {"\", \"addr\" : \"300000\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};

char var_RX2_1[] = {"{ \"titulo\" : \"RX [Vega II]\", \"valor\": \" "};
char var_RX2_2[] = {"\", \"addr\" : \"300000\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};

char var_IQC3_1[] = {"{ \"titulo\" : \"IQC [IHM] \", \"valor\": \" "};
char var_IQC3_2[] = {"%\", \"addr\" : \"300000\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};

char var_TX3_1[] = {"{ \"titulo\" : \"TX [IHM]\", \"valor\": \" "};
char var_TX3_2[] = {"\", \"addr\" : \"300000\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};

char var_RX3_1[] = {"{ \"titulo\" : \"RX [IHM]\", \"valor\": \" "};
char var_RX3_2[] = {"\", \"addr\" : \"300000\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};

char var_RSSI_1[] = {"{ \"titulo\" : \"Nível de sinal\", \"valor\": \" "};
char var_RSSI_2[] = {"\", \"addr\" : \"300000\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};



/* Variaveis - Módulo RTU > Informações */
/* 65500 */
char var_ver_sw_1[] = {"{ \"titulo\" : \"Versão de software \", \"valor\": \" "};
char var_ver_sw_2[] = {"\", \"addr\" : \"300000\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};

char var_data_fabr_1[] = {"{ \"titulo\" : \"Data de fabricação \", \"valor\": \" "};
char var_data_fabr_2[] = {"\", \"addr\" : \"300000\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};

char var_uc_id_1[] = {"{ \"titulo\" : \"Identificação do microcontrolador \", \"valor\": \" "};
char var_uc_id_2[] = {"\", \"addr\" : \"300000\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};

char var_num_ser_1[] = {"{ \"titulo\" : \"Número de série \", \"valor\": \" "};
char var_num_ser_2[] = {"\", \"addr\" : \"300000\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};

char var_modem_1[] = {"{ \"titulo\" : \"Tipo de modem \", \"valor\": \" "};
char var_modem_2[] = {"\", \"addr\" : \"300000\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};

char var_IMEI_1[] = {"{ \"titulo\" : \"IMEI \", \"valor\": \" "};
char var_IMEI_2[] = {"\", \"addr\" : \"300000\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};


/* Variaveis - Módulo IHM > Informações */
/* 400 */
char var_cnt_init_1[] = {"{ \"titulo\" : \"Contador de inicialização \", \"valor\": \" "};
char var_cnt_init_2[] = {"\", \"addr\" : \"300000\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};


/* Variaveis - Gateway > Informações */
/* 700 */
char var_ip_1[] = {"{ \"titulo\" : \"IP \", \"valor\": \" "};
char var_ip_2[] = {"\", \"addr\" : \"300700\", \"tipo\" : \"bytearray16\", \"acesso\" : \"adm\", \"fmt\": \" \"} \n"};

char var_mask_1[] = {"{ \"titulo\" : \"Máscara de rede \", \"valor\": \" "};
char var_mask_2[] = {"\", \"addr\" : \"300708\", \"tipo\" : \"bytearray16\", \"acesso\" : \"adm\", \"fmt\": \" \"} \n"};

char var_gateway_1[] = {"{ \"titulo\" : \"Gateway \", \"valor\": \" "};
char var_gateway_2[] = {"\", \"addr\" : \"300716\", \"tipo\" : \"bytearray16\", \"acesso\" : \"adm\", \"fmt\": \" \"} \n"};

/* 750 */
char var_ver_sist_1[] = {"{ \"titulo\" : \"Versão do sistema \", \"valor\": \" "};
char var_ver_sist_2[] = {"\", \"addr\" : \"300000\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};

char var_ver_main_1[] = {"{ \"titulo\" : \"Versão Main Server \", \"valor\": \" "};
char var_ver_main_2[] = {"\", \"addr\" : \"300000\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};

char var_ver_modbs_1[] = {"{ \"titulo\" : \"Versão Modbus TCP Server \", \"valor\": \" "};
char var_ver_modbs_2[] = {"\", \"addr\" : \"300000\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};

char var_ver_updatesw_1[] = {"{ \"titulo\" : \"Versão Update SW\", \"valor\": \" "};
char var_ver_updatesw_2[] = {"\", \"addr\" : \"300000\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};

char var_ver_modbclient_1[] = {"{ \"titulo\" : \"Versão Modbus Client\", \"valor\": \" "};
char var_ver_modbclient_2[] = {"\", \"addr\" : \"300000\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};

char var_online_1[] = {"{ \"titulo\" : \"Online\", \"valor\": \" "};
char var_online_2[] = {"\", \"addr\" : \"300000\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};

char var_TempCore_1[] = {"{ \"titulo\" : \"Temperatura Core\", \"valor\": \" "};
char var_TempCore_2[] = {"°C\", \"addr\" : \"300000\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};

char var_erro_Comm1[] = {"{ \"titulo\" : \"Erro COMM \", \"valor\": \" "};
char var_erro_Comm2[] = {"\", \"addr\" : \"300000\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};

char var_erro_RX1[] = {"{ \"titulo\" : \"Serial RX Counter \", \"valor\": \" "};
char var_erro_RX2[] = {"\", \"addr\" : \"300000\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};

char var_MSLock1[] = {"{ \"titulo\" : \"MainServer Lock \", \"valor\": \" "};
char var_MSLock2[] = {"\", \"addr\" : \"300000\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};

char var_StatDatalog1[] = {"{ \"titulo\" : \"Datalog \", \"valor\": \" "};
char var_StatDatalog2[] = {"%\", \"addr\" : \"300000\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};


/* Variaveis - Computador de vazão > Config. */
/* 1000 */
char var_vazao_unid_1[] = {"{ \"titulo\" : \"Unidade de vazão \", \"valor\": \" "};
char var_vazao_unid_2[] = {"\", \"addr\" : \"301000\", \"tipo\" : \"u16\", \"acesso\" : \"eol\", \"fmt\": \" \"} \n"};

char var_temp_unid_1[] = {"{ \"titulo\" : \"Unidade de temperatura \", \"valor\": \" "};
char var_temp_unid_2[] = {"\", \"addr\" : \"301001\", \"tipo\" : \"u16\", \"acesso\" : \"eol\", \"fmt\": \" \"} \n"};

char var_densid_unid_1[] = {"{ \"titulo\" : \"Unidade de densidade para cálculo do volume corrigido \", \"valor\": \" "};
char var_densid_unid_2[] = {"\", \"addr\" : \"301002\", \"tipo\" : \"u16\", \"acesso\" : \"eol\", \"fmt\": \" \"} \n"};

char var_vol_unid_1[] = {"{ \"titulo\" : \"Unidade de volume \", \"valor\": \" "};
char var_vol_unid_2[] = {"\", \"addr\" : \"301003\", \"tipo\" : \"u16\", \"acesso\" : \"eol\", \"fmt\": \" \"} \n"};


/* Variaveis - Sistema > Informações */
/* 600 */
char var_ver_firm_1[] = {"{ \"titulo\" : \"Versão de firmware \", \"valor\": \" "};
char var_ver_firm_2[] = {"\", \"addr\" : \"300000\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};

char var_ver_modb_1[] = {"{ \"titulo\" : \"Versão modbus \", \"valor\": \" "};
char var_ver_modb_2[] = {"\", \"addr\" : \"300000\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};

char var_sts_bat_1[] = {"{ \"titulo\" : \"Status bateria \", \"valor\": \" "};
char var_sts_bat_2[] = {"\", \"addr\" : \"300000\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};

char var_modo_op_1[] = {"{ \"titulo\" : \"Modo de operação \", \"valor\": \" "};
char var_modo_op_2[] = {"\", \"addr\" : \"300000\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};

char var_cnt_op_1[] = {"{ \"titulo\" : \"Contador de operações \", \"valor\": \" "};
char var_cnt_op_2[] = {"\", \"addr\" : \"300000\", \"tipo\" : \"bytearray4\", \"acesso\" : \"\", \"fmt\": \" \"} \n"};



/* Variaveis - Operadores*/
char var_Op1i[] =  {"{ \"titulo\" : \"Operador 1\", \"valor\": \" "};
char var_Op1f[] =  {"\", \"addr\" : \"401500\", \"tipo\" : \"bytearray8\", \"acesso\" : \"adm\", \"fmt\": \"AAAAAAAA\"} \n"};
char var_Pass1i[] =  {"{ \"titulo\" : \"Senha 1\", \"valor\": \" "};
char var_Pass1f[] =  {"\", \"addr\" : \"401504\", \"tipo\" : \"password\", \"acesso\" : \"adm\", \"fmt\": \"****\"} \n"};

char var_Op2i[] =  {"{ \"titulo\" : \"Operador 2\", \"valor\": \" "};
char var_Op2f[] =  {"\", \"addr\" : \"401506\", \"tipo\" : \"bytearray8\", \"acesso\" : \"adm\", \"fmt\": \"AAAAAAAA\"} \n"};
char var_Pass2i[] =  {"{ \"titulo\" : \"Senha 2\", \"valor\": \" "};
char var_Pass2f[] =  {"\", \"addr\" : \"401510\", \"tipo\" : \"password\", \"acesso\" : \"adm\", \"fmt\": \"****\"} \n"};

char var_Op3i[] =  {"{ \"titulo\" : \"Operador 3\", \"valor\": \" "};
char var_Op3f[] =  {"\", \"addr\" : \"401512\", \"tipo\" : \"bytearray8\", \"acesso\" : \"adm\", \"fmt\": \"AAAAAAAA\"} \n"};
char var_Pass3i[] =  {"{ \"titulo\" : \"Senha 3\", \"valor\": \" "};
char var_Pass3f[] =  {"\", \"addr\" : \"401516\", \"tipo\" : \"password\", \"acesso\" : \"adm\", \"fmt\": \"****\"} \n"};

char var_Op4i[] =  {"{ \"titulo\" : \"Operador 4\", \"valor\": \" "};
char var_Op4f[] =  {"\", \"addr\" : \"401518\", \"tipo\" : \"bytearray8\", \"acesso\" : \"adm\", \"fmt\": \"AAAAAAAA\"} \n"};
char var_Pass4i[] =  {"{ \"titulo\" : \"Senha 4\", \"valor\": \" "};
char var_Pass4f[] =  {"\", \"addr\" : \"401522\", \"tipo\" : \"password\", \"acesso\" : \"adm\", \"fmt\": \"****\"} \n"};

char var_Op5i[] =  {"{ \"titulo\" : \"Operador 5\", \"valor\": \" "};
char var_Op5f[] =  {"\", \"addr\" : \"401524\", \"tipo\" : \"bytearray8\", \"acesso\" : \"adm\", \"fmt\": \"AAAAAAAA\"} \n"};
char var_Pass5i[] =  {"{ \"titulo\" : \"Senha 5\", \"valor\": \" "};
char var_Pass5f[] =  {"\", \"addr\" : \"401528\", \"tipo\" : \"password\", \"acesso\" : \"adm\", \"fmt\": \"****\"} \n"};

char var_Op6i[] =  {"{ \"titulo\" : \"Operador 6\", \"valor\": \" "};
char var_Op6f[] =  {"\", \"addr\" : \"401530\", \"tipo\" : \"bytearray8\", \"acesso\" : \"adm\", \"fmt\": \"AAAAAAAA\"} \n"};
char var_Pass6i[] =  {"{ \"titulo\" : \"Senha 6\", \"valor\": \" "};
char var_Pass6f[] =  {"\", \"addr\" : \"401534\", \"tipo\" : \"password\", \"acesso\" : \"adm\", \"fmt\": \"****\"} \n"};

char var_Op7i[] =  {"{ \"titulo\" : \"Operador 7\", \"valor\": \" "};
char var_Op7f[] =  {"\", \"addr\" : \"401536\", \"tipo\" : \"bytearray8\", \"acesso\" : \"adm\", \"fmt\": \"AAAAAAAA\"} \n"};
char var_Pass7i[] =  {"{ \"titulo\" : \"Senha 7\", \"valor\": \" "};
char var_Pass7f[] =  {"\", \"addr\" : \"401540\", \"tipo\" : \"password\", \"acesso\" : \"adm\", \"fmt\": \"****\"} \n"};

char var_Op8i[] =  {"{ \"titulo\" : \"Operador 8\", \"valor\": \" "};
char var_Op8f[] =  {"\", \"addr\" : \"401542\", \"tipo\" : \"bytearray8\", \"acesso\" : \"adm\", \"fmt\": \"AAAAAAAA\"} \n"};
char var_Pass8i[] =  {"{ \"titulo\" : \"Senha 8\", \"valor\": \" "};
char var_Pass8f[] =  {"\", \"addr\" : \"401546\", \"tipo\" : \"password\", \"acesso\" : \"adm\", \"fmt\": \"****\"} \n"};

char var_Op9i[] =  {"{ \"titulo\" : \"Operador 9\", \"valor\": \" "};
char var_Op9f[] =  {"\", \"addr\" : \"401548\", \"tipo\" : \"bytearray8\", \"acesso\" : \"adm\", \"fmt\": \"AAAAAAAA\"} \n"};
char var_Pass9i[] =  {"{ \"titulo\" : \"Senha 9\", \"valor\": \" "};
char var_Pass9f[] =  {"\", \"addr\" : \"401552\", \"tipo\" : \"password\", \"acesso\" : \"adm\", \"fmt\": \"****\"} \n"};

char var_Op10i[] =  {"{ \"titulo\" : \"Operador 10\", \"valor\": \" "};
char var_Op10f[] =  {"\", \"addr\" : \"401554\", \"tipo\" : \"bytearray8\", \"acesso\" : \"adm\", \"fmt\": \"AAAAAAAA\"} \n"};
char var_Pass10i[] =  {"{ \"titulo\" : \"Senha 10\", \"valor\": \" "};
char var_Pass10f[] =  {"\", \"addr\" : \"401558\", \"tipo\" : \"password\", \"acesso\" : \"adm\", \"fmt\": \"****\"} \n"};

/*
 * Strings Arquivo Histórico
 *
 */
char arqhist_ini[] = {"[{ \"historico\" : [ \n"};
char arqhist_fim[] = {"\n]}]"};

char varhist_POSICAO[]        = {" \"posicao\" : \""};
char varhist_DATAINICIO[]     = {" \"data_inicio\" : \""};
char varhist_DATATERMINO[]    = {" \"data_termino\" : \""};
char varhist_VEICULO[]        = {" \"veiculo\" : \""};
char varhist_OPERADOR[]       = {" \"operador\" : \""};
char varhist_LACRE[]       	  = {" \"lacre\" : \""};
char varhist_COD_OP[]         = {" \"cod_operacao\" : \""};
char varhist_MODO_OP[]        = {" \"modo_operacao\" : \""};
char varhist_VOL_BRUTO[]      = {" \"volume_bruto\" : \""};
char varhist_VOL_CORRIGIDO[]  = {" \"volume_corrigido\" : \""};
char varhist_VAZAO[]          = {" \"vazao\" : \""};
char varhist_TEMP[]           = {" \"temperatura\" : \""};
char varhist_DENS[]           = {" \"densidade\" : \""};
char varhist_ERROR[]          = {" \"error\" : \""};
char varhist_ERROR2[]          = {" \"error2\" : \""};
char varhist_WARNING[]        = {" \"warning\" : \""};

#endif /* STRINGS_GEMAP_H_ */
