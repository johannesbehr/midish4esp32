#ifdef __cplusplus
extern "C" {
#endif

typedef size_t (*writeDef)(const char *buffer, size_t size);
typedef size_t (*readBytesDef)( char *buffer, size_t length);
void mdep_desp_register(writeDef w, readBytesDef r);

#ifdef __cplusplus
}
#endif