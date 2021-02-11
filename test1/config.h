#ifndef __CONFIG_H
#define __CONFIG_H

#include <stdio.h>

#define TRUE 1
#define FALSE 0

#define CFG_DS_USERNAME  101
#define CFG_DS_PASSWORD  102
#define CFG_DS_TYPE      103
#define CFG_DS_TYPE_STR  104
#define CFG_DS_CONN_STR  105
#define CFG_DS_FETCHSIZE 106
#define CFG_DS_CHARSET   107

#define CFG_QR_TEXT       201
#define CFG_QR_VAR_CNT    202
#define CFG_QR_PM_DEF_CNT 203
#define CFG_QR_PM_DEFS    204
#define CFG_QR_PM_VAL_CNT 205
#define CFG_QR_PM_VALS    206

#define CFG_OP_FILE       301
#define CFG_OP_LOG        302
#define CFG_OP_HEADER     303
#define CFG_OP_HEADER_STR 304
#define CFG_OP_QUOTE      305
#define CFG_OP_QUOTE_STR  306
#define CFG_OP_COLSEP     307
#define CFG_OP_ROWSEP     308
#define CFG_OP_DATEFMT    309

#define CFG_DS_TYPE_UNKNOWN   10000
#define CFG_DS_TYPE_TNSNAME   10001
#define CFG_DS_TYPE_TNSCFG    10002

#define CFG_OP_QUOTE_TYPE_AUTO   10101
#define CFG_OP_QUOTE_TYPE_ALWAYS 10102
#define CFG_OP_QUOTE_TYPE_NEVER  10103

#define CFG_DS_FETCHSIZE_DEFAULT  500
#define CFG_DS_CHARSET_DEFAULT    "AL32UTF8"
#define CFG_OP_HEADER_DEFAULT     1
#define CFG_OP_QUOTE_DEFAULT      CFG_OP_QUOTE_TYPE_AUTO
#define CFG_OP_COLSEP_DEFAULT     ","
#define CFG_OP_ROWSEP_DEFAULT     "\r\n"
#define CFG_OP_DATEFMT_DEFAULT    "%Y-%m-%d %H:%M:%S"

#define CFG_SRC_TYPE_FILE 1
#define CFG_SRC_TYPE_STR  2

typedef struct cfg_qr_pm_def {
    char* name;
    int*  use_cnt;
    int*  indexes;
} cfg_qr_pm_def_t;

typedef struct cfg_qr_pm_val {
    char* name;
    char* value;
} cfg_qr_pm_val_t;

typedef struct cfg_config {
    char*            ds_username;
    char*            ds_password;
    int              ds_type;
    char*            ds_conn_str;
    int              ds_fetchsize;
    char*            ds_charset;
    char*            qr_text;
    int              qr_var_cnt;
    int              qr_pm_def_cnt;
    cfg_qr_pm_def_t* qr_pm_defs;
    int              qr_pm_val_cnt;
    cfg_qr_pm_val_t* qr_pm_vals;
    char*            op_file;
    char*            op_log;
    int              op_header;
    int              op_quote;
    char*            op_colsep;
    char*            op_rowsep;
    char*            op_datefmt;
} cfg_config_t;

cfg_config_t* cfg_create(const int src_type, const void* cfg_src);
void cfg_delete(cfg_config_t* config);

char* cfg_get_str(cfg_config_t* config, const int key);
void cfg_set_str(cfg_config_t* config, const int key, const char* value);

int cfg_get_int(cfg_config_t* config, const int key);
void cfg_set_int(cfg_config_t* config, const int key, const int value);

void* cfg_get_obj(cfg_config_t* config, const int key);

void cfg_set_ds(cfg_config_t* config, const char* username, const char* password, const char* conn_str);

#endif /* ! __CONFIG_H */
