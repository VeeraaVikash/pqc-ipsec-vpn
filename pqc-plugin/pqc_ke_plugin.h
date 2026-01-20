#ifndef PQC_KE_PLUGIN_H_
#define PQC_KE_PLUGIN_H_

typedef struct pqc_ke_plugin_t pqc_ke_plugin_t;

struct pqc_ke_plugin_t {
    char* (*get_name)(pqc_ke_plugin_t *this);
    void (*destroy)(pqc_ke_plugin_t *this);
};

pqc_ke_plugin_t *pqc_ke_plugin_create();

#endif
