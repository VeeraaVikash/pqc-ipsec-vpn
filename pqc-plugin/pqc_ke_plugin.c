#include "pqc_ke_plugin.h"
#include "pqc_ke_handler.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct private_pqc_ke_plugin_t private_pqc_ke_plugin_t;

struct private_pqc_ke_plugin_t {
    pqc_ke_plugin_t public;
    pqc_ke_handler_t *handler;
};

static char* get_name(pqc_ke_plugin_t *this)
{
    return "pqc-ke";
}

static void destroy_plugin(pqc_ke_plugin_t *this)
{
    private_pqc_ke_plugin_t *priv = (private_pqc_ke_plugin_t*)this;
    if (priv->handler) {
        priv->handler->destroy(priv->handler);
    }
    free(priv);
}

pqc_ke_plugin_t *pqc_ke_plugin_create()
{
    private_pqc_ke_plugin_t *this;
    
    this = malloc(sizeof(private_pqc_ke_plugin_t));
    if (!this) {
        return NULL;
    }
    
    memset(this, 0, sizeof(*this));
    
    this->public.get_name = get_name;
    this->public.destroy = destroy_plugin;
    this->handler = pqc_ke_handler_create();
    
    if (!this->handler) {
        free(this);
        return NULL;
    }

    printf("[PQC Plugin] Loaded - Kyber-768 ready\n");

    return &this->public;
}
