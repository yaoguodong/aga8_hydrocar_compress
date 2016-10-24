/* -----------------------------------------------------------------------------	*/
/* PROGRAMA			: odisins_tareas.pc												*/
/* DESCRIPCION		: Órdenes de Normalización con tareas de medidor y empalme		*/
/* AUTOR			: AM															*/
/* FECHA			: Octubre 2016.													*/
/* ------------------------------------------------------------------------------	*/

#include <SYNString.h>
#include <api_correo.h>			

/* Header principal (comunicación db) */
EXEC SQL INCLUDE parametros.h;	

/* Variables Generales */
#define DEBUG 0
char C255_msg[256];
char delimiter='\0';/*0x9;tab*/
int flag_records=FALSE;

/* Parametros de entrada */
char C001_par_conexion[2];
char C003_par_empresa[4];
char C007_par_rol[8];
char C010_par_tipo_tarea[11];
char C010_par_fec_inicio[11];
char C010_par_fec_fin[11];

/* Constantes interacción db */
#define NODATAFOUND 1403
#define SQLNOTFOUND ( sqlca.sqlcode == NODATAFOUND )

EXEC SQL BEGIN DECLARE SECTION;
/* Declaracion estructuras datos medidores */
/*
C015_nro_ord_norm	Nro. Orden			NroOrden			NRO_ORD_NORM			NUMBER (15)
C010_nro_suministro		Nro. Cliente		Cliente				NRO_SUMINISTRO			NUMBER (10)
C010_tarifa				Tarifa				Tarifa				CAMPO19					CHAR (10 Byte)
C100_clave_tarifa		Clave tarifa		ClaveTarifa			DESCRIPCION				VARCHAR2 (100 Byte)
C100_comuna				Comuna				Comuna				DESCRIPCION				VARCHAR2 (100 Byte)
C120_tipo_ejecucion		Tipo Ejecución		TipoEjec			VALOR1 + DESCRIPCION	VARCHAR2 (20 Byte) + VARCHAR2 (100 Byte)
C045_contratista		Contratista			Contratista			NOM_CONTRATISTA			VARCHAR2 (45 Byte)
C100_tipo_resultado		Tipo Resultado		Resultado			DESCRIPCION				VARCHAR2 (100 Byte)
C100_estado_propiedad	Estado Propiedad	EstPropiedad		NOM_ACCION				VARCHAR2 (100 Byte)
C050_anormalidad		Anormalidad			Anormalidad			h3.nom_accion			VARCHAR2 (50 Byte)
C001_tarea_ejecutada	Tarea Ejecutada		h8.ejecutada		EJECUTADA				CHAR (1 Byte)
C004_tipo_tarea			Tipo Tarea 			h3.cod_tarea		COD_TAREA				CHAR (4 Byte)
C010_fec_ejecucion		Fecha de ejecución	FecEjecucion		FEC_HORA_FIN_EJE		DATE
C010_fec_creacion		Fecha creación		FecCreacion			FEC_CREACION			DATE
C010_fec_asignacion		Fecha Asignación	FecAsignacion		FEC_ASIGNACION			DATE
C010_fec_envio			Fecha de envío		FecEnvio 			FEC_ENVIO				DATE	(agregado)
C010_fec_finalizacion	Fecha Finalización	FecFinalizada		FEC_TER_ORD				DATE
C010_fec_atendida		Fecha Atendida		FecAtendida			FEC_INGRESO_DATOS		DATE
C2000_observaciones		Observación			observaciones		OBSERVACIONES			VARCHAR2 (2000 Byte)
C010_fec_hora_ini_eje	fec_hora_ini_eje	fec_hora_ini_eje	fec_hora_ini_eje		DATE
C010_fec_devoluc_prev	FecDevPrev			fec_devoluc_prev	fec_devoluc_prev		DATE
*/

char C015_nro_ord_norm[16]		;		EXEC SQL VAR C015_nro_ord_norm 		IS STRING(16) 	;
char C010_nro_suministro[11]	;		EXEC SQL VAR C010_nro_suministro 	IS STRING(11) 	;
char C010_tarifa[11]			;		EXEC SQL VAR C010_tarifa 			IS STRING(11) 	;
char C100_clave_tarifa[101]		;		EXEC SQL VAR C100_clave_tarifa 		IS STRING(101) 	;
char C100_comuna[101]			;		EXEC SQL VAR C100_comuna 			IS STRING(101) 	;
char C120_tipo_ejecucion[121]	;		EXEC SQL VAR C120_tipo_ejecucion 	IS STRING(121) 	;
char C045_contratista[46]		;		EXEC SQL VAR C045_contratista 		IS STRING(46) 	;
char C100_tipo_resultado[101]	;		EXEC SQL VAR C100_tipo_resultado 	IS STRING(101) 	;
char C100_estado_propiedad[101]	;		EXEC SQL VAR C100_estado_propiedad 	IS STRING(101) 	;
char C050_anormalidad[51]		;		EXEC SQL VAR C050_anormalidad 		IS STRING(51) 	;
char C001_tarea_ejecutada[2]	;		EXEC SQL VAR C001_tarea_ejecutada 	IS STRING(2) 	;
char C004_tipo_tarea[5]			;		EXEC SQL VAR C004_tipo_tarea 		IS STRING(5) 	;
char C010_fec_ejecucion[11]		;		EXEC SQL VAR C010_fec_ejecucion 	IS STRING(11) 	;
char C010_fec_creacion[11]		;		EXEC SQL VAR C010_fec_creacion 		IS STRING(11) 	;
char C010_fec_asignacion[11]	;		EXEC SQL VAR C010_fec_asignacion 	IS STRING(11) 	;
char C010_fec_envio[11]			;		EXEC SQL VAR C010_fec_envio 		IS STRING(11) 	;
char C010_fec_finalizacion[11]	;		EXEC SQL VAR C010_fec_finalizacion 	IS STRING(11) 	;
char C010_fec_atendida[11]		;		EXEC SQL VAR C010_fec_atendida 		IS STRING(11) 	;
char C2000_observaciones[2001]	;		EXEC SQL VAR C2000_observaciones 	IS STRING(2001) ;

char C010_fec_hora_ini_eje[11]	;		EXEC SQL VAR C010_fec_hora_ini_eje 	IS STRING(11) 	;
char C010_fec_devoluc_prev[11]	;		EXEC SQL VAR C010_fec_devoluc_prev 	IS STRING(11) 	;



/* Variables Uso Email */
char C1024_from_name[1024]					; EXEC SQL VAR C1024_from_name			IS STRING(1024);
char c1024_from_email[1024]					; EXEC SQL VAR c1024_from_email			IS STRING(1024);
char C1024_to_name[1024]					; EXEC SQL VAR C1024_to_name			IS STRING(1024);
char C1024_to_email[1024]					; EXEC SQL VAR C1024_to_email			IS STRING(1024);
char C1024_cc_email[1024]					; EXEC SQL VAR C1024_cc_email			IS STRING(1024);
char C1024_cco_email[1024]					; EXEC SQL VAR C1024_cco_email			IS STRING(1024);
char C1024_subject[1024]					; EXEC SQL VAR C1024_subject			IS STRING(1024);
char C2048_body[2048]						; EXEC SQL VAR C2048_body				IS STRING(2048);
char C1024_email[1024]						; EXEC SQL VAR C1024_email				IS STRING(1024);
char C050_param_adj[51]						; EXEC SQL VAR C050_param_adj			IS STRING(51);


/* Variables generales CHAR */
char	C255_nom_file[256]		;	 		EXEC SQL VAR C255_nom_file		IS STRING(256)	;

EXEC SQL END DECLARE SECTION;

/* Variables Uso General */
int flag_email=FALSE;



/*************************************************************************/
/* Concatena buffer con variables incluidas                              */
/*************************************************************************/
long strpcat(char *str, char *msg, ...)
{
    /*double *args;*/
    va_list args;
    char buffer[1024];
    va_start(args, msg);
    vsprintf(buffer,msg,args);
    va_end(args);
    strcat(str,buffer);
    return(strlen(str));
}

/*                        ABRE CURSOR PRINCIPAL                                */
SQL_OPEN_medidores(){
    char C3000_sql_sente[3001];
	
	memset(C3000_sql_sente, '\0', sizeof(C3000_sql_sente));
		
	strcpy(C3000_sql_sente, " select distinct ");
	strpcat(C3000_sql_sente, "     h6.nro_ord_norm NroOrden, ");
	strpcat(C3000_sql_sente, "     h6.nro_suministro Cliente, ");
	strpcat(C3000_sql_sente, "     n5.campo19 Tarifa, ");
	strpcat(C3000_sql_sente, "         (select n11.descripcion from nucssb0011 n11 where n11.cod_empresa = n5.cod_empresa and n11.nomtabla = 'CLATAR' and trim(n5.campo20) = n11.codigo) ClaveTarifa, ");
	strpcat(C3000_sql_sente, "         (select n11.descripcion from nucssb0011 n11 where n11.cod_empresa = n19.cod_empresa and n11.nomtabla = 'COMUNA' and n19.campo22 = n11.codigo) Comuna, ");
	strpcat(C3000_sql_sente, "         (select n11.valor1 || ' - ' || n11.descripcion   ");
	strpcat(C3000_sql_sente, "         from hurssb0039 h39,nucssb0011 n11   ");
	strpcat(C3000_sql_sente, "         where h39.cod_empresa = h6.cod_empresa   ");
	strpcat(C3000_sql_sente, "         and h39.id_folio_preselecc = h6.id_folio_preselecc   ");
	strpcat(C3000_sql_sente, "         and n11.cod_empresa = h6.cod_empresa   ");
	strpcat(C3000_sql_sente, "         and n11.nomtabla = 'EJEINSP'  and n11.codigo=h39.tip_ejecucion) TipoEjec, ");
	strpcat(C3000_sql_sente, "         (select n11.descripcion  ");
	strpcat(C3000_sql_sente, "         from nucssb0011 n11 ");
	strpcat(C3000_sql_sente, "         where n11.cod_empresa = h6.cod_empresa  ");
	strpcat(C3000_sql_sente, "         and n11.nomtabla = 'RESNORM'  ");
	strpcat(C3000_sql_sente, "     and n11.codigo = h6.tip_resultado) Resultado, ");
	strpcat(C3000_sql_sente, "         (select descripcion from nucssb0011 n11 where n11.nomtabla = 'ESTPROP' and n11.cod_empresa = h6.cod_empresa  and n11.codigo = h6.est_propiedad  ) EstPropiedad, ");
	strpcat(C3000_sql_sente, "     h3.nom_accion, ");
	strpcat(C3000_sql_sente, "     h3.cod_tarea,  ");
	strpcat(C3000_sql_sente, "     h8.ejecutada, ");
	strpcat(C3000_sql_sente, "     to_char(h6.fec_creacion,'dd/mm/yyyy hh24:mi:ss') FecCreacion, ");
	strpcat(C3000_sql_sente, "     to_char(h6.fec_asignacion,'dd/mm/yyyy hh24:mi:ss') FecAsignacion, ");
	strpcat(C3000_sql_sente, "     to_char(h6.fec_envio,'dd/mm/yyyy hh24:mi:ss') FecEnvio, ");
	strpcat(C3000_sql_sente, "     to_char(h6.fec_devoluc_prev,'dd/mm/yyyy') FecDevPrev, ");
	strpcat(C3000_sql_sente, "     to_char(h6.fec_hora_fin_eje ,'dd/mm/yyyy hh24:mi:ss') FecEjecucion, ");
	strpcat(C3000_sql_sente, "     to_char(h6.fec_ingreso_datos ,'dd/mm/yyyy hh24:mi:ss') FecAtendida, ");
	strpcat(C3000_sql_sente, "         (select to_char(t1.fec_ter_ord ,'dd/mm/yyyy hh24:mi:ss') ");
	strpcat(C3000_sql_sente, "          from tomssb0001 t1 ");
	strpcat(C3000_sql_sente, "          where t1.cod_empresa = h6.cod_empresa ");
	strpcat(C3000_sql_sente, "          and t1.nro_orden = h6.nro_ord_norm ");
	strpcat(C3000_sql_sente, "          and t1.tip_orden = 'NORM') FecFinalizada, ");
	strpcat(C3000_sql_sente, "         (select t1.nom_contratista from tasssb0001 t1 where t1.cod_empresa = h6.cod_empresa and t1.cod_contratista = h6.cod_contratista) Contratista, ");
	strpcat(C3000_sql_sente, "     h6.fec_hora_ini_eje, ");
	strpcat(C3000_sql_sente, "     h6.observaciones ");
	strpcat(C3000_sql_sente, " from hurssb0006 h6, hurssb0008 h8, nucssb0005 n5,nucssb0019 n19, hurssb0003 h3 ");
	strpcat(C3000_sql_sente, " where h6.cod_empresa = %s  " , C003_par_empresa);
	strpcat(C3000_sql_sente, " and h6.tip_ord_norm = 'O' ");
	strpcat(C3000_sql_sente, " and h6.fec_hora_ini_eje >= to_date('%s' || ' 00:00:00' , 'dd/mm/yyyy hh24:mi:ss') ", C010_par_fec_inicio);
	strpcat(C3000_sql_sente, " and h6.fec_hora_ini_eje <= to_date('%s' || ' 23:59:59' , 'dd/mm/yyyy hh24:mi:ss') ", C010_par_fec_fin);
	strpcat(C3000_sql_sente, " and h6.est_orden = 'F' ");
	strpcat(C3000_sql_sente, " and h6.cod_empresa = h8.cod_empresa ");
	strpcat(C3000_sql_sente, " and h6.nro_ord_norm = h8.nro_ord_norm ");
	strpcat(C3000_sql_sente, " and h6.tip_ord_norm = h8.tip_ord_norm ");
	strpcat(C3000_sql_sente, " and h8.cod_empresa = h3.cod_empresa ");
	strpcat(C3000_sql_sente, " and h8.cod_accion = h3.cod_accion ");
	strpcat(C3000_sql_sente, " and h6.nro_suministro = n5.nro_suministro ");
	strpcat(C3000_sql_sente, " and h6.cod_empresa = n5.cod_empresa ");
	strpcat(C3000_sql_sente, " and h6.nro_suministro  = n19.nro_suministro  ");
	strpcat(C3000_sql_sente, " and h6.cod_empresa = n19.cod_empresa ");
	strpcat(C3000_sql_sente, " and n19.tip_dir = 'S' ");
	strpcat(C3000_sql_sente, " order by h6.nro_ord_norm \n");
		
	
	EXEC SQL DECLARE p_medidores STATEMENT;
    do_error("DECLARE p_medidores (SQL_OPEN_medidores)");

	EXEC SQL PREPARE p_medidores FROM :C3000_sql_sente;
    do_error("PREPARE p_medidores (SQL_OPEN_medidores)");

	EXEC SQL DECLARE cur_medidores CURSOR FOR p_medidores;
    do_error("DECLARE cur_medidores (SQL_OPEN_medidores)");

	EXEC SQL OPEN cur_medidores;
    do_error("OPEN cur_medidores (SQL_OPEN_medidores)");

	/* Debugger */
	if(DEBUG){
		printf("------------------------------------------------------\n");
		printf("DEBUG[SQL_OPEN_medidores]\n");
		printf("Resultado OK\n");
		printf("------------------------------------------------------\n\n");
	}	
	
	return ( TRUE );
}

/*                        FECTCH CURSOR PRINCIPAL                            */
int SQL_FETCH_medidores(){
	int		iRet;

	memset(C015_nro_ord_norm, 	'\0', sizeof(C015_nro_ord_norm));
	memset(C010_nro_suministro, '\0', sizeof(C010_nro_suministro));
	memset(C010_tarifa, '\0', sizeof(C010_tarifa));
	memset(C100_clave_tarifa, '\0', sizeof(C100_clave_tarifa));
	memset(C100_comuna, '\0', sizeof(C100_comuna));
	memset(C120_tipo_ejecucion, '\0', sizeof(C120_tipo_ejecucion));
	memset(C045_contratista, '\0', sizeof(C045_contratista));
	memset(C100_tipo_resultado, '\0', sizeof(C100_tipo_resultado));
	memset(C100_estado_propiedad, '\0', sizeof(C100_estado_propiedad));
	memset(C050_anormalidad, '\0', sizeof(C050_anormalidad));
	memset(C001_tarea_ejecutada, '\0', sizeof(C001_tarea_ejecutada));
	memset(C004_tipo_tarea, '\0', sizeof(C004_tipo_tarea));
	memset(C010_fec_ejecucion, '\0', sizeof(C010_fec_ejecucion));
	memset(C010_fec_creacion, '\0', sizeof(C010_fec_creacion));
	memset(C010_fec_asignacion, '\0', sizeof(C010_fec_asignacion));
	memset(C010_fec_envio, '\0', sizeof(C010_fec_envio));
	memset(C010_fec_finalizacion, '\0', sizeof(C010_fec_finalizacion));
	memset(C010_fec_atendida, '\0', sizeof(C010_fec_atendida));
	memset(C2000_observaciones, '\0', sizeof(C2000_observaciones));
	memset(C010_fec_hora_ini_eje, '\0', sizeof(C010_fec_hora_ini_eje));
	memset(C010_fec_devoluc_prev, '\0', sizeof(C010_fec_devoluc_prev));
	

/*
C015_nro_ord_norm	Nro. Orden			NroOrden			NRO_ORD_NORM			NUMBER (15)
C010_nro_suministro		Nro. Cliente		Cliente				NRO_SUMINISTRO			NUMBER (10)
C010_tarifa				Tarifa				Tarifa				CAMPO19					CHAR (10 Byte)
C100_clave_tarifa		Clave tarifa		ClaveTarifa			DESCRIPCION				VARCHAR2 (100 Byte)
C100_comuna				Comuna				Comuna				DESCRIPCION				VARCHAR2 (100 Byte)
C120_tipo_ejecucion		Tipo Ejecución		TipoEjec			VALOR1 + DESCRIPCION	VARCHAR2 (20 Byte) + VARCHAR2 (100 Byte)
C045_contratista		Contratista			Contratista			NOM_CONTRATISTA			VARCHAR2 (45 Byte)
C100_tipo_resultado		Tipo Resultado		Resultado			DESCRIPCION				VARCHAR2 (100 Byte)
C100_estado_propiedad	Estado Propiedad	EstPropiedad		NOM_ACCION				VARCHAR2 (100 Byte)
C050_anormalidad		Anormalidad			Anormalidad			h3.nom_accion			VARCHAR2 (50 Byte)
C001_tarea_ejecutada	Tarea Ejecutada		h8.ejecutada		EJECUTADA				CHAR (1 Byte)
C004_tipo_tarea			Tipo Tarea 			h3.cod_tarea		COD_TAREA				CHAR (4 Byte)
C010_fec_ejecucion		Fecha de ejecución	FecEjecucion		FEC_HORA_FIN_EJE		DATE
C010_fec_creacion		Fecha creación		FecCreacion			FEC_CREACION			DATE
C010_fec_asignacion		Fecha Asignación	FecAsignacion		FEC_ASIGNACION			DATE
C010_fec_envio			Fecha de envío		FecEnvio 			FEC_ENVIO				DATE	(agregado)
C010_fec_finalizacion	Fecha Finalización	FecFinalizada		FEC_TER_ORD				DATE
C010_fec_atendida		Fecha Atendida		FecAtendida			FEC_INGRESO_DATOS		DATE
C2000_observaciones		Observación			observaciones		OBSERVACIONES			VARCHAR2 (2000 Byte)
C010_fec_hora_ini_eje	fec_hora_ini_eje	fec_hora_ini_eje	fec_hora_ini_eje		DATE
C010_fec_devoluc_prev	FecDevPrev			fec_devoluc_prev	fec_devoluc_prev		DATE
*/

    EXEC SQL 
		FETCH cur_medidores 
		INTO :C015_nro_ord_norm,:C010_nro_suministro,:C010_tarifa,:C100_clave_tarifa,:C100_comuna
		,:C120_tipo_ejecucion,:C100_tipo_resultado,:C100_estado_propiedad,:C050_anormalidad,:C004_tipo_tarea
		,:C001_tarea_ejecutada,:C010_fec_creacion,:C010_fec_asignacion,:C010_fec_envio,:C010_fec_devoluc_prev
		,:C010_fec_ejecucion,:C010_fec_atendida,C010_fec_finalizacion,C045_contratista,:C010_fec_hora_ini_eje
		,:C2000_observaciones;

    iRet = do_error("FETCH cur_medidores (SQL_FETCH_medidores)");
    if ( iRet == TRUE )
        return ( FALSE );

	/* Debugger */
	if(DEBUG){
		printf("------------------------------------------------------\n");
		printf("DEBUG[SQL_FETCH_medidores]\n");
		printf("Resultado OK\n");
		printf("------------------------------------------------------\n\n");
	}

	if(SQLNOTFOUND){return ( FALSE );}
	else		   {return ( TRUE );}
}


/* ------------------------------------------------------------------------- */
/*                        PROCESAMIENTO DE DATOS                                */
/* ------------------------------------------------------------------------- */
int bfnProcesar(){
	FILE	*fpMedidores=NULL;
	char    C2000_Buffer[2001]; 	EXEC SQL VAR C2000_Buffer IS STRING(2001) ;
	int iLecturas = 0;
	int i = 0;

	if (SQL_OPEN_medidores())
	{
		while (SQL_FETCH_medidores())
		{

		}
	
	}
		
	return ( TRUE );
}

/* ------------------------------------------------------------------------- */
/*                        PROGRAMA  PRINCIPAL                                */
/* ------------------------------------------------------------------------- */
main(int argc,char **argv)
{
   if( argc != 7)
    {
	printf("Use : %s <N° Conexion> <Empresa> <Rol> <Tipo de Tarea> <FecInicio> <FecFin> \n",argv[0]);
	exit(1);
    }

	strcpy( C001_par_conexion,			argv[1] );
	strcpy( C003_par_empresa,			argv[2] );
	strcpy( C007_par_rol,				argv[3] );
	strcpy( C010_par_tipo_tarea,		argv[4] );
	strcpy( C010_par_fec_inicio,		argv[5] );
	strcpy( C010_par_fec_fin,			argv[6] );

	sql_conexion(C001_par_conexion);

	
	if(!bfnProcesar()){
		printf("Error\n");
		exit(1);
	}else{
		printf("OK\n");
		exit(0);
	}
}               
