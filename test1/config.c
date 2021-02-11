#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <yaml.h>

#include "config.h"

#define CFG_PARSE_SUCCESS 1
#define CFG_PARSE_ERROR   0

static void cfg_init(cfg_config_t* config);
static int cfg_parse(cfg_config_t* config, yaml_parser_t* parser);
static yaml_parser_t* cfg_yaml_parser_create(void);
static void cfg_yaml_parser_delete(yaml_parser_t* parser);

static char* cfg_get_const_str(const int key);
static void cfg_set_qr_text(cfg_config_t* config, const char* qr_text);
static void cfg_set_op_header(cfg_config_t* config, const char* op_header_str);
static void cfg_set_op_quote(cfg_config_t* config, const char* op_quote_str);
static char* cfg_dup_string(const char* str);

cfg_config_t* cfg_create(const int src_type, const void* cfg_src) {

    cfg_config_t* config;
    int success = FALSE;

    yaml_parser_t* parser = cfg_yaml_parser_create();
    if (src_type == CFG_SRC_TYPE_FILE) {
        FILE* file = (FILE*)cfg_src;
        yaml_parser_set_input_file(parser, file);
    }
    else if (src_type == CFG_SRC_TYPE_STR) {
        char* str = (char*)cfg_src;
        size_t length = strlen(str);
        yaml_parser_set_input_string(parser, str, length);
    }
    else {
        // TODO: raise error
        return NULL;
    }

    config = (cfg_config_t*)malloc(sizeof(cfg_config_t));
    if (config == NULL) {
        // TODO: raise error
        return NULL;
    }
    else {
        cfg_init(config);
    }

    int result = cfg_parse(config, parser);
    cfg_yaml_parser_delete(parser);

    if (result == CFG_PARSE_ERROR) {
        // TODO: raise error
        return NULL;
    }

    return config;
}

void cfg_delete(cfg_config_t* config) {

    if (config != NULL) {
        if (config->ds_username != NULL)
            free(config->ds_username);
        if (config->ds_password != NULL)
            free(config->ds_password);
        if (config->ds_conn_str != NULL)
            free(config->ds_conn_str);
        if (config->ds_charset != NULL)
            free(config->ds_charset);
        if (config->qr_text != NULL)
            free(config->qr_text);
        if (config->op_file != NULL)
            free(config->op_file);
        if (config->op_log != NULL)
            free(config->op_log);
        if (config->op_colsep != NULL)
            free(config->op_colsep);
        if (config->op_rowsep != NULL)
            free(config->op_rowsep);
        if (config->op_datefmt != NULL)
            free(config->op_datefmt);

        if (config->qr_pm_defs != NULL) {
            // TODO: free array
            free(config->qr_pm_defs);
        }

        if (config->qr_pm_vals != NULL) {
            // TODO: free array
            free(config->qr_pm_vals);
        }

        free(config);
    }
}

static void cfg_init(cfg_config_t* config) {
    config->ds_username   = NULL;
    config->ds_password   = NULL;
    config->ds_type       = 0;
    config->ds_conn_str   = NULL;
    config->ds_fetchsize  = CFG_DS_FETCHSIZE_DEFAULT;
    config->ds_charset    = NULL;
    config->qr_text       = NULL;
    config->qr_var_cnt    = 0;
    config->qr_pm_def_cnt = 0;
    config->qr_pm_defs    = NULL;
    config->qr_pm_val_cnt = 0;
    config->qr_pm_vals    = NULL;
    config->op_file       = NULL;
    config->op_log        = NULL;
    config->op_header     = CFG_OP_HEADER_DEFAULT;
    config->op_quote      = CFG_OP_QUOTE_DEFAULT;
    config->op_colsep     = NULL;
    config->op_rowsep     = NULL;
    config->op_datefmt    = NULL;
}

static yaml_parser_t* cfg_yaml_parser_create(void) {
    yaml_parser_t* parser = (yaml_parser_t*)malloc(sizeof(yaml_parser_t));
    if (parser == NULL)
        return NULL;

    yaml_parser_initialize(parser);
    return parser;
}

static void cfg_yaml_parser_delete(yaml_parser_t* parser) {
    if (parser != NULL) {
        yaml_parser_delete(parser);
        free(parser);
    }
}

static int cfg_parse(cfg_config_t* config, yaml_parser_t* parser) {

    yaml_event_t event;
    int done = 0;

    while (!done) {
        /* Get the next event. */
        if (!yaml_parser_parse(parser, &event)) {
            return CFG_PARSE_ERROR;
        }

        /*
        ...
        Process the event.
        ...
        */

        /* Are we finished? */
        done = (event.type == YAML_STREAM_END_EVENT);

        /* The application is responsible for destroying the event object. */
        yaml_event_delete(&event);
    }

    return CFG_PARSE_SUCCESS;
}

char* cfg_get_str(cfg_config_t* config, const int key) {

    switch (key) {
    case CFG_DS_USERNAME:     return config->ds_username;
    case CFG_DS_PASSWORD:     return config->ds_password;
    case CFG_DS_TYPE_STR:     return cfg_get_const_str(config->ds_type);
    case CFG_DS_CONN_STR:     return config->ds_conn_str;
    case CFG_DS_CHARSET:      return (config->ds_charset == NULL)? CFG_DS_CHARSET_DEFAULT: config->ds_charset;
    case CFG_QR_TEXT:         return config->qr_text;
    case CFG_OP_FILE:         return config->op_file;
    case CFG_OP_LOG:          return config->op_log;
    case CFG_OP_HEADER_STR:   return cfg_get_const_str(config->op_header);
    case CFG_OP_QUOTE_STR:    return cfg_get_const_str(config->op_quote);
    case CFG_OP_COLSEP:       return (config->op_colsep == NULL)? CFG_OP_COLSEP_DEFAULT: config->op_colsep;
    case CFG_OP_ROWSEP:       return (config->op_rowsep == NULL)? CFG_OP_ROWSEP_DEFAULT: config->op_rowsep;
    case CFG_OP_DATEFMT:      return (config->op_datefmt == NULL)? CFG_OP_DATEFMT_DEFAULT: config->op_datefmt;
    default:
        return NULL;
    }
}

void cfg_set_str(cfg_config_t* config, const int key, const char* value) {

    switch (key) {
    case CFG_DS_CHARSET:
        config->ds_charset = cfg_dup_string(value);
        break;
    case CFG_QR_TEXT:
        cfg_set_qr_text(config, value);
        break;
    case CFG_OP_FILE:
        config->op_file = cfg_dup_string(value);
        break;
    case CFG_OP_LOG:
        config->op_log = cfg_dup_string(value);
        break;
    case CFG_OP_HEADER_STR:
        cfg_set_op_header(config, value);
        break;
    case CFG_OP_QUOTE_STR:
        cfg_set_op_quote(config, value);
        break;
    case CFG_OP_COLSEP:
        config->op_colsep = cfg_dup_string(value);
        break;
    case CFG_OP_ROWSEP:
        config->op_rowsep = cfg_dup_string(value);
        break;
    case CFG_OP_DATEFMT:
        config->op_datefmt = cfg_dup_string(value);
        break;
    }
}

int cfg_get_int(cfg_config_t* config, const int key) {
    switch (key) {
    case CFG_DS_TYPE:        return config->ds_type;
    case CFG_DS_FETCHSIZE:   return config->ds_fetchsize;
    case CFG_QR_VAR_CNT:     return config->qr_var_cnt;
    case CFG_QR_PM_DEF_CNT:  return config->qr_pm_def_cnt;
    case CFG_QR_PM_VAL_CNT:  return config->qr_pm_val_cnt;
    case CFG_OP_HEADER:      return config->op_header;
    case CFG_OP_QUOTE:       return config->op_quote;
    default:
        return 0;
    }
}

void cfg_set_int(cfg_config_t* config, const int key, const int value) {

    switch (key) {
    case CFG_DS_FETCHSIZE:
        config->ds_fetchsize = value;
        break;
    case CFG_OP_HEADER:
        config->op_header = value;
        break;
    case CFG_OP_QUOTE:
        config->op_quote = value;
        break;
    }
}

void* cfg_get_obj(cfg_config_t* config, const int key) {

    switch (key) {
    case CFG_QR_PM_DEFS: return config->qr_pm_defs;
    case CFG_QR_PM_VALS: return config->qr_pm_vals;
    default:
        return NULL;
    }
}

void cfg_set_ds(cfg_config_t* config, const char* username, const char* password, const char* conn_str) {
/*
case CFG_DS_USERNAME:
case CFG_DS_PASSWORD:
case CFG_DS_TYPE:
case CFG_DS_CONN_STR:
*/
}

static char* cfg_dup_string(const char* str) {
    char* newstr = (char*)malloc(strlen(str) + 1);
    strcpy(newstr, str);
    return newstr;
}

static char* cfg_get_const_str(const int key) {
    return NULL;
}

static void cfg_set_qr_text(cfg_config_t* config, const char* qr_text) {
/*
CFG_QR_TEXT
CFG_QR_VAR_CNT
CFG_QR_PM_DEF_CNT
CFG_QR_PM_DEFS
*/
}

static void cfg_set_op_header(cfg_config_t* config, const char* op_header_str) {
/*
CFG_OP_HEADER
CFG_OP_HEADER_STR
*/
}

static void cfg_set_op_quote(cfg_config_t* config, const char* op_quote_str) {
/*
CFG_OP_QUOTE
CFG_OP_QUOTE_STR
*/
}
