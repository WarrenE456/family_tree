typedef struct {
    unsigned len;
    unsigned reserved;
    char* data;
} Str;

void StrNew(Str* self);
void StrAppend(Str* self, char c);
