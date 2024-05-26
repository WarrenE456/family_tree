#include <stdlib.h>
#include "headers/str.h"


void StrNew(Str* self)
{
    self->len = 0;
    self->reserved = 16;
    self->data = (char*)malloc(sizeof(char) * self->reserved);
}
void StrAppend(Str* self, char c)
{
    ++self->len;
    if (self->len + 1 > self->reserved) {
        self->reserved *= 2;
        self->data = realloc(self->data, self->reserved);
    }
    self->data[self->len - 1] = c;
    self->data[self->len] = '\0';
}
